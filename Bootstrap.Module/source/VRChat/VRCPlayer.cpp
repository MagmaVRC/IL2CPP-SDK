#include <VRChat/VRCPlayer.hpp>
#include <VRChat/VRCPlayerApi.hpp>
#include <VRChat/Player.hpp>
#include <VRChat/PlayerNameplate.hpp>
#include <bootstrap_internal.hpp>

namespace IL2CPP::VRChat {

    VRCPlayer VRCPlayer::GetLocalPlayer() {
        if (!Bootstrap::Module::is_connected()) return VRCPlayer();
        return VRCPlayer(Bootstrap::Module::get_vtable()->get_local_vrc_player());
    }

    VRCPlayerApi VRCPlayer::GetVRCPlayerApi() {
        if (!valid() || !Bootstrap::Module::is_connected()) return VRCPlayerApi();
        auto* data = Bootstrap::Module::get_vtable()->get_vrc_player_data();
        return VRCPlayerApi(*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(raw()) + data->VRCPlayerApi));
    }

    Player VRCPlayer::GetPlayer() {
        if (!valid() || !Bootstrap::Module::is_connected()) return Player();
        auto* data = Bootstrap::Module::get_vtable()->get_vrc_player_data();
        return Player(*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(raw()) + data->Player));
    }

    PlayerNameplate VRCPlayer::GetNameplate() {
        if (!valid() || !Bootstrap::Module::is_connected()) return {};
        auto* data = Bootstrap::Module::get_vtable()->get_vrc_player_data();
        return PlayerNameplate(*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(raw()) + data->PlayerNameplate));
    }

} // namespace IL2CPP::VRChat
