#pragma once
#ifndef UNIX_USE
#  error "<unix/services/photon.hpp> is included by <unix/module.hpp>; include that instead"
#endif

namespace UNIx {

/// <summary>Photon events crossing this client. Callbacks run on the Photon thread.</summary>
class Photon {
public:
    /// <summary>See each Photon event before the game does. Returning false drops it game-wide.</summary>
    template <class = void> [[nodiscard]] Subscription Intercept(std::function<bool(const PhotonEvent&)> fn) {
        UNIX_USE(photon, intercept);
        if (!fn) return {};
        detail::Node* n = detail::NodeForVeto<unix_photon_ev>(std::move(fn));
        const unix_handle h = detail::g_d.photon_intercept(detail::g_self, &detail::TrampPhoton, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }

    /// <summary>Whether the local client is in a Photon room.</summary>
    template <class = void> [[nodiscard]] bool InRoom() const {
        UNIX_USE(photon, in_room);
        return detail::g_d.photon_in_room(detail::g_self);
    }
    /// <summary>The local actor number, 0 outside a room.</summary>
    template <class = void> [[nodiscard]] int32_t LocalActor() const {
        UNIX_USE(photon, local_actor);
        return detail::g_d.photon_local_actor(detail::g_self);
    }
    /// <summary>The master client's actor number, 0 outside a room.</summary>
    template <class = void> [[nodiscard]] int32_t MasterActor() const {
        UNIX_USE(photon, master_actor);
        return detail::g_d.photon_master_actor(detail::g_self);
    }
    /// <summary>Actors in the room the host has seen enter and not leave.</summary>
    template <class = void> [[nodiscard]] uint32_t ActorCount() const {
        UNIX_USE(photon, actor_count);
        return detail::g_d.photon_actor_count(detail::g_self);
    }
};

}   // namespace UNIx
