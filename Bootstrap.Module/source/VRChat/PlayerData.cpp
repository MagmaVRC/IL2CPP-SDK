#include <VRChat/PlayerData.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>
#include <IL2CPP.Module/include/System/Array.hpp>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

    static constexpr const char* kPlayerDataClass = "VRC.SDK3.Persistence.PlayerData";

    template<typename T>
    static T PlayerData_Get(const char* method, VRCPlayerApi player, std::string_view key) {
        if (!player) return T{};
        auto m = MethodHandler::resolve(kPlayerDataClass, method, 2);
        if (!m) return T{};
        auto str = IL2CPP::Module::System::String::create(key);
        void* params[2] = { player.raw(), str.raw() };
        return MethodHandler::invoke<T>(m, nullptr, params);
    }

    template<typename T>
    static void PlayerData_Set(const char* method, std::string_view key, T value) {
        auto m = MethodHandler::resolve(kPlayerDataClass, method, 2);
        if (!m) return;
        auto str = IL2CPP::Module::System::String::create(key);
        void* params[2] = { str.raw(), &value };
        MethodHandler::invoke<void>(m, nullptr, params);
    }

    static std::vector<std::string> PlayerData_ArrayStrings(void* raw) {
        std::vector<std::string> out;
        IL2CPP::Module::System::Array<void*> arr{ raw };
        out.reserve(arr.size());
        for (void* s : arr.as_span())
            if (s) out.push_back(IL2CPP::Module::System::String(s).to_string());
        return out;
    }

    // GetKeys is declared IEnumerable<string>; the concrete collection is VRChat's choice.
    static std::vector<std::string> PlayerData_Strings(void* raw) {
        using IL2CPP::Module::ManagedObject;
        using IL2CPP::Module::Class;
        ManagedObject obj{ raw };
        Class klass = obj.get_class();
        if (!klass) return {};
        const std::string name = klass.full_name();
        if (name.size() >= 2 && name.compare(name.size() - 2, 2, "[]") == 0)
            return PlayerData_ArrayStrings(raw);

        auto count = klass.get_method("get_Count", 0);
        auto copy  = klass.get_method("CopyTo", 2);
        if (count && copy) {
            void* boxed = count.invoke(raw, nullptr);
            const int n = boxed ? *static_cast<int*>(IL2CPP::Unbox(boxed)) : 0;
            if (n <= 0) return {};
            auto arr = IL2CPP::Module::System::Array<void*>::Create("System.String", (uintptr_t)n);
            if (!arr) return {};
            int zero = 0;
            void* args[2] = { arr.raw(), &zero };
            copy.invoke(raw, args);
            return PlayerData_ArrayStrings(arr.raw());
        }

        auto getEnum = klass.get_method("GetEnumerator", 0);
        if (!getEnum) getEnum = klass.get_method("System.Collections.IEnumerable.GetEnumerator", 0);
        void* it = getEnum ? getEnum.invoke(raw, nullptr) : nullptr;
        if (!it) return {};
        ManagedObject iter{ it };
        Class itClass = iter.get_class();
        // A boxed struct enumerator would advance a copy, never the original.
        if (!itClass || itClass.is_subclass_of(Class::find("System.ValueType"))) return {};
        auto moveNext = itClass.get_method("MoveNext", 0);
        auto current  = itClass.get_method("get_Current", 0);
        if (!moveNext || !current) return {};
        std::vector<std::string> out;
        for (int guard = 0; guard < 100000; ++guard) {
            void* more = moveNext.invoke(it, nullptr);
            if (!more || !*static_cast<bool*>(IL2CPP::Unbox(more))) break;
            if (void* s = current.invoke(it, nullptr))
                out.push_back(IL2CPP::Module::System::String(s).to_string());
        }
        return out;
    }

    bool PlayerData::HasKey(VRCPlayerApi player, std::string_view key) {
        return PlayerData_Get<bool>("HasKey", player, key);
    }

    std::vector<std::string> PlayerData::GetKeys(VRCPlayerApi player) {
        if (!player) return {};
        auto m = MethodHandler::resolve(kPlayerDataClass, "GetKeys", 1);
        if (!m) return {};
        void* params[1] = { player.raw() };
        void* raw = MethodHandler::invoke<void*>(m, nullptr, params);
        if (!raw) return {};
        return PlayerData_Strings(raw);
    }

    std::string PlayerData::GetTypeName(VRCPlayerApi player, std::string_view key) {
        void* type = PlayerData_Get<void*>("GetType", player, key);
        if (!type) return {};
        auto klass = IL2CPP::Module::Class::from_system_type(type);
        return klass ? klass.full_name() : std::string{};
    }

    int PlayerData::GetInt(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<int>("GetInt", player, key); }
    float PlayerData::GetFloat(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<float>("GetFloat", player, key); }
    bool PlayerData::GetBool(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<bool>("GetBool", player, key); }
    int64_t PlayerData::GetLong(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<int64_t>("GetLong", player, key); }
    double PlayerData::GetDouble(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<double>("GetDouble", player, key); }
    IL2CPP::Vector3 PlayerData::GetVector3(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<IL2CPP::Vector3>("GetVector3", player, key); }
    IL2CPP::Quaternion PlayerData::GetQuaternion(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<IL2CPP::Quaternion>("GetQuaternion", player, key); }
    IL2CPP::Color PlayerData::GetColor(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<IL2CPP::Color>("GetColor", player, key); }

    std::string PlayerData::GetString(VRCPlayerApi player, std::string_view key) {
        void* s = PlayerData_Get<void*>("GetString", player, key);
        return s ? IL2CPP::Module::System::String(s).to_string() : std::string{};
    }

    void PlayerData::SetInt(std::string_view key, int value) { PlayerData_Set("SetInt", key, value); }
    void PlayerData::SetFloat(std::string_view key, float value) { PlayerData_Set("SetFloat", key, value); }
    void PlayerData::SetBool(std::string_view key, bool value) { PlayerData_Set("SetBool", key, value); }
    void PlayerData::SetLong(std::string_view key, int64_t value) { PlayerData_Set("SetLong", key, value); }
    void PlayerData::SetDouble(std::string_view key, double value) { PlayerData_Set("SetDouble", key, value); }
    void PlayerData::SetVector3(std::string_view key, IL2CPP::Vector3 value) { PlayerData_Set("SetVector3", key, value); }
    void PlayerData::SetQuaternion(std::string_view key, IL2CPP::Quaternion value) { PlayerData_Set("SetQuaternion", key, value); }
    void PlayerData::SetColor(std::string_view key, IL2CPP::Color value) { PlayerData_Set("SetColor", key, value); }

    uint8_t PlayerData::GetByte(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<uint8_t>("GetByte", player, key); }
    int8_t PlayerData::GetSByte(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<int8_t>("GetSByte", player, key); }
    int16_t PlayerData::GetShort(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<int16_t>("GetShort", player, key); }
    uint16_t PlayerData::GetUShort(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<uint16_t>("GetUShort", player, key); }
    uint32_t PlayerData::GetUInt(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<uint32_t>("GetUInt", player, key); }
    uint64_t PlayerData::GetULong(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<uint64_t>("GetULong", player, key); }
    IL2CPP::Vector2 PlayerData::GetVector2(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<IL2CPP::Vector2>("GetVector2", player, key); }
    IL2CPP::Vector4 PlayerData::GetVector4(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<IL2CPP::Vector4>("GetVector4", player, key); }
    IL2CPP::Color32 PlayerData::GetColor32(VRCPlayerApi player, std::string_view key) { return PlayerData_Get<IL2CPP::Color32>("GetColor32", player, key); }

    std::vector<uint8_t> PlayerData::GetBytes(VRCPlayerApi player, std::string_view key) {
        void* raw = PlayerData_Get<void*>("GetBytes", player, key);
        return raw ? IL2CPP::Module::System::Array<uint8_t>{ raw }.to_vector() : std::vector<uint8_t>{};
    }

    void PlayerData::SetByte(std::string_view key, uint8_t value) { PlayerData_Set("SetByte", key, value); }
    void PlayerData::SetSByte(std::string_view key, int8_t value) { PlayerData_Set("SetSByte", key, value); }
    void PlayerData::SetShort(std::string_view key, int16_t value) { PlayerData_Set("SetShort", key, value); }
    void PlayerData::SetUShort(std::string_view key, uint16_t value) { PlayerData_Set("SetUShort", key, value); }
    void PlayerData::SetUInt(std::string_view key, uint32_t value) { PlayerData_Set("SetUInt", key, value); }
    void PlayerData::SetULong(std::string_view key, uint64_t value) { PlayerData_Set("SetULong", key, value); }
    void PlayerData::SetVector2(std::string_view key, IL2CPP::Vector2 value) { PlayerData_Set("SetVector2", key, value); }
    void PlayerData::SetVector4(std::string_view key, IL2CPP::Vector4 value) { PlayerData_Set("SetVector4", key, value); }
    void PlayerData::SetColor32(std::string_view key, IL2CPP::Color32 value) { PlayerData_Set("SetColor32", key, value); }

    void PlayerData::SetBytes(std::string_view key, const std::vector<uint8_t>& value) {
        auto m = MethodHandler::resolve(kPlayerDataClass, "SetBytes", 2);
        if (!m) return;
        auto arr = IL2CPP::Module::System::Array<uint8_t>::Create("System.Byte", value.size());
        if (!arr) return;
        if (!value.empty()) std::memcpy(arr.data(), value.data(), value.size());
        auto k = IL2CPP::Module::System::String::create(key);
        void* params[2] = { k.raw(), arr.raw() };
        MethodHandler::invoke<void>(m, nullptr, params);
    }

    void PlayerData::SetString(std::string_view key, std::string_view value) {
        auto m = MethodHandler::resolve(kPlayerDataClass, "SetString", 2);
        if (!m) return;
        auto k = IL2CPP::Module::System::String::create(key);
        auto v = IL2CPP::Module::System::String::create(value);
        void* params[2] = { k.raw(), v.raw() };
        MethodHandler::invoke<void>(m, nullptr, params);
    }

} // namespace IL2CPP::VRChat
