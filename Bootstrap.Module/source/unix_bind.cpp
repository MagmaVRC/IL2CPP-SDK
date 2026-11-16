#include <unix/module.hpp>

#include <atomic>
#include <cstdio>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace UNIx::detail {
namespace {

struct RowId { uint64_t id; uint64_t name_id; };

constexpr RowId kRowIds[kSymCount] = {
#define UNIX_EXPORT(ns, n, r, a, ...) \
    { ::UNIx::abi::sym_id<r (UNIX_CC*) a>(#ns "." #n), ::UNIx::abi::fnv(#ns "." #n) },
#define UNIX_REMOVED(ns, n, r, a, ...)
#define UNIX_ALIAS(ns, n, r, a, ...)
#include <unix/unix_exports.inc>
};

std::mutex         g_nodeMutex;
std::deque<Node>   g_nodePool;
std::vector<Node*> g_nodeFree;
std::unordered_multimap<unix_handle, Node*> g_nodeOwners;

struct PageLive { uint32_t live = 0; bool settled = false; };
std::mutex g_pageMutex;
std::unordered_map<unix_handle, PageLive> g_pageLive;

std::mutex g_loadMutex;
struct LoadReg { const void* value; std::function<void()> writeback; };
std::vector<LoadReg> g_loads;

std::atomic<uint64_t> g_seen[64];

bool MarkOnce(uint64_t id) noexcept {
    if (id == 0) return false;
    const std::size_t base = static_cast<std::size_t>(id) & 63u;
    for (std::size_t i = 0; i < 64; ++i) {
        std::atomic<uint64_t>& slot = g_seen[(base + i) & 63u];
        const uint64_t cur = slot.load(std::memory_order_relaxed);
        if (cur == id) return false;
        if (cur == 0) {
            uint64_t expected = 0;
            if (slot.compare_exchange_strong(expected, id, std::memory_order_relaxed)) return true;
            if (expected == id) return false;
        }
    }
    return false;
}

void Report(LogLevel level, const char* what, uint64_t id) noexcept {
    if (!g_bound.test(kSym_log_write)) return;
    char buf[80];
    const int n = std::snprintf(buf, sizeof(buf), "unix: %s export %016llx", what,
                                static_cast<unsigned long long>(id));
    if (n > 0)
        g_d.log_write(g_self, static_cast<uint32_t>(level),
                      unix_str{ buf, static_cast<uint32_t>(n) });
}

struct BindState {
    uint32_t denied[kSymCount];
    uint32_t denied_count;
    uint32_t notes;
};

void UNIX_CC NoteSink(void* ud, const unix_bind_note* note) {
    auto* st = static_cast<BindState*>(ud);
    if (!st || !note) return;
    ++st->notes;
    if (note->kind == unix_note_denied && note->req_index < kSymCount &&
        st->denied_count < kSymCount)
        st->denied[st->denied_count++] = note->req_index;
}

} // namespace

void MissingOnce(uint64_t sym_id) noexcept {
    if (MarkOnce(sym_id)) Report(LogLevel::Warning, "unbound", sym_id);
}

void DeniedOnce(uint64_t sym_id) noexcept {
    if (MarkOnce(sym_id)) Report(LogLevel::Warning, "denied", sym_id);
}

Node* AllocNodeRaw() {
    std::lock_guard<std::mutex> lock(g_nodeMutex);
    if (!g_nodeFree.empty()) {
        Node* n = g_nodeFree.back();
        g_nodeFree.pop_back();
        return n;
    }
    return &g_nodePool.emplace_back();
}

void FreeNode(Node* n) noexcept {
    if (!n) return;
    std::function<void(const void*)> fn;
    std::function<bool(const void*)> ret;
    {
        std::lock_guard<std::mutex> lock(g_nodeMutex);
        n->owner = unix_null;
        if (n->busy) { n->dead = true; return; }
        fn.swap(n->fn);
        ret.swap(n->ret);
        n->dead = false;
        g_nodeFree.push_back(n);
    }
}

bool EnterNode(Node* n) noexcept {
    std::lock_guard<std::mutex> lock(g_nodeMutex);
    ++n->busy;
    return !n->dead;
}

void LeaveNode(Node* n) noexcept {
    bool release = false;
    {
        std::lock_guard<std::mutex> lock(g_nodeMutex);
        if (n->busy) --n->busy;
        release = n->busy == 0 && n->dead;
    }
    if (release) FreeNode(n);
}

void PageTrack(unix_handle page) {
    if (page == unix_null) return;
    std::lock_guard<std::mutex> lock(g_pageMutex);
    ++g_pageLive[page].live;
}

void PageSettle(unix_handle page) noexcept {
    std::lock_guard<std::mutex> lock(g_pageMutex);
    const auto it = g_pageLive.find(page);
    if (it != g_pageLive.end()) it->second.settled = true;
}

bool PageUntrack(unix_handle page) noexcept {
    std::lock_guard<std::mutex> lock(g_pageMutex);
    const auto it = g_pageLive.find(page);
    if (it == g_pageLive.end()) return false;
    if (it->second.live > 1) { --it->second.live; return false; }
    const bool orphan = !it->second.settled;
    g_pageLive.erase(it);
    return orphan;
}

void OwnNode(unix_handle h, Node* n) {
    if (!n) return;
    if (h == unix_null) { FreeNode(n); return; }
    std::lock_guard<std::mutex> lock(g_nodeMutex);
    n->owner = h;
    g_nodeOwners.emplace(h, n);
}

void ReleaseOwned(unix_handle h) noexcept {
    if (h == unix_null) return;
    for (;;) {
        Node* n = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_nodeMutex);
            const auto it = g_nodeOwners.find(h);
            if (it == g_nodeOwners.end()) return;
            n = it->second;
            g_nodeOwners.erase(it);
        }
        FreeNode(n);
    }
}

