#include <VRChat/VRChatEvents.hpp>
#include <bootstrap_internal.hpp>

namespace IL2CPP::VRChat {

    VRChatEvents& VRChatEvents::Get() {
        static VRChatEvents instance;
        return instance;
    }

    bool VRChatEvents::valid() const noexcept {
        return Bootstrap::Module::is_connected();
    }

    uint32_t VRChatEvents::register_on_awake(uint32_t module_id, PlayerCallback callback) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return Bootstrap::Module::get_vtable()->register_player_event(
            module_id, Bootstrap::PlayerEvent::Awake,
            reinterpret_cast<Bootstrap::fn_player_simple_callback>(callback));
    }

    uint32_t VRChatEvents::register_on_join(uint32_t module_id, PlayerCallback callback) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return Bootstrap::Module::get_vtable()->register_player_event(
            module_id, Bootstrap::PlayerEvent::Join,
            reinterpret_cast<Bootstrap::fn_player_simple_callback>(callback));
    }

    uint32_t VRChatEvents::register_on_join_complete(uint32_t module_id, PlayerCallback callback) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return Bootstrap::Module::get_vtable()->register_player_event(
            module_id, Bootstrap::PlayerEvent::JoinComplete,
            reinterpret_cast<Bootstrap::fn_player_simple_callback>(callback));
    }

    uint32_t VRChatEvents::register_on_leave(uint32_t module_id, PlayerCallback callback) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return Bootstrap::Module::get_vtable()->register_player_event(
            module_id, Bootstrap::PlayerEvent::Leave,
            reinterpret_cast<Bootstrap::fn_player_simple_callback>(callback));
    }

    void VRChatEvents::unregister_callback(uint32_t module_id, uint32_t callback_id) {
        if (!valid()) return;
        Bootstrap::Module::get_vtable()->unregister_player_event(module_id, callback_id);
    }

} // namespace IL2CPP::VRChat
