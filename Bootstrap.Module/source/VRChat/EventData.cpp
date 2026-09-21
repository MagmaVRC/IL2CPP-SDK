#include <VRChat/EventData.hpp>

namespace IL2CPP::VRChat {

    uint8_t EventData::GetCode() {
        if (!valid()) return 0;
        return get_field<uint8_t>("Code");
    }

    void* EventData::GetParameters() {
        if (!valid()) return nullptr;
        return get_field<void*>("Parameters");
    }

    uint8_t EventData::GetSenderKey() {
        if (!valid()) return 0;
        return get_field<uint8_t>("SenderKey");
    }

    int EventData::GetSender() {
        if (!valid()) return 0;

        const int cached = get_field<int>("sender");
        if (cached != -1) return cached;
        return call_method<int>("get_Sender", nullptr, 0);
    }

    uint8_t EventData::GetCustomDataKey() {
        if (!valid()) return 0;
        return get_field<uint8_t>("CustomDataKey");
    }

    void* EventData::GetCustomData() {
        if (!valid()) return nullptr;
        if (void* value = get_property<void*>("CustomData")) return value;
        return get_field<void*>("customData");
    }

    void* EventData::GetParameter(uint8_t key) {
        if (!valid()) return nullptr;
        void* args[]{ &key };
        return call_method<void*>("get_Item", args, 1);
    }

    int EventData::GetParameterCount() {
        if (!valid()) return 0;

        void* parameters = GetParameters();
        if (!parameters) return 0;

        return IL2CPP::Module::ManagedObject{ parameters }.call_method<int>("get_Count", nullptr, 0);
    }

    void EventData::SetSender(int sender) {
        if (!valid()) return;
        void* args[]{ &sender };
        call_method<void>("set_Sender", args, 1);
    }

    void EventData::Reset() {
        if (!valid()) return;
        call_method<void>("Reset", nullptr, 0);
    }

} // namespace IL2CPP::VRChat
