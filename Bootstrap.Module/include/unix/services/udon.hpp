#pragma once
#ifndef UNIX_USE
#  error "<unix/services/udon.hpp> is included by <unix/module.hpp>; include that instead"
#endif

namespace UNIx {

using UdonInfo       = unix_udon_info;
using UdonEntry      = unix_udon_entry;
using UdonParam      = unix_udon_param;
using UdonVar        = unix_udon_var;
using UdonValue      = unix_udon_value;
using UdonArg        = unix_udon_arg;
using UdonInput      = unix_udon_input;
using UdonEvent      = unix_udon_ev;
using UdonNetEvent   = unix_udon_net_ev;

/// <summary>Who a networked Udon event reaches. Matches VRChat's NetworkEventTarget, which has
/// no per-player value: one named player needs Udon().RpcToPlayer.</summary>
enum class NetTarget : uint32_t {
    All = unix_udon_all, Owner = unix_udon_owner, Others = unix_udon_others, Self = unix_udon_self,
};

/// <summary>Who an RPC reaches. Wider than NetTarget, and the only path to one named player.</summary>
enum class RpcTarget : uint32_t {
    All = unix_rpc_all, Others = unix_rpc_others, Owner = unix_rpc_owner, Master = unix_rpc_master,
    AllBuffered = unix_rpc_all_buffered, OthersBuffered = unix_rpc_others_buffered,
    Local = unix_rpc_local, AllBufferOne = unix_rpc_all_buffer_one,
    OthersBufferOne = unix_rpc_others_buffer_one, TargetPlayer = unix_rpc_target_player,
};

enum class UdonLifecycle : uint32_t {
    Init = unix_udon_init, RequestSerialization = unix_udon_serialize,
};

/// <summary>Build a value to pass as an event or RPC argument. `type` forces the managed type it
/// is parsed as; leaving it empty lets the text decide.</summary>
[[nodiscard]] inline UdonValue UdonText(std::string_view text, std::string_view type = {}) {
    UdonValue v{};
    v.size = sizeof(v);
    v.kind = unix_udon_v_string;
    const size_t n = text.size() < sizeof(v.text) - 1 ? text.size() : sizeof(v.text) - 1;
    for (size_t i = 0; i < n; ++i) v.text[i] = text[i];
    const size_t t = type.size() < sizeof(v.type_name) - 1 ? type.size() : sizeof(v.type_name) - 1;
    for (size_t i = 0; i < t; ++i) v.type_name[i] = type[i];
    return v;
}

[[nodiscard]] inline UdonArg UdonNamed(std::string_view name, std::string_view text,
                                       std::string_view type = {}) {
    UdonArg a{};
    a.size = sizeof(a);
    const size_t n = name.size() < sizeof(a.name) - 1 ? name.size() : sizeof(a.name) - 1;
    for (size_t i = 0; i < n; ++i) a.name[i] = name[i];
    a.value = UdonText(text, type);
    return a;
}

namespace detail {
inline bool UNIX_CC TrampUdonNet(void* ud, const unix_udon_net_ev* ev) {
    auto* n = static_cast<Node*>(ud);
    // A registration whose closure is gone must not silently swallow the world's traffic.
    return (n && n->ret && ev) ? n->ret(ev) : true;
}
}

/// <summary>The world's Udon scripts: what they declare, what they hold, and driving them.
/// Every call must be on the Unity main thread; VRChat's own networking throws otherwise.
/// A behaviour is a live UdonBehaviour pointer, valid only while the object is.</summary>
class Udon {
public:
    /// <summary>Every UdonBehaviour in the scene whose script or object name contains `filter`.
    /// An empty filter returns them all.</summary>
    template <class = void> [[nodiscard]] std::vector<void*> Find(std::string_view filter = {}) const {
        UNIX_USE(udon, find);
        std::vector<void*> out;
        const uint32_t need = detail::g_d.udon_find(detail::g_self, Sv(filter), nullptr, 0);
        if (need == unix_absent || need == unix_denied || need == 0) return out;
        out.resize(need);
        const uint32_t got = detail::g_d.udon_find(detail::g_self, Sv(filter), out.data(), need);
        out.resize(got == unix_absent || got == unix_denied || got > need ? 0 : got);
        return out;
    }

