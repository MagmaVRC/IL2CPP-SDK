#include <VRChat/PlayerNameplate.hpp>
#include <bootstrap_internal.hpp>

namespace IL2CPP::VRChat {

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetContents() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_contents));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetMainContainer() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_mainContainer));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetTextContainer() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_textContainer));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetSubText() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_subText));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetPronouns() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_pronouns));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetIcon() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_icon));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetPlatform() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_platform));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetFriendIcon() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_friendIcon));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetDevBanner() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_devBanner));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetDevIcon() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_devIcon));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetQuickStats() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_quickStats));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetInteractionStatus() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_interactionStatus));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetPlayerStatusIcons() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_playerStatusIcons));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetUserMuted() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_userMuted));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetUserVolume() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_userVolume));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetListenBlocked() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_listenBlocked));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetEarmuffsIcon() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_earmuffsIcon));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetFocusViewIcon() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_focusViewIcon));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetGroupInfo() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_groupInfo));
    }

    IL2CPP::Module::Unity::GameObject PlayerNameplate::GetAvatarProgress() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_nameplate_data();
        if (!data) return {};
        return IL2CPP::Module::Unity::GameObject(*reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(m_native) + data->gameObject_avatarProgress));
    }

} // namespace IL2CPP::VRChat
