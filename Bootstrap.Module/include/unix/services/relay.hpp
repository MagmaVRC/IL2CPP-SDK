#pragma once
#ifndef UNIX_USE
#  error "<unix/services/relay.hpp> is included by <unix/module.hpp>; include that instead"
#endif

namespace UNIx {

using RelayMessage     = unix_relay_msg_ev;
using RelayStreamValue = unix_relay_stream_ev;
using RelayPeer        = unix_relay_peer;

enum class RelayState : uint32_t {
    Down = unix_relay_down, Connecting = unix_relay_connecting,
    NoMessaging = unix_relay_no_messaging, Ready = unix_relay_ready,
};

enum class RelayResult : uint32_t {
    Delivered = unix_relay_delivered, Expired = unix_relay_expired, Dropped = unix_relay_dropped,
};

/// <summary>The lowest channel a module may use; lower channels are the client's own.</summary>
inline constexpr uint16_t kRelayModuleChannelMin = static_cast<uint16_t>(unix_relay_module_channel_min);

/// <summary>The body of a relay message or stream value, valid only inside the callback.</summary>
template <class Ev>
[[nodiscard]] inline std::span<const uint8_t> RelayBody(const Ev& e) noexcept {
    return e.len ? std::span<const uint8_t>(e.data, e.len) : std::span<const uint8_t>();
}

/// <summary>The relay-stamped VRChat user id of the sender, valid only inside the callback.</summary>
template <class Ev>
[[nodiscard]] inline std::string_view RelaySender(const Ev& e) noexcept { return View(e.user_id); }

/// <summary>How one SendUser is delivered.</summary>
struct RelaySendOptions {
    /// <summary>False sends a datagram: no ack, no resend, no result.</summary>
    bool reliable = true;
    /// <summary>Give up after this long; 0 is the host default of 10 s, and 60 s is the cap.</summary>
    uint32_t ttlMs = 0;
    /// <summary>Non-zero drops an older pending send to the same user, channel and key.</summary>
    uint32_t replaceKey = 0;
    /// <summary>Runs once on the Unity main thread for a reliable send the host accepted.</summary>
    std::function<void(RelayResult)> onResult;
};

/// <summary>How a channel subscription filters and reports.</summary>
struct RelaySubOptions {
    /// <summary>Runs before the message is acked. False refuses it, so the sender resends;
    /// this is where a consumer checks a signature. `reliable` is not set in the event it sees.</summary>
    std::function<bool(const RelayMessage&)> accept;
    /// <summary>Runs once if the host revokes the subscription after repeated faults.</summary>
    std::function<void()> onRevoked;
    /// <summary>Deliver messages from a sender the relay marks disputed.</summary>
    bool allowDisputed = false;
};

namespace detail {
inline void UNIX_CC TrampRelayMsg(void* ud, const unix_relay_msg_ev* ev) {
    auto* n = static_cast<Node*>(ud);
    if (!n || !ev) return;
    NodeScope s(n);
    if (s && n->fn) n->fn(ev);
}
inline bool UNIX_CC TrampRelayAccept(void* ud, const unix_relay_msg_ev* ev) {
    auto* n = static_cast<Node*>(ud);
    if (!n || !ev) return false;
    NodeScope s(n);
    return s && n->ret && n->ret(ev);
}
inline void UNIX_CC TrampRelayRevoked(void* ud) {
    auto* n = static_cast<Node*>(ud);
    if (!n) return;
    NodeScope s(n);
    if (s && n->fn) n->fn(nullptr);
}
inline void UNIX_CC TrampRelayResult(void* ud, unix_relay_result r) {
    auto* n = static_cast<Node*>(ud);
    if (!n) return;
    if (n->fn) n->fn(&r);
    FreeNode(n);
}
}   // namespace detail

/// <summary>Client-to-client messages over the UNIx relay link, to one user anywhere the link
/// reaches them or to everyone in this relay room. Every call is on the Unity main thread, and
/// so is every callback. Channels start at kRelayModuleChannelMin, one subscriber per channel
/// across the whole client.</summary>
class Relay {
public:
    template <class = void> [[nodiscard]] RelayState State() const {
        UNIX_USE(relay, state);
        return static_cast<RelayState>(detail::g_d.relay_state(detail::g_self));
    }

    template <class = void> [[nodiscard]] bool IsReady() const { return State() == RelayState::Ready; }