    /// <summary>The UdonBehaviours on one GameObject.</summary>
    template <class = void> [[nodiscard]] std::vector<void*> OnObject(void* gameObject) const {
        UNIX_USE(udon, on_object);
        std::vector<void*> out;
        const uint32_t need = detail::g_d.udon_on_object(detail::g_self, gameObject, nullptr, 0);
        if (need == unix_absent || need == unix_denied || need == 0) return out;
        out.resize(need);
        const uint32_t got = detail::g_d.udon_on_object(detail::g_self, gameObject, out.data(), need);
        out.resize(got == unix_absent || got == unix_denied || got > need ? 0 : got);
        return out;
    }

    template <class = void> [[nodiscard]] std::optional<UdonInfo> GetInfo(void* behaviour) const {
        UNIX_USE(udon, info);
        UdonInfo i{};
        i.size = sizeof(i);
        if (detail::g_d.udon_info(detail::g_self, behaviour, &i) != unix_ok) return std::nullopt;
        return i;
    }

    /// <summary>Every entry point, with whether a networked send would actually be delivered.</summary>
    template <class = void> [[nodiscard]] std::vector<UdonEntry> GetEntryPoints(void* behaviour) const {
        UNIX_USE(udon, entrypoints);
        return detail::ReadArray<UdonEntry>([&](UdonEntry* b, uint32_t cap, uint32_t stride) {
            return detail::g_d.udon_entrypoints(detail::g_self, behaviour, b, cap, stride);
        });
    }

    /// <summary>The ordered parameters of a [NetworkCallable] entry point.</summary>
    template <class = void> [[nodiscard]] std::vector<UdonParam>
    GetEntryParams(void* behaviour, std::string_view event) const {
        UNIX_USE(udon, entry_params);
        return detail::ReadArray<UdonParam>([&](UdonParam* b, uint32_t cap, uint32_t stride) {
            return detail::g_d.udon_entry_params(detail::g_self, behaviour, Sv(event), b, cap, stride);
        });
    }

    /// <summary>The hash VRChat puts on the wire for an entry point.</summary>
    template <class = void> [[nodiscard]] std::optional<uint32_t>
    GetEntryHash(void* behaviour, std::string_view event) const {
        UNIX_USE(udon, entry_hash);
        uint32_t hash = 0;
        if (detail::g_d.udon_entry_hash(detail::g_self, behaviour, Sv(event), &hash) != unix_ok)
            return std::nullopt;
        return hash;
    }

