#include <BindMiss.hpp>

#include <cstring>

namespace IL2CPP::Module {

    namespace {

        struct Entry {
            char    klass[BindMiss::kNameMax];
            char    member[BindMiss::kNameMax];
            int32_t argc;
        };

        // Namespace-scope PODs: Record runs inside magic-static initialisers,
        // where a function-local static would recurse on its own guard.
        Entry                 g_entries[BindMiss::kCapacity]{};
        std::atomic<uint32_t> g_total{ 0 };
        std::atomic<uint32_t> g_retained{ 0 };

        void CopyBounded(char* dst, std::string_view src) noexcept {
            const size_t n = src.size() < BindMiss::kNameMax - 1 ? src.size() : BindMiss::kNameMax - 1;
            std::memcpy(dst, src.data(), n);
            dst[n] = '\0';
        }

        bool SameBounded(const char* stored, std::string_view src) noexcept {
            const size_t n = src.size() < BindMiss::kNameMax - 1 ? src.size() : BindMiss::kNameMax - 1;
            return std::strncmp(stored, src.data(), n) == 0 && stored[n] == '\0';
        }

        bool AlreadyRetained(std::string_view klass, std::string_view member, int argc) noexcept {
            uint32_t n = g_retained.load(std::memory_order_relaxed);
            if (n > BindMiss::kCapacity) n = BindMiss::kCapacity;
            for (uint32_t i = 0; i < n; ++i) {
                const Entry& e = g_entries[i];
                if (e.argc == argc && SameBounded(e.member, member) && SameBounded(e.klass, klass))
                    return true;
            }
            return false;
        }

    }

    void BindMiss::Record(std::string_view klass, std::string_view member, int argc) noexcept {
        g_total.fetch_add(1, std::memory_order_relaxed);
        if (AlreadyRetained(klass, member, argc)) return;

        const uint32_t slot = g_retained.fetch_add(1, std::memory_order_relaxed);
        if (slot >= kCapacity) {
            g_retained.store(kCapacity, std::memory_order_relaxed);
            return;
        }

        CopyBounded(g_entries[slot].klass, klass);
        CopyBounded(g_entries[slot].member, member);
        g_entries[slot].argc = argc;
    }

    uint32_t BindMiss::Count() noexcept {
        return g_total.load(std::memory_order_relaxed);
    }

    uint32_t BindMiss::Retained() noexcept {
        const uint32_t n = g_retained.load(std::memory_order_relaxed);
        return n < kCapacity ? n : kCapacity;
    }

    std::string BindMiss::Report() {
        std::string out;
        const uint32_t n = Retained();
        for (uint32_t i = 0; i < n; ++i) {
            out += g_entries[i].klass;
            out += "::";
            out += g_entries[i].member;
            out += '/';
            out += std::to_string(g_entries[i].argc);
            out += '\n';
        }
        return out;
    }

    void BindMiss::Reset() noexcept {
        g_total.store(0, std::memory_order_relaxed);
        g_retained.store(0, std::memory_order_relaxed);
    }

    std::string SelfTest() {
        const uint32_t total = BindMiss::Count();
        if (total == 0) return "bindings ok";

        const uint32_t retained = BindMiss::Retained();
        std::string out = std::to_string(total) + " binding(s) missed";
        if (total > retained) out += " (first " + std::to_string(retained) + " retained)";
        out += ":\n";
        out += BindMiss::Report();
        return out;
    }

}
