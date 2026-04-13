#pragma once

#include "VRCPlayerApi.hpp"
#include "Player.hpp"

namespace IL2CPP::VRChat {

    using PlayerCallback = void(__cdecl*)(Player player);

    using ExplorerNavigateCallback = void(__cdecl*)(
        IL2CPP::Module::Unity::GameObject gameObject,
        IL2CPP::Module::ManagedObject component);

} // namespace IL2CPP::VRChat
