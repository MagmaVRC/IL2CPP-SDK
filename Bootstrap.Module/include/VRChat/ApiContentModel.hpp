#pragma once

#include "ApiModel.hpp"
#include <string>
#include <vector>

namespace IL2CPP::VRChat {

    /// Shared base of VRChat's uploadable content (avatars, worlds). Mirrors
    /// VRC.Core.ApiContentModel`1, which declares these properties; ApiAvatar and
    /// friends add only what is their own.
    class ApiContentModel : public ApiModel {
    public:
        using ApiModel::ApiModel;

        /// <summary>Content display name.</summary>
        [[nodiscard]] std::string GetName();
        /// <summary>Author-written description.</summary>
        [[nodiscard]] std::string GetDescription();
        /// <summary>User id of the author.</summary>
        [[nodiscard]] std::string GetAuthorId();
        /// <summary>Display name of the author.</summary>
        [[nodiscard]] std::string GetAuthorName();
        /// <summary>Full-size preview image url.</summary>
        [[nodiscard]] std::string GetImageUrl();
        /// <summary>Thumbnail preview image url.</summary>
        [[nodiscard]] std::string GetThumbnailImageUrl();
        /// <summary>Release status: public, private or hidden.</summary>
        [[nodiscard]] std::string GetReleaseStatus();
        /// <summary>Content version number; 0 when unresolved.</summary>
        [[nodiscard]] int GetVersion();
        /// <summary>Author-assigned tags.</summary>
        [[nodiscard]] std::vector<std::string> GetTags();
        /// <summary>Asset url of the package for the running platform.</summary>
        [[nodiscard]] std::string GetAssetUrl();
        /// <summary>Unity version the running platform's package was built with.</summary>
        [[nodiscard]] std::string GetUnityVersion();
    };

} // namespace IL2CPP::VRChat
