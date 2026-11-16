#pragma once

#include <unix/module.hpp>

#include <VRChat/VRChat.hpp>
#include <IL2CPP.Module/include/Unity/Transform.hpp>

#include <cmath>

#ifdef GetObject
#undef GetObject
#endif

namespace UNIx {

    /// <summary>The typed VRChat object behind a player.</summary>
    [[nodiscard]] inline IL2CPP::VRChat::Player ToVrc(Player p) noexcept {
        return IL2CPP::VRChat::Player(p.Raw());
    }

    /// <summary>The SDK player for a typed VRChat object.</summary>
    [[nodiscard]] inline Player FromVrc(IL2CPP::VRChat::Player p) noexcept {
        return Player(p.raw());
    }

    /// <summary>The Photon payload as the typed object its accessors live on.</summary>
    /// <returns>An invalid EventData when the event carried none.</returns>
    [[nodiscard]] inline IL2CPP::VRChat::EventData ToEventData(const PhotonEvent& e) noexcept {
        return IL2CPP::VRChat::EventData(e.event_data);
    }

    /// <summary>World position of a player.</summary>
    /// <returns>The origin when the player or its transform is gone.</returns>
    [[nodiscard]] inline Vec3 Position(Player p) {
        auto t = ToVrc(p).GetTransform();
        if (!t) return {};
        const auto v = t.GetPosition();
        return Vec3{ v.x, v.y, v.z };
    }

    /// <summary>Distance in metres between two players.</summary>
    /// <returns>Zero when either player or its transform is gone.</returns>
    [[nodiscard]] inline float Distance(Player a, Player b) {
        const Vec3 pa = Position(a);
        const Vec3 pb = Position(b);
        const float dx = pa.x - pb.x, dy = pa.y - pb.y, dz = pa.z - pb.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    // players.rank takes the APIUser, and no row turns a Player into one; the walk is a
    // deob.offsets member read the typed layer already owns.
    inline PlayerRank Player::GetRank() const {
        IL2CPP::VRChat::Player p = ToVrc(*this);
        return p.GetPlayerRank();
    }

    inline Color Player::GetRankColor() const {
        return IL2CPP::VRChat::Player::GetRankColor(GetRank());
    }

    /// <summary>The typed layer, as the service Host::Il2Cpp() returns. It wraps the host's
    /// IL2CPP sibling table rather than an export group, so it has no exported rows; the free
    /// functions above are the same operations without the service hop.</summary>
    class Il2Cpp {
    public:
        /// <summary>The host's IL2CPP sibling table from unix_context, or null before bind.</summary>
        [[nodiscard]] const void* GetTable() const noexcept {
            return detail::g_ctx ? detail::g_ctx->il2cpp : nullptr;
        }
        /// <summary>The host's Unity sibling table from unix_context, or null before bind.</summary>
        [[nodiscard]] const void* GetUnityTable() const noexcept {
            return detail::g_ctx ? detail::g_ctx->unity : nullptr;
        }
        [[nodiscard]] IL2CPP::VRChat::Player ToVrc(Player p) const noexcept {
            return ::UNIx::ToVrc(p);
        }
        [[nodiscard]] Player FromVrc(IL2CPP::VRChat::Player p) const noexcept {
            return ::UNIx::FromVrc(p);
        }
        [[nodiscard]] IL2CPP::VRChat::EventData ToEventData(const PhotonEvent& e) const noexcept {
            return ::UNIx::ToEventData(e);
        }
        [[nodiscard]] Vec3 GetPosition(Player p) const { return ::UNIx::Position(p); }
        [[nodiscard]] float GetDistance(Player a, Player b) const { return ::UNIx::Distance(a, b); }
    };

} // namespace UNIx
