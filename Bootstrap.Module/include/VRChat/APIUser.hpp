#pragma once

#include "ApiModel.hpp"
#include <unix/unix_types.hpp>
#include <string>
#include <vector>

namespace IL2CPP::VRChat {

    class ApiBadge;
    using PlayerRank = UNIx::PlayerRank;

    class APIUser : public ApiModel {
    public:
        using ApiModel::ApiModel;

        /// <summary>The logged-in user, read off APIUser's own static. This is where the
        /// local user lives: Player::GetAPIUser walks per-player offsets, and the local
        /// player's slot is empty on builds that never populate it.</summary>
        /// <returns>Null before login has completed.</returns>
        [[nodiscard]] static APIUser GetCurrentUser();

        /// <summary>The instance the user is actually in. `location` also carries states
        /// rather than places -- "traveling", "private", "offline" -- and while travelling
        /// the instance being joined is in `travelingToLocation` instead.</summary>
        /// <returns>An instance id, or empty when neither field names one.</returns>
        [[nodiscard]] std::string GetInstanceLocation();

        [[nodiscard]] std::string GetDisplayName();
        [[nodiscard]] std::string GetUsername();
        [[nodiscard]] std::string GetPronouns();
        [[nodiscard]] std::string GetBio();
        [[nodiscard]] std::vector<std::string> GetBioLinks();
        [[nodiscard]] std::string GetStatus();
        [[nodiscard]] std::string GetStatusDescription();
        [[nodiscard]] std::vector<std::string> GetTags();
        [[nodiscard]] std::string GetNote();
        [[nodiscard]] std::string GetUserLanguageCode();

        [[nodiscard]] std::string GetAvatarId();
        [[nodiscard]] std::string GetFallbackId();
        [[nodiscard]] std::string GetCurrentAvatarImageUrl();
        [[nodiscard]] std::string GetCurrentAvatarThumbnailImageUrl();
        [[nodiscard]] std::vector<std::string> GetCurrentAvatarTags();
        [[nodiscard]] bool GetAllowAvatarCopying();

        [[nodiscard]] std::string GetUserIcon();
        [[nodiscard]] std::string GetProfilePicOverride();
        [[nodiscard]] std::string GetIconUrl();
        [[nodiscard]] std::string GetThumbnailUrl();

        [[nodiscard]] bool GetIsFriend();
        [[nodiscard]] std::string GetLocation();
        [[nodiscard]] std::vector<ApiBadge> GetBadges();

        [[nodiscard]] bool GetAgeVerified();
        [[nodiscard]] bool GetIsAdult();
        [[nodiscard]] bool GetIsOnMobile();
        [[nodiscard]] std::string GetLastPlatform();
        [[nodiscard]] std::string GetPlatform();
        [[nodiscard]] PlayerRank GetPlayerRank();

        [[nodiscard]] std::string GetDateJoined();
        [[nodiscard]] std::string GetDeveloperType();
    };

} // namespace IL2CPP::VRChat
