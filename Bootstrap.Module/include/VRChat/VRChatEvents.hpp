#pragma once

#include "Callbacks.hpp"
#include <cstdint>

namespace IL2CPP::VRChat {

    namespace detail {
        [[deprecated("UNIX0056: VRChatEvents::RegisterOnAwake is removed in 9.0; use UNIx::PlayerManager::OnJoining")]]
        inline void Deprecated_RegisterOnAwake() noexcept {}
        [[deprecated("UNIX0056: VRChatEvents::RegisterOnJoin is removed in 9.0; use UNIx::PlayerManager::OnJoined")]]
        inline void Deprecated_RegisterOnJoin() noexcept {}
        [[deprecated("UNIX0056: VRChatEvents::RegisterOnJoinComplete is removed in 9.0; use UNIx::PlayerManager::OnJoinComplete")]]
        inline void Deprecated_RegisterOnJoinComplete() noexcept {}
        [[deprecated("UNIX0056: VRChatEvents::RegisterOnLeave is removed in 9.0; use UNIx::PlayerManager::OnLeft")]]
        inline void Deprecated_RegisterOnLeave() noexcept {}
    }

    // The markers fire from template bodies in this level-1 region: a warning at the caller,
    // never an error under /sdl.
#pragma warning(push)
#pragma warning(1 : 4996)

    /// <summary>Deprecated 8.2 (UNIX0056), removed 9.0: use UNIx::PlayerManager.
    /// Player lifecycle notifications for consumers that are not UNIx::Module subclasses.</summary>
    class VRChatEvents {
    public:
        static VRChatEvents& Get();

        /// <returns>A subscription handle for Unregister, or 0 when the host refused.</returns>
        template <class = void> uint32_t RegisterOnAwake(PlayerCallback callback) {
            detail::Deprecated_RegisterOnAwake();
            return Subscribe(0u, callback);
        }
        template <class = void> uint32_t RegisterOnJoin(PlayerCallback callback) {
            detail::Deprecated_RegisterOnJoin();
            return Subscribe(1u, callback);
        }
        template <class = void> uint32_t RegisterOnJoinComplete(PlayerCallback callback) {
            detail::Deprecated_RegisterOnJoinComplete();
            return Subscribe(2u, callback);
        }
        template <class = void> uint32_t RegisterOnLeave(PlayerCallback callback) {
            detail::Deprecated_RegisterOnLeave();
            return Subscribe(3u, callback);
        }

        /// <param name="handle">A handle from one of the Register calls.</param>
        void Unregister(uint32_t handle);

    private:
        VRChatEvents() = default;
        /// <param name="phase">unix_player_phase: awake 0, joined 1, joined_complete 2, left 3.</param>
        static uint32_t Subscribe(uint32_t phase, PlayerCallback callback);
    };

#pragma warning(pop)

} // namespace IL2CPP::VRChat
