#include <VRChat/EventData.hpp>
#include <atomic>

namespace IL2CPP::VRChat {

    namespace {
        // Photon hands every inbound packet to us as the same EventData class, so
        // the by-name metadata lookups in the accessors below repeat per packet per
        // subscriber. Offsets are resolved once and keyed on the class pointer.
        struct EventDataLayout {
            std::atomic<void*> klass{ nullptr };
            int code = -1;
            int parameters = -1;
            int senderKey = -1;
            int sender = -1;
            int customDataKey = -1;
            int customData = -1;
            IL2CPP::Module::Method customDataGetter{};
            IL2CPP::Module::Method senderGetter{};
        };

        EventDataLayout g_layout;

        int FieldOffset(const IL2CPP::Module::Class& klass, std::string_view name) {
            IL2CPP::Module::Field f = klass.get_field(name);
            return f ? f.offset() : -1;
        }

        // Returns null until every offset resolves; a failed resolve is never cached.
        const EventDataLayout* Layout(const IL2CPP::Module::ManagedObject& obj) {
            IL2CPP::Module::Class klass = obj.get_class();
            if (!klass) return nullptr;

            void* raw = klass.raw();
            if (g_layout.klass.load(std::memory_order_acquire) == raw) return &g_layout;

            EventDataLayout probe;
            probe.code = FieldOffset(klass, "Code");
            probe.parameters = FieldOffset(klass, "Parameters");
            probe.senderKey = FieldOffset(klass, "SenderKey");
            probe.sender = FieldOffset(klass, "sender");
            probe.customDataKey = FieldOffset(klass, "CustomDataKey");
            probe.customData = FieldOffset(klass, "customData");

            if (probe.code < 0 || probe.parameters < 0 || probe.senderKey < 0 ||
                probe.sender < 0 || probe.customDataKey < 0 || probe.customData < 0)
                return nullptr;

            if (IL2CPP::Module::Property prop = klass.get_property("CustomData"))
                probe.customDataGetter = prop.getter();
            probe.senderGetter = klass.get_method("get_Sender", 0);

            g_layout.code = probe.code;
            g_layout.parameters = probe.parameters;
            g_layout.senderKey = probe.senderKey;
            g_layout.sender = probe.sender;
            g_layout.customDataKey = probe.customDataKey;
            g_layout.customData = probe.customData;
            g_layout.customDataGetter = probe.customDataGetter;
            g_layout.senderGetter = probe.senderGetter;
            g_layout.klass.store(raw, std::memory_order_release);
            return &g_layout;
        }
    }

    uint8_t EventData::GetCode() {
        if (!valid()) return 0;
        if (const EventDataLayout* l = Layout(*this)) return get_field<uint8_t>(l->code);
        return get_field<uint8_t>("Code");
    }

    void* EventData::GetParameters() {
        if (!valid()) return nullptr;
        if (const EventDataLayout* l = Layout(*this)) return get_field<void*>(l->parameters);
        return get_field<void*>("Parameters");
    }

    uint8_t EventData::GetSenderKey() {
        if (!valid()) return 0;
        if (const EventDataLayout* l = Layout(*this)) return get_field<uint8_t>(l->senderKey);
        return get_field<uint8_t>("SenderKey");
    }

    int EventData::GetSender() {
        if (!valid()) return 0;

        const EventDataLayout* l = Layout(*this);
        const int cached = l ? get_field<int>(l->sender) : get_field<int>("sender");
        if (cached != -1) return cached;
        if (l && l->senderGetter) {
            void* result = l->senderGetter.invoke(raw(), nullptr);
            return result ? *reinterpret_cast<int*>(IL2CPP::Unbox(result)) : 0;
        }
        return call_method<int>("get_Sender", nullptr, 0);
    }

    uint8_t EventData::GetCustomDataKey() {
        if (!valid()) return 0;
        if (const EventDataLayout* l = Layout(*this)) return get_field<uint8_t>(l->customDataKey);
        return get_field<uint8_t>("CustomDataKey");
    }

    void* EventData::GetCustomData() {
        if (!valid()) return nullptr;

        // get_CustomData returns the backing field once set and only fills it when null.
        const EventDataLayout* l = Layout(*this);
        if (!l) {
            if (void* value = get_field<void*>("customData")) return value;
            if (void* value = get_property<void*>("CustomData")) return value;
            return get_field<void*>("customData");
        }

        if (void* value = get_field<void*>(l->customData)) return value;
        if (l->customDataGetter) {
            if (void* value = l->customDataGetter.invoke(raw(), nullptr)) return value;
        }
        return get_field<void*>(l->customData);
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
