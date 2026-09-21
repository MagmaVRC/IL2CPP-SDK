#include <VRChat/APIUser.hpp>
#include <VRChat/ApiBadge.hpp>
#include <VRChat/HostBridge.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>
#include <IL2CPP.Module/include/System/List.hpp>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

    namespace {
        std::string invoke_string_getter(const IL2CPP::Module::Method& method, void* instance) {
            if (!instance) return "";
            void* str = MethodHandler::invoke<void*>(method, instance);
            return str ? IL2CPP::Module::System::String(str).to_string() : "";
        }

        std::vector<std::string> collect_string_list(IL2CPP::Module::ManagedObject obj, const IL2CPP::Module::Method& method) {
            std::vector<std::string> result;
            if (!obj) return result;
            auto* listPtr = MethodHandler::invoke<void*>(method, obj.raw());
            if (!listPtr) return result;
            IL2CPP::Module::System::List<void*> list(listPtr);
            int cnt = list.count();
            if (cnt > 0) result.reserve(static_cast<size_t>(cnt));
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
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetUsername() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_username", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetPronouns() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_pronouns", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetBio() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_bio", 0);
        return invoke_string_getter(m, raw());
    }

    std::vector<std::string> APIUser::GetBioLinks() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_bioLinks", 0);
        return collect_string_list(*this, m);
    }

    std::string APIUser::GetStatus() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_status", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetStatusDescription() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_statusDescription", 0);
        return invoke_string_getter(m, raw());
    }

    std::vector<std::string> APIUser::GetTags() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_tags", 0);
        return collect_string_list(*this, m);
    }

    std::string APIUser::GetNote() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_note", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetUserLanguageCode() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_userLanguageCode", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetAvatarId() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_avatarId", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetFallbackId() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_fallbackId", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetCurrentAvatarImageUrl() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_currentAvatarImageUrl", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetCurrentAvatarThumbnailImageUrl() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_currentAvatarThumbnailImageUrl", 0);
        return invoke_string_getter(m, raw());
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
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetProfilePicOverride() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_profilePicOverride", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetIconUrl() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_iconUrl", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetThumbnailUrl() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_thumbnailUrl", 0);
        return invoke_string_getter(m, raw());
    }

    bool APIUser::GetIsFriend() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_isFriend", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    std::string APIUser::GetLocation() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_location", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetInstanceLocation() {
        const auto names_instance = [](const std::string& text) {
            return text.rfind("wrld_", 0) == 0 || text.rfind("local:", 0) == 0;
        };

        std::string here = GetLocation();
        if (names_instance(here)) return here;

        // Not a place but a state -- "traveling", "private", "offline". While travelling,
        // and on builds that leave it at "traveling" after arriving, the instance actually
        // joined is the one named here.
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_travelingToLocation", 0);
        std::string going = invoke_string_getter(m, raw());
        return names_instance(going) ? going : std::string{};
    }

    APIUser APIUser::GetCurrentUser() {
        // The property's backing field rather than get_CurrentUser: a static getter has to
        // be invoked on a class whose cctor may not have run, and the field read cannot
        // throw into the caller the way that invoke can.
        // Resolved lazily and cached only once it succeeds: the first caller runs before
        // the class is loaded, and a static initialised from that attempt would cache the
        // failure for the rest of the session.
        static IL2CPP::Module::Class klass{};
        static IL2CPP::Module::Field field{};
        if (!klass) {
            klass = IL2CPP::Module::Class::find("VRC.Core.APIUser");
            if (!klass) return {};
        }
        if (!field) {
            field = klass.get_field("<CurrentUser>k__BackingField");
            if (!field) return {};
        }

        const int offset = field.offset();
        if (offset < 0) return {};
        void* statics = klass.static_field_data();
        if (!statics) return {};   // absent until the class has been initialised

        return APIUser(*reinterpret_cast<void**>(static_cast<char*>(statics) + offset));
    }

    std::vector<ApiBadge> APIUser::GetBadges() {
        std::vector<ApiBadge> result;
        if (!valid()) return result;
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_badges", 0);
        auto* listPtr = MethodHandler::invoke<void*>(m, raw());
        if (!listPtr) return result;
        IL2CPP::Module::System::List<void*> list(listPtr);
        int cnt = list.count();
        if (cnt > 0) result.reserve(static_cast<size_t>(cnt));
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
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetPlatform() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_platform", 0);
        return invoke_string_getter(m, raw());
    }

    PlayerRank APIUser::GetPlayerRank() {
        if (!valid()) return PlayerRank::Visitor;
        return static_cast<PlayerRank>(Bridge::Rank(raw()));
    }

    std::string APIUser::GetDateJoined() {
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_date_joined", 0);
        return invoke_string_getter(m, raw());
    }

    std::string APIUser::GetDeveloperType() {
        if (!valid()) return "";
        // developerType is the DeveloperType enum, not a string; reading the
        // returned int as a String* handed callers whatever bytes sat at 0..3.
        static auto m = MethodHandler::resolve("VRC.Core.APIUser", "get_developerType", 0);
        switch (MethodHandler::invoke<int>(m, raw())) {
            case 0:  return "none";
            case 1:  return "trusted";
            case 2:  return "internal";
            case 3:  return "moderator";
            default: return "";
        }
    }
} // namespace IL2CPP::VRChat
