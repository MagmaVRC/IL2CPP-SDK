#include <VRChat/APIUser.hpp>
#include <VRChat/ApiBadge.hpp>
#include <bootstrap_internal.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>
#include <IL2CPP.Module/include/System/List.hpp>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

    namespace {
        std::vector<std::string> collect_string_list(IL2CPP::Module::ManagedObject obj, const IL2CPP::Module::Method& method) {
            std::vector<std::string> result;
            if (!obj.valid()) return result;
            auto* listPtr = MethodHandler::invoke<void*>(method, obj.raw());
            if (!listPtr) return result;
            IL2CPP::Module::System::List<void*> list(listPtr);
            int cnt = list.count();
            for (int i = 0; i < cnt; ++i) {
                auto* elemPtr = list.try_at(static_cast<uintptr_t>(i));
                if (elemPtr && *elemPtr) {
                    result.push_back(IL2CPP::Module::System::String(*elemPtr).to_string());
                }
            }
            return result;
        }
    }

    std::string APIUser::GetDisplayName() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_displayName", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetUsername() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_username", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetPronouns() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_pronouns", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetBio() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_bio", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::vector<std::string> APIUser::GetBioLinks() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_bioLinks", 0);
        return collect_string_list(*this, m);
    }

    std::string APIUser::GetStatus() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_status", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetStatusDescription() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_statusDescription", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::vector<std::string> APIUser::GetTags() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_tags", 0);
        return collect_string_list(*this, m);
    }

    std::string APIUser::GetNote() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_note", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetUserLanguageCode() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_userLanguageCode", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetAvatarId() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_avatarId", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetFallbackId() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_fallbackId", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetCurrentAvatarImageUrl() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_currentAvatarImageUrl", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetCurrentAvatarThumbnailImageUrl() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_currentAvatarThumbnailImageUrl", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::vector<std::string> APIUser::GetCurrentAvatarTags() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_currentAvatarTags", 0);
        return collect_string_list(*this, m);
    }

    bool APIUser::GetAllowAvatarCopying() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_allowAvatarCopying", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    std::string APIUser::GetUserIcon() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_userIcon", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetProfilePicOverride() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_profilePicOverride", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetIconUrl() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_iconUrl", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetThumbnailUrl() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_thumbnailUrl", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    bool APIUser::GetIsFriend() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_isFriend", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    std::string APIUser::GetLocation() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_location", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::vector<ApiBadge> APIUser::GetBadges() {
        std::vector<ApiBadge> result;
        if (!valid()) return result;
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_badges", 0);
        auto* listPtr = MethodHandler::invoke<void*>(m, raw());
        if (!listPtr) return result;
        IL2CPP::Module::System::List<void*> list(listPtr);
        int cnt = list.count();
        for (int i = 0; i < cnt; ++i) {
            auto* elemPtr = list.try_at(static_cast<uintptr_t>(i));
            if (elemPtr && *elemPtr) {
                result.push_back(ApiBadge(*elemPtr));
            }
        }
        return result;
    }

    bool APIUser::GetAgeVerified() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_ageVerified", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool APIUser::GetIsAdult() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_isAdult", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool APIUser::GetIsOnMobile() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_IsOnMobile", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    std::string APIUser::GetLastPlatform() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_last_platform", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string APIUser::GetPlatform() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_platform", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    PlayerRank APIUser::GetPlayerRank() {
        if (!valid() || !Bootstrap::Module::is_connected()) return PlayerRank::Visitor;
        return static_cast<PlayerRank>(
            Bootstrap::Module::get_vtable()->get_player_rank(raw()));
    }
} // namespace IL2CPP::VRChat