    /// <summary>Resolve an entry-point hash seen on the wire back to its name.</summary>
    template <class = void> [[nodiscard]] std::string GetEntryName(void* behaviour, uint32_t hash) const {
        UNIX_USE(udon, entry_name);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.udon_entry_name(detail::g_self, behaviour, hash, b, c);
        });
    }

    template <class = void> [[nodiscard]] std::vector<UdonVar> GetVariables(void* behaviour) const {
        UNIX_USE(udon, variables);
        return detail::ReadArray<UdonVar>([&](UdonVar* b, uint32_t cap, uint32_t stride) {
            return detail::g_d.udon_variables(detail::g_self, behaviour, b, cap, stride);
        });
    }

    template <class = void> [[nodiscard]] std::optional<UdonValue>
    GetVar(void* behaviour, std::string_view symbol) const {
        UNIX_USE(udon, get_var);
        UdonValue v{};
        v.size = sizeof(v);
        if (detail::g_d.udon_get_var(detail::g_self, behaviour, Sv(symbol), &v) != unix_ok)
            return std::nullopt;
        return v;
    }

    /// <summary>Write one program variable. Session-only, and a synced behaviour can desync.</summary>
    template <class = void> Status SetVar(void* behaviour, std::string_view symbol, const UdonValue& value) {
        UNIX_USE(udon, set_var);
        return detail::g_d.udon_set_var(detail::g_self, behaviour, Sv(symbol), &value);
    }
    template <class = void> Status SetVar(void* behaviour, std::string_view symbol, std::string_view text) {
        return SetVar(behaviour, symbol, UdonText(text));
    }

    /// <summary>Run an entry point on this client only.</summary>
    template <class = void> Status SendEvent(void* behaviour, std::string_view event) {
        UNIX_USE(udon, send_event);
        return detail::g_d.udon_send_event(detail::g_self, behaviour, Sv(event));
    }

    /// <summary>Run an entry point on this client after a delay. Give seconds OR frames.</summary>
    /// <param name="timing">unix_udon EventTiming: 0 Update, 1 LateUpdate, 2 PostLateUpdate, 3 FixedUpdate.</param>
    template <class = void> Status SendEventDelayed(void* behaviour, std::string_view event,
                                                    float seconds, int32_t frames = 0, uint32_t timing = 0) {
        UNIX_USE(udon, send_event_delayed);
        return detail::g_d.udon_send_event_delayed(detail::g_self, behaviour, Sv(event),
                                                   seconds, frames, timing);
    }

    /// <summary>Whether VRChat would actually deliver a networked send of this event. A reserved
    /// event such as _interact is local-only however it is sent, and the send is refused rather
    /// than dropped in silence.</summary>
    template <class = void> [[nodiscard]] bool CanSendNetworkEvent(void* behaviour,
                                                                   std::string_view event) const {
        UNIX_USE(udon, can_send_network_event);
        return detail::g_d.udon_can_send_network_event(detail::g_self, behaviour, Sv(event));
    }

    /// <summary>Send an entry point to the instance, with up to eight arguments for an event
    /// declared [NetworkCallable]. VRChat rate-limits these per entry point.</summary>
    template <class = void> Status SendNetworkEvent(void* behaviour, NetTarget target,
                                                    std::string_view event,
                                                    std::span<const UdonValue> params = {}) {
        UNIX_USE(udon, send_network_event);
        return detail::g_d.udon_send_network_event(detail::g_self, behaviour, (uint32_t)target,
                                                   Sv(event), params.data(), (uint32_t)params.size());
    }

    /// <summary>Run an entry point locally and report whether it ran, which SendEvent cannot.</summary>
    template <class = void> bool RunEvent(void* behaviour, std::string_view event,
                                          bool canRunBeforeStart = false) {
        UNIX_USE(udon, run_event);
        return detail::g_d.udon_run_event(detail::g_self, behaviour, Sv(event), canRunBeforeStart);
    }

    /// <summary>Run an entry point locally with named arguments, which VRChat binds to the
    /// program's own parameter symbols for the call. Unlike writing the heap first, nothing else
    /// can observe or clobber the values in between.</summary>
    /// <param name="mangleNames">Map each name the way an UdonSharp event parameter expects.</param>
    template <class = void> Status RunEventWith(void* behaviour, std::string_view event,
                                                std::span<const UdonArg> args,
                                                bool mangleNames = true, bool canRunBeforeStart = false) {
        UNIX_USE(udon, run_event_params);
        return detail::g_d.udon_run_event_params(detail::g_self, behaviour, Sv(event),
                                                 args.data(), (uint32_t)args.size(),
                                                 mangleNames, canRunBeforeStart);
    }

    /// <summary>Run an entry point by name, bypassing the event table SendEvent walks.</summary>
    template <class = void> Status RunProgram(void* behaviour, std::string_view event) {
        UNIX_USE(udon, run_program);
        return detail::g_d.udon_run_program(detail::g_self, behaviour, Sv(event));
    }

    /// <summary>Run whatever starts at a code address. No name lookup and no validation: an
    /// address that is not an entry point runs the program from the middle.</summary>
    template <class = void> Status RunProgramAt(void* behaviour, uint32_t address) {
        UNIX_USE(udon, run_program_at);
        return detail::g_d.udon_run_program_at(detail::g_self, behaviour, address);
    }

    /// <summary>Deliver an input event (_inputJump, _inputUse, ...) with its event args.</summary>
    template <class = void> Status RunInputEvent(void* behaviour, std::string_view event,
                                                 const UdonInput& input) {
        UNIX_USE(udon, run_input_event);
        return detail::g_d.udon_run_input_event(detail::g_self, behaviour, Sv(event), &input);
    }

    template <class = void> Status RequestSerialization(void* behaviour) {
        UNIX_USE(udon, request_serialization);
        return detail::g_d.udon_request_serialization(detail::g_self, behaviour);
    }

    /// <summary>The behaviour's master switch. With event processing off it runs nothing, local
    /// or networked, and stays loaded.</summary>
    template <class = void> Status SetEventProcessing(void* behaviour, bool enabled) {
        UNIX_USE(udon, set_event_processing);
        return detail::g_d.udon_set_event_processing(detail::g_self, behaviour, enabled);
    }

    template <class = void> Status SetInteractive(void* behaviour, bool interactive) {
        UNIX_USE(udon, set_interactive);
        return detail::g_d.udon_set_interactive(detail::g_self, behaviour, interactive);
    }

    /// <param name="syncMethod">0 Unknown, 1 None, 2 Continuous, 3 Manual.</param>
    template <class = void> Status SetSyncMethod(void* behaviour, uint32_t syncMethod) {
        UNIX_USE(udon, set_sync_method);
        return detail::g_d.udon_set_sync_method(detail::g_self, behaviour, syncMethod);
    }

    /// <summary>Whether the code running right now was entered from a networked event.</summary>
    template <class = void> [[nodiscard]] bool InNetworkCall() const {
        UNIX_USE(udon, in_network_call);
        return detail::g_d.udon_in_network_call(detail::g_self);
    }

    /// <summary>The VRCPlayerApi that sent the networked event being handled, or null.</summary>
    template <class = void> [[nodiscard]] void* CallingPlayer() const {
        UNIX_USE(udon, calling_player);
        return detail::g_d.udon_calling_player(detail::g_self);
    }

    /// <summary>Networked sends the rate limiter is still holding. Non-zero means sending harder
    /// achieves nothing. A null behaviour counts every queued send this client holds.</summary>
    template <class = void> [[nodiscard]] int32_t QueuedSends(void* behaviour = nullptr,
                                                              std::string_view event = {}) const {
        UNIX_USE(udon, queued_events);
        return detail::g_d.udon_queued_events(detail::g_self, behaviour, Sv(event));
    }

    /// <summary>Run fn with VRChat's calling-player context set, so an entry point that reads
    /// the calling player sees this one. Synchronous.</summary>
    template <class = void> Status WithCallingPlayer(void* vrcPlayerApi, std::function<void()> fn) {
        UNIX_USE(udon, with_calling_player);
        detail::Node* n = detail::NodeForTask(std::move(fn));
        const unix_status s = detail::g_d.udon_with_calling_player(detail::g_self, vrcPlayerApi,
                                                                   &detail::TrampTask, n);
        detail::FreeNode(n);
        return s;
    }

    /// <summary>Fires as behaviours finish loading, or ask for serialization. VRChat's own hook,
    /// so nothing has to poll FindObjectsOfType for behaviours that have appeared.</summary>
    template <class = void> [[nodiscard]] Subscription
    OnLifecycle(UdonLifecycle kind, std::function<void(const UdonEvent&)> fn) {
        UNIX_USE(udon, on_lifecycle);
        detail::Node* n = detail::NodeFor<unix_udon_ev>(std::move(fn));
        const unix_handle h = detail::g_d.udon_on_lifecycle(detail::g_self, (uint32_t)kind,
                                                            &detail::Tramp<unix_udon_ev>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }

    /// <summary>Every networked Udon event crossing this client, inbound and outbound, before it
    /// is delivered or sent. Return false to DROP it: nothing runs locally and nothing goes out.
    /// This is authority over other people's traffic, so the row also needs the Photon veto
    /// capability.</summary>
    template <class = void> [[nodiscard]] Subscription
    OnNetworkEvent(std::function<bool(const UdonNetEvent&)> fn) {
        UNIX_USE(udon, on_network_event);
        detail::Node* n = detail::NodeForVeto<unix_udon_net_ev>(std::move(fn));
        const unix_handle h = detail::g_d.udon_on_network_event(detail::g_self,
                                                                &detail::TrampUdonNet, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }

    /// <summary>VRChat's RPC path: a wider audience list than a networked Udon event, and the
    /// only one that reaches a single named player.</summary>
    /// <param name="targetObject">The GameObject carrying the receiving method.</param>
    template <class = void> Status Rpc(RpcTarget target, void* targetObject, std::string_view method,
                                       std::span<const UdonValue> params = {}) {
        UNIX_USE(udon, rpc);
        return detail::g_d.udon_rpc(detail::g_self, (uint32_t)target, targetObject, Sv(method),
                                    params.data(), (uint32_t)params.size());
    }

    /// <summary>An RPC to exactly one player.</summary>
    template <class = void> Status RpcToPlayer(void* vrcPlayerApi, void* targetObject,
                                               std::string_view method,
                                               std::span<const UdonValue> params = {}) {
        UNIX_USE(udon, rpc_to_player);
        return detail::g_d.udon_rpc_to_player(detail::g_self, vrcPlayerApi, targetObject,
                                              Sv(method), params.data(), (uint32_t)params.size());
    }
};

} // namespace UNIx
