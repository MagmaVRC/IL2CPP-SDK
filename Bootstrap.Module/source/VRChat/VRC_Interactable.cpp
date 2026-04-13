#include <VRChat/VRC_Interactable.hpp>

namespace IL2CPP::VRChat {

    std::string VRC_Interactable::GetInteractText() {
        return get_string_field("interactText");
    }

    void VRC_Interactable::SetInteractText(std::string_view text) {
        set_string_field("interactText", text);
    }

    float VRC_Interactable::GetProximity() {
        if (!valid()) return 0.0f;
        return get_field<float>("proximity");
    }

    void VRC_Interactable::SetProximity(float proximity) {
        if (!valid()) return;
        set_field("proximity", proximity);
    }

} // namespace IL2CPP::VRChat
