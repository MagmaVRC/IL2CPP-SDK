#pragma once
#ifndef UNIX_USE
#  error "<unix/services/players.hpp> is included by <unix/module.hpp>; include that instead"
#endif

namespace UNIx {

/// <summary>Player lifecycle in the instance, from NetworkManager's own join/leave methods.
/// Per actor per room, each phase fires at most once and in order:
/// EnteredRoom, Joining, Joined, JoinComplete, LeftRoom, Left. Joined fires once the name,
/// VRCPlayerApi and actor id are readable. Every player that got Joining gets exactly one
/// Left, while its object is still alive (except reason Destroyed). Unity main thread.</summary>
class PlayerManager {
public:
    using Callback = std::function<void(const PlayerLifecycleEvent&)>;

    /// <summary>A Photon actor entered the room. Earliest join; no VRChat Player yet.</summary>
    template <class = void> [[nodiscard]] Subscription OnEnteredRoom(Callback fn) { return On(unix_player_entered_room, std::move(fn)); }
    /// <summary>The VRChat Player exists and was handed to persistence. Name may still be empty.</summary>
    template <class = void> [[nodiscard]] Subscription OnJoining(Callback fn) { return On(unix_player_awake, std::move(fn)); }
    /// <summary>The player is registered and readable.</summary>
    template <class = void> [[nodiscard]] Subscription OnJoined(Callback fn) { return On(unix_player_joined, std::move(fn)); }
    /// <summary>The player is fully set up. Never before OnJoined.</summary>
    template <class = void> [[nodiscard]] Subscription OnJoinComplete(Callback fn) { return On(unix_player_joined_complete, std::move(fn)); }
    /// <summary>The Photon actor left. Earliest leave; the player object is still alive.</summary>
    template <class = void> [[nodiscard]] Subscription OnLeftRoom(Callback fn) { return On(unix_player_left_room, std::move(fn)); }
    /// <summary>The player is about to be torn down. See PlayerLifecycleEvent::reason.</summary>
    template <class = void> [[nodiscard]] Subscription OnLeft(Callback fn) { return On(unix_player_left, std::move(fn)); }

    /// <summary>Every tracked player still alive.</summary>
    template <class = void> [[nodiscard]] std::vector<Player> All() const {
        UNIX_USE(players, all);
        const uint32_t need = detail::g_d.players_all(detail::g_self, nullptr, 0);
        if (need == 0 || need >= 0xFFFFFF00u) return {};
        std::vector<void*> raw(need);
        const uint32_t got = detail::g_d.players_all(detail::g_self, raw.data(), need);
        if (got == 0 || got > need) return {};
        std::vector<Player> out;
        out.reserve(got);
        for (uint32_t i = 0; i < got; ++i) out.emplace_back(raw[i]);
        return out;
    }

    template <class = void> [[nodiscard]] Player Local() const {
        UNIX_USE(players, local);
        return Player{ detail::g_d.players_local(detail::g_self) };
    }

    /// <summary>The VRChat Player for a Photon actor, or an empty Player.</summary>
    template <class = void> [[nodiscard]] Player ByActor(int32_t actorId) const {
        UNIX_USE(players, by_actor);
        return Player{ detail::g_d.players_by_actor(detail::g_self, actorId) };
    }

    /// <summary>A player's Photon actor number, read from the game's own field; 0 when unknown.</summary>
    template <class = void> [[nodiscard]] int32_t ActorOf(Player p) const {
        UNIX_USE(players, actor_of);
        return detail::g_d.players_actor_of(detail::g_self, p.Raw());
    }

    template <class = void> [[nodiscard]] PlayerStage StageOf(Player p) const {
        UNIX_USE(players, stage);
        return static_cast<PlayerStage>(detail::g_d.players_stage(detail::g_self, p.Raw()));
    }

private:
    template <class = void> [[nodiscard]] Subscription On(unix_player_phase phase, Callback fn) {
        UNIX_USE(players, on_event);
        if (!fn) return {};
        return detail::SubscribePlayer(detail::PhaseBit(phase),
            [f = std::move(fn)](const PlayerEvent& e) { f(PlayerLifecycleEvent::From(e)); });
    }
};

}   // namespace UNIx