void ForgetNode(Node* n) noexcept {
    if (!n) return;
    bool release = false;
    {
        std::lock_guard<std::mutex> lock(g_nodeMutex);
        if (n->owner == unix_null) {
            release = static_cast<bool>(n->fn) || static_cast<bool>(n->ret);
        } else {
            const auto [first, last] = g_nodeOwners.equal_range(n->owner);
            for (auto it = first; it != last; ++it) {
                if (it->second != n) continue;
                g_nodeOwners.erase(it);
                release = true;
                break;
            }
        }
    }
    if (release) FreeNode(n);
}

void Cancel(unix_handle h) {
    UNIX_USE(core, cancel);
    if (h == unix_null) return;
    g_d.core_cancel(g_self, h);
    ReleaseOwned(h);
}

void UNIX_CC TrampTask(void* ud) {
    auto* n = static_cast<Node*>(ud);
    if (!n) return;
    NodeScope s(n);
    if (s && n->fn) n->fn(nullptr);
}

bool UNIX_CC TrampPhoton(void* ud, const unix_photon_ev* ev) {
    auto* n = static_cast<Node*>(ud);
    if (!n) return true;
    NodeScope s(n);
    return (s && n->ret) ? n->ret(ev) : true;
}

bool UNIX_CC TrampAvail(void* ud) {
    auto* n = static_cast<Node*>(ud);
    if (!n) return true;
    NodeScope s(n);
    return (s && n->ret) ? n->ret(nullptr) : true;
}

void RegisterLoad(const void* value, std::function<void()> writeback) {
    if (!value) return;
    std::lock_guard<std::mutex> lock(g_loadMutex);
    for (LoadReg& r : g_loads) {
        if (r.value == value) { r.writeback = std::move(writeback); return; }
    }
    g_loads.push_back(LoadReg{ value, std::move(writeback) });
}

void UnregisterLoad(const void* value) noexcept {
    std::lock_guard<std::mutex> lock(g_loadMutex);
    for (std::size_t i = 0; i < g_loads.size(); ++i) {
        if (g_loads[i].value != value) continue;
        g_loads[i] = std::move(g_loads.back());
        g_loads.pop_back();
        return;
    }
}

void RunLoadWriteback() {
    std::vector<std::function<void()>> pending;
    {
        std::lock_guard<std::mutex> lock(g_loadMutex);
        pending.reserve(g_loads.size());
        for (const LoadReg& r : g_loads) pending.push_back(r.writeback);
    }
    for (const std::function<void()>& fn : pending) if (fn) fn();
}

bool BindAll(const unix_context* ctx, unix_self self) {
    if (!ctx || !ctx->bind) return false;

    // Function-local statics, never automatics: the VM path range-checks reqs and every out
    // against the guest's own data blob or its stack.
    static unix_bind_req reqs[kSymCount];
    static void*         outs[kSymCount];
    static BindState     st;

    for (uint32_t i = 0; i < kSymCount; ++i) {
        outs[i] = nullptr;
        reqs[i].id      = kRowIds[i].id;
        reqs[i].name_id = kRowIds[i].name_id;
        reqs[i].flags   = static_cast<uint32_t>(unix_req_optional)
                        | (g_required.test(i)  ? static_cast<uint32_t>(unix_req_required) : 0u)
                        | (g_used_syms.test(i) ? static_cast<uint32_t>(unix_req_report)   : 0u)
                        | (g_silent.test(i)    ? static_cast<uint32_t>(unix_req_silent)   : 0u);
        reqs[i]._pad    = 0;
        reqs[i].out     = &outs[i];
    }
    st.denied_count = 0;
    st.notes = 0;

    g_ctx  = ctx;
    g_self = self ? self : ctx->self;

    const uint32_t bound = ctx->bind(g_self, reqs, kSymCount,
                                     static_cast<uint32_t>(sizeof(unix_bind_req)),
                                     &NoteSink, &st);

    bool ok = bound != 0;
    for (uint32_t i = 0; i < kSymCount; ++i) {
        const bool live = outs[i] != nullptr;
        g_bound.set(i, live);
        if (!live && g_required.test(i)) ok = false;
    }
    for (uint32_t i = 0; i < st.denied_count; ++i) {
        const uint32_t k = st.denied[i];
        if (!outs[k]) outs[k] = kDeniedStubs[k];
    }

    // g_d last: a slot is callable from another thread the instant it stops being a stub.
    uint32_t slot = 0;
#define UNIX_EXPORT(ns, n, r, a, ...)                                             \
    if (outs[slot]) g_d.ns##_##n = reinterpret_cast<r (UNIX_CC*) a>(outs[slot]);  \
    ++slot;
#define UNIX_REMOVED(ns, n, r, a, ...)
#define UNIX_ALIAS(ns, n, r, a, ...)
#include <unix/unix_exports.inc>

    if (g_bound.test(kSym_log_write)) {
        char buf[128];
        const int n = std::snprintf(buf, sizeof(buf),
                                    "unix: bound %u/%u exports, %u bind notes",
                                    bound, static_cast<unsigned>(kSymCount), st.notes);
        if (n > 0)
            g_d.log_write(g_self, static_cast<uint32_t>(ok ? LogLevel::Log : LogLevel::Error),
                          unix_str{ buf, static_cast<uint32_t>(n) });
    }
    return ok;
}

} // namespace UNIx::detail
