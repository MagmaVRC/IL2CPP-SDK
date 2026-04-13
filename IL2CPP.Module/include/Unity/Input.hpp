#pragma once
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::Unity {

    class Input {
    public:
        Input() = delete;

        [[nodiscard]] static float GetAxis(std::string_view axisName) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetAxis", 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return 0.f;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(axisName).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<float>(m, nullptr, params);
        }

        [[nodiscard]] static float GetAxisRaw(std::string_view axisName) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetAxisRaw", 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return 0.f;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(axisName).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<float>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetKey(KeyCode key) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetKey", 1);
            int k = static_cast<int>(key);
            void* params[] = { &k };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetKeyDown(KeyCode key) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetKeyDown", 1);
            int k = static_cast<int>(key);
            void* params[] = { &k };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetKeyUp(KeyCode key) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetKeyUp", 1);
            int k = static_cast<int>(key);
            void* params[] = { &k };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetMouseButton(int button) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetMouseButton", 1);
            void* params[] = { &button };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetMouseButtonDown(int button) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetMouseButtonDown", 1);
            void* params[] = { &button };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetMouseButtonUp(int button) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetMouseButtonUp", 1);
            void* params[] = { &button };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetAnyKey() {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "get_anyKey", 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static bool GetAnyKeyDown() {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "get_anyKeyDown", 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static Vector3 GetMousePosition() {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "get_mousePosition", 0);
            return MethodHandler::invoke<Vector3>(m, nullptr);
        }

        [[nodiscard]] static Vector2 GetMouseScrollDelta() {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "get_mouseScrollDelta", 0);
            return MethodHandler::invoke<Vector2>(m, nullptr);
        }

        [[nodiscard]] static bool GetMousePresent() {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "get_mousePresent", 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static bool GetButton(std::string_view buttonName) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetButton", 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return false;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(buttonName).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetButtonDown(std::string_view buttonName) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetButtonDown", 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return false;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(buttonName).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetButtonUp(std::string_view buttonName) {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "GetButtonUp", 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return false;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(buttonName).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static int GetTouchCount() {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "get_touchCount", 0);
            return MethodHandler::invoke<int>(m, nullptr);
        }

        [[nodiscard]] static bool IsTouchSupported() {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "get_touchSupported", 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static Vector3 GetAcceleration() {
            static auto m = MethodHandler::resolve("UnityEngine.Input", "get_acceleration", 0);
            return MethodHandler::invoke<Vector3>(m, nullptr);
        }
    };

} // namespace IL2CPP::Module::Unity