    /// <summary>Receive user and room messages on one channel.</summary>
    /// <returns>An empty Subscription when the channel is taken, below the module range, or the
    /// relay link is not available in this client.</returns>
    template <class = void> [[nodiscard]] Subscription
    Subscribe(uint16_t channel, std::function<void(const RelayMessage&)> fn, RelaySubOptions options = {}) {
        UNIX_USE(relay, subscribe);
        if (!fn) return {};
        detail::Node* n = detail::AllocNodeRaw();
        n->fn = [g = std::move(fn), r = std::move(options.onRevoked)](const void* p) {
            if (p) g(*static_cast<const RelayMessage*>(p));
            else if (r) r();
        };
        unix_relay_sub_desc d{};
        d.size       = sizeof(d);
        d.channel    = channel;
        d.flags      = options.allowDisputed ? unix_relay_sub_allow_disputed : 0u;
        d.on_message = &detail::TrampRelayMsg;
        if (options.accept) {
            n->ret = [a = std::move(options.accept)](const void* p) {
                return a(*static_cast<const RelayMessage*>(p));
            };
            d.accept = &detail::TrampRelayAccept;
        }
        d.on_revoked = &detail::TrampRelayRevoked;
        d.ud         = n;
        const unix_handle h = detail::g_d.relay_subscribe(detail::g_self, &d);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }

    /// <summary>Receive the stream values room members publish on one channel, one per key.</summary>
    template <class = void> [[nodiscard]] Subscription
    SubscribeStream(uint16_t channel, std::function<void(const RelayStreamValue&)> fn) {
        UNIX_USE(relay, subscribe_stream);
        if (!fn) return {};
        detail::Node* n = detail::NodeFor<unix_relay_stream_ev>(std::move(fn));
        const unix_handle h = detail::g_d.relay_subscribe_stream(detail::g_self, channel,
                                                                 &detail::Tramp<unix_relay_stream_ev>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }

    /// <summary>Send to one VRChat user's live session, in any room.</summary>
    /// <returns>False when the link is not ready, the body is over the relay limit or this
    /// client's send budget is spent.</returns>
    template <class = void> bool SendUser(std::string_view userId, uint16_t channel,
                                          std::span<const uint8_t> body, RelaySendOptions options = {}) {
        UNIX_USE(relay, send_user);
        unix_relay_send_desc d{};
        d.size        = sizeof(d);
        d.flags       = options.reliable ? 0u : unix_relay_send_datagram;
        d.ttl_ms      = options.ttlMs;
        d.replace_key = options.replaceKey;
        detail::Node* n = nullptr;
        if (options.reliable && options.onResult) {
            n = detail::AllocNode([g = std::move(options.onResult)](const void* p) {
                g(static_cast<RelayResult>(*static_cast<const unix_relay_result*>(p)));
            });
            d.on_result = &detail::TrampRelayResult;
            d.ud        = n;
        }
        const unix_status s = detail::g_d.relay_send_user(detail::g_self, Sv(userId), channel,
                                                          body.data(), static_cast<uint32_t>(body.size()), &d);
        if (s != unix_ok && n) detail::FreeNode(n);
        return s == unix_ok;
    }

    /// <summary>Send a datagram to everyone in this relay room.</summary>
    template <class = void> bool SendRoom(uint16_t channel, std::span<const uint8_t> body) {
        UNIX_USE(relay, send_room);
        return detail::g_d.relay_send_room(detail::g_self, channel, body.data(),
                                           static_cast<uint32_t>(body.size())) == unix_ok;
    }

    /// <summary>Publish this client's latest value for one key on a room stream; an empty
    /// body clears it. Members who join later receive the current value.</summary>
    template <class = void> bool SetStream(uint16_t channel, uint8_t key, std::span<const uint8_t> body) {
        UNIX_USE(relay, set_stream);
        return detail::g_d.relay_set_stream(detail::g_self, channel, key, body.data(),
                                            static_cast<uint32_t>(body.size())) == unix_ok;
    }

    /// <summary>A member of the current relay room by VRChat user id.</summary>
    template <class = void> [[nodiscard]] std::optional<RelayPeer> FindPeer(std::string_view userId) const {
        UNIX_USE(relay, find_peer);
        RelayPeer p{};
        p.size = sizeof(p);
        if (detail::g_d.relay_find_peer(detail::g_self, Sv(userId), &p) != unix_ok) return std::nullopt;
        return p;
    }
};

}   // namespace UNIx
