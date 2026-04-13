#pragma once

#include <IL2CPP.Module/include/il2cpp_module.hpp>

namespace IL2CPP::VRChat {

    class PlayerNameplate : public IL2CPP::Module::Unity::MonoBehaviour {
    public:
        using MonoBehaviour::MonoBehaviour;

        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetContents();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetMainContainer();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetTextContainer();

        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetSubText();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetPronouns();

        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetIcon();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetPlatform();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetFriendIcon();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetDevBanner();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetDevIcon();

        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetQuickStats();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetInteractionStatus();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetPlayerStatusIcons();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetUserMuted();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetUserVolume();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetListenBlocked();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetEarmuffsIcon();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetFocusViewIcon();

        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetGroupInfo();
        [[nodiscard]] IL2CPP::Module::Unity::GameObject GetAvatarProgress();
    };

} // namespace IL2CPP::VRChat
