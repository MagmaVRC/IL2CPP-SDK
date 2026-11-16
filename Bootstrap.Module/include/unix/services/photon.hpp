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
};

}   // namespace UNIx
