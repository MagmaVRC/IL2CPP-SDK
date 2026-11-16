#include <VRChat/VRC_Pickup.hpp>
#include <IL2CPP.Module/include/Reflection.hpp>
#include <atomic>
#include <type_traits>

namespace IL2CPP::VRChat {

    namespace {
        struct PickupMembers {
            void* klass = nullptr;
            IL2CPP::Module::Method isHeld;
            IL2CPP::Module::Method currentPlayer;
            IL2CPP::Module::Method currentHand;
            IL2CPP::Module::Field pickupable;
            IL2CPP::Module::Field disallowTheft;
            IL2CPP::Module::Field autoHold;
            IL2CPP::Module::Field orientation;
            IL2CPP::Module::Field pickupableSet;
            IL2CPP::Module::Field disallowTheftSet;
            IL2CPP::Module::Field autoHoldSet;
            IL2CPP::Module::Field orientationSet;
        };

        constexpr int kSlots = 8;
        std::atomic<const PickupMembers*> g_slots[kSlots]{};

        const PickupMembers* MembersOf(const IL2CPP::Module::ManagedObject& obj) {
            const IL2CPP::Module::Class klass = obj.get_class();
            if (!klass) return nullptr;
            void* raw = klass.raw();

            for (auto& slot : g_slots) {
                const PickupMembers* m = slot.load(std::memory_order_acquire);
                if (!m) break;
                if (m->klass == raw) return m;
            }

            auto* fresh = new PickupMembers();
            fresh->klass = raw;
            fresh->isHeld = klass.get_method("get_IsHeld", 0);
            fresh->currentPlayer = klass.get_method("get_currentPlayer", 0);
            fresh->currentHand = klass.get_method("get_currentHand", 0);
            fresh->pickupable = klass.get_field("pickupable");
            fresh->disallowTheft = klass.get_field("DisallowTheft");
            fresh->autoHold = klass.get_field("AutoHold");
            fresh->orientation = klass.get_field("orientation");
            fresh->pickupableSet = klass.get_field_deep("pickupable");
            fresh->disallowTheftSet = klass.get_field_deep("DisallowTheft");
            fresh->autoHoldSet = klass.get_field_deep("AutoHold");
            fresh->orientationSet = klass.get_field_deep("orientation");

            for (auto& slot : g_slots) {
                const PickupMembers* expected = nullptr;
                if (slot.compare_exchange_strong(expected, fresh, std::memory_order_acq_rel))
                    return fresh;
                if (expected->klass == raw) {
                    delete fresh;
                    return expected;
                }
            }
            delete fresh;
            return nullptr;
        }

        template<typename T>
        T InvokeAs(const IL2CPP::Module::Method& method, void* self) {
            void* result = method.invoke(self, nullptr);
            if (!result) return T{};
            if constexpr (std::is_pointer_v<T>) return reinterpret_cast<T>(result);
            else return *reinterpret_cast<T*>(IL2CPP::Unbox(result));
        }
    }

    bool VRC_Pickup::IsHeld() {
        if (!valid()) return false;
        if (const auto* m = MembersOf(*this); m && m->isHeld) return InvokeAs<bool>(m->isHeld, raw());
        return call_method<bool>("get_IsHeld", nullptr, 0);
    }

    VRCPlayerApi VRC_Pickup::GetCurrentPlayer() {
        if (!valid()) return VRCPlayerApi(static_cast<void*>(nullptr));
        if (const auto* m = MembersOf(*this); m && m->currentPlayer)
            return VRCPlayerApi(InvokeAs<void*>(m->currentPlayer, raw()));
        return VRCPlayerApi(call_method<void*>("get_currentPlayer", nullptr, 0));
    }

    PickupHand VRC_Pickup::GetCurrentHand() {
        if (!valid()) return static_cast<PickupHand>(0);
        if (const auto* m = MembersOf(*this); m && m->currentHand)
            return static_cast<PickupHand>(InvokeAs<int>(m->currentHand, raw()));
        return static_cast<PickupHand>(call_method<int>("get_currentHand", nullptr, 0));
    }

    bool VRC_Pickup::GetPickupable() {
        if (const auto* m = MembersOf(*this); m && m->pickupable) return get_field<bool>(m->pickupable);
        return get_field<bool>("pickupable");
    }

    void VRC_Pickup::SetPickupable(bool value) {
        if (const auto* m = MembersOf(*this); m && m->pickupableSet) return set_field(m->pickupableSet, value);
        set_field("pickupable", value);
    }

    std::string VRC_Pickup::GetUseText() {
        return get_string_field("UseText");
    }

    void VRC_Pickup::SetUseText(std::string_view text) {
        set_string_field("UseText", text);
    }

    bool VRC_Pickup::GetDisallowTheft() {
        if (const auto* m = MembersOf(*this); m && m->disallowTheft) return get_field<bool>(m->disallowTheft);
        return get_field<bool>("DisallowTheft");
    }

    void VRC_Pickup::SetDisallowTheft(bool value) {
        if (const auto* m = MembersOf(*this); m && m->disallowTheftSet) return set_field(m->disallowTheftSet, value);
        set_field("DisallowTheft", value);
    }

    AutoHoldMode VRC_Pickup::GetAutoHold() {
        if (const auto* m = MembersOf(*this); m && m->autoHold) return static_cast<AutoHoldMode>(get_field<int>(m->autoHold));
        return static_cast<AutoHoldMode>(get_field<int>("AutoHold"));
    }

    void VRC_Pickup::SetAutoHold(AutoHoldMode mode) {
        if (const auto* m = MembersOf(*this); m && m->autoHoldSet) return set_field(m->autoHoldSet, static_cast<int>(mode));
        set_field("AutoHold", static_cast<int>(mode));
    }

    PickupOrientation VRC_Pickup::GetOrientation() {
        if (const auto* m = MembersOf(*this); m && m->orientation)
            return static_cast<PickupOrientation>(get_field<int>(m->orientation));
        return static_cast<PickupOrientation>(get_field<int>("orientation"));
    }

    void VRC_Pickup::SetOrientation(PickupOrientation orientation) {
        if (const auto* m = MembersOf(*this); m && m->orientationSet)
            return set_field(m->orientationSet, static_cast<int>(orientation));
        set_field("orientation", static_cast<int>(orientation));
    }

    void VRC_Pickup::Drop() {
        call_method<void>("Drop", nullptr, 0);
    }

    void VRC_Pickup::GenerateHapticEvent(float duration, float amplitude, float frequency) {
        void* params[3] = { &duration, &amplitude, &frequency };
        call_method<void>("GenerateHapticEvent", params, 3);
    }

} // namespace IL2CPP::VRChat
