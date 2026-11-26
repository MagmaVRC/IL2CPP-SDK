#include <VRChat/VRChatEvents.hpp>
#include <VRChat/HostBridge.hpp>

namespace IL2CPP::VRChat {

    namespace {
        void UNIX_CC forward_player_event(void* ud, const unix_player_ev* ev) {
            if (!ud || !ev) return;
            reinterpret_cast<PlayerCallback>(ud)(Player(ev->player));
        }

    }

    VRChatEvents& VRChatEvents::Get() {
        static VRChatEvents instance;
        return instance;
    }

    uint32_t VRChatEvents::Subscribe(uint32_t phase, PlayerCallback callback) {
        if (!callback || phase > unix_player_left) return 0;
        return Bridge::OnPlayerEvent(1u << phase, &forward_player_event,
                                     reinterpret_cast<void*>(callback));
    }

    void VRChatEvents::Unregister(uint32_t handle) {
        if (handle != unix_null) Bridge::Cancel(handle);
    }

} // namespace IL2CPP::VRChat
