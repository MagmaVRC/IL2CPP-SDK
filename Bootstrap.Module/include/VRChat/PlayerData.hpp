#pragma once

#include "VRCPlayerApi.hpp"
#include <IL2CPP.Module/include/il2cpp_module.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace IL2CPP::VRChat {

    class PlayerData {
    public:
        [[nodiscard]] static bool HasKey(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static std::vector<std::string> GetKeys(VRCPlayerApi player);
        [[nodiscard]] static std::string GetTypeName(VRCPlayerApi player, std::string_view key);

        [[nodiscard]] static int GetInt(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static float GetFloat(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static std::string GetString(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static bool GetBool(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static int64_t GetLong(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static double GetDouble(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static IL2CPP::Vector3 GetVector3(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static IL2CPP::Quaternion GetQuaternion(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static IL2CPP::Color GetColor(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static uint8_t GetByte(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static int8_t GetSByte(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static int16_t GetShort(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static uint16_t GetUShort(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static uint32_t GetUInt(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static uint64_t GetULong(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static IL2CPP::Vector2 GetVector2(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static IL2CPP::Vector4 GetVector4(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static IL2CPP::Color32 GetColor32(VRCPlayerApi player, std::string_view key);
        [[nodiscard]] static std::vector<uint8_t> GetBytes(VRCPlayerApi player, std::string_view key);

        static void SetInt(std::string_view key, int value);
        static void SetFloat(std::string_view key, float value);
        static void SetString(std::string_view key, std::string_view value);
        static void SetBool(std::string_view key, bool value);
        static void SetLong(std::string_view key, int64_t value);
        static void SetDouble(std::string_view key, double value);
        static void SetVector3(std::string_view key, IL2CPP::Vector3 value);
        static void SetQuaternion(std::string_view key, IL2CPP::Quaternion value);
        static void SetColor(std::string_view key, IL2CPP::Color value);
        static void SetByte(std::string_view key, uint8_t value);
        static void SetSByte(std::string_view key, int8_t value);
        static void SetShort(std::string_view key, int16_t value);
        static void SetUShort(std::string_view key, uint16_t value);
        static void SetUInt(std::string_view key, uint32_t value);
        static void SetULong(std::string_view key, uint64_t value);
        static void SetVector2(std::string_view key, IL2CPP::Vector2 value);
        static void SetVector4(std::string_view key, IL2CPP::Vector4 value);
        static void SetColor32(std::string_view key, IL2CPP::Color32 value);
        static void SetBytes(std::string_view key, const std::vector<uint8_t>& value);
    };

} // namespace IL2CPP::VRChat
