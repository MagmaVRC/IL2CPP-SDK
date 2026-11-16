#pragma once

#include "ApiContentModel.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace IL2CPP::VRChat {

    /// VRC.Core.ApiAvatar. Name, author, images, tags and version are declared by
    /// ApiContentModel`1 and inherited; only avatar-specific members belong here.
    class ApiAvatar : public ApiContentModel {
    public:
        using ApiContentModel::ApiContentModel;

        /// <summary>Every platform the avatar was uploaded for, with that platform's
        /// performance rating, from supportedPlatforms and GetPerformanceRatingForPlatform.</summary>
        /// <returns>(SupportedPlatforms enum name, rating) pairs; the rating is empty when
        /// VRChat holds none. Empty when the model is unpopulated.</returns>
        [[nodiscard]] std::vector<std::pair<std::string, std::string>> GetPlatformRatings();

        /// <summary>VRChat's created_at as unix seconds; 0 when unset.</summary>
        [[nodiscard]] int64_t GetCreatedAtUnix();

        /// <summary>VRChat's updated_at as unix seconds; 0 when unset.</summary>
        [[nodiscard]] int64_t GetUpdatedAtUnix();
    };

} // namespace IL2CPP::VRChat
