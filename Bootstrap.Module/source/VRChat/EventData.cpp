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
        return get_field<int>("Sender");
    }

    uint8_t EventData::GetCustomDataKey() {
        if (!valid()) return 0;
        return get_field<uint8_t>("CustomDataKey");
    }

    void* EventData::GetCustomData() {
        if (!valid()) return nullptr;
        return get_field<void*>("CustomData");
    }

} // namespace IL2CPP::VRChat
