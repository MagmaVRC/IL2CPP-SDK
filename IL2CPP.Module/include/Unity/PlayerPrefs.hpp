#pragma once
#include "../MethodHandler.hpp"
#include "../System/String.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::Unity {

    class PlayerPrefs {
    public:
        PlayerPrefs() = delete;

        static void SetInt(std::string_view key, int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("SetInt"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(key).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, nullptr, params);
        }
        [[nodiscard]] static int GetInt(std::string_view key, int defaultValue = 0) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("GetInt"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return defaultValue;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(key).c_str());
            void* params[] = { str, &defaultValue };
            return MethodHandler::invoke<int>(m, nullptr, params);
        }

        static void SetFloat(std::string_view key, float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("SetFloat"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(key).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, nullptr, params);
        }
        [[nodiscard]] static float GetFloat(std::string_view key, float defaultValue = 0.f) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("GetFloat"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return defaultValue;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(key).c_str());
            void* params[] = { str, &defaultValue };
            return MethodHandler::invoke<float>(m, nullptr, params);
        }

        static void SetString(std::string_view key, std::string_view value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("SetString"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* keyStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(key).c_str());
            void* valStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(value).c_str());
            void* params[] = { keyStr, valStr };
            MethodHandler::invoke(m, nullptr, params);
        }
        [[nodiscard]] static std::string GetString(std::string_view key, std::string_view defaultValue = "") {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("GetString"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return std::string(defaultValue);
            void* keyStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(key).c_str());
            void* defStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(defaultValue).c_str());
            void* params[] = { keyStr, defStr };
            void* result = MethodHandler::invoke<void*>(m, nullptr, params);
            if (!result) return std::string(defaultValue);
            return System::String{ result }.to_string();
        }

        [[nodiscard]] static bool HasKey(std::string_view key) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("HasKey"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return false;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(key).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        static void DeleteKey(std::string_view key) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("DeleteKey"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(key).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void DeleteAll() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("DeleteAll"), 0);
            MethodHandler::invoke(m, nullptr);
        }

        static void Save() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.PlayerPrefs"), IL2CPP_STR("Save"), 0);
            MethodHandler::invoke(m, nullptr);
        }
    };

} // namespace IL2CPP::Module::Unity
