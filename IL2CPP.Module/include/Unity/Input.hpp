#pragma once
#include "../MethodHandler.hpp"
#include "../ICall.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::Unity {

    class Input {
        static constexpr int kDirectKeyEnd = static_cast<int>(KeyCode::Mouse6) + 1;
        static constexpr int kDirectMouseButtons = 7;
        static constexpr size_t kMaxCachedStrings = 32;

        using KeyIcall = bool(IL2CPP_CALLTYPE)(int32_t);
        using FlagIcall = bool(IL2CPP_CALLTYPE)();

        [[nodiscard]] static KeyIcall KeyFn(ICall& ic, const char* icallName, int value, int begin, int end) {
            if (value < begin || value >= end) return nullptr;
            if (!ic.ready()) ic.resolve(nullptr, nullptr, 1, icallName);
            return ic.as<KeyIcall>();
        }

        [[nodiscard]] static void* NewString(il2cpp_exports const* exports, std::string_view text) {
            return reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(text).c_str());
        }

        [[nodiscard]] static void* CachedString(std::string_view text) {
            struct Entry { std::string text; void* str; };
            static std::mutex s_mutex;
            static std::vector<Entry> s_cache;
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return nullptr;
            std::scoped_lock lock(s_mutex);
            for (const auto& e : s_cache)
                if (e.text == text) return e.str;
            void* str = NewString(exports, text);
            if (str && s_cache.size() < kMaxCachedStrings && exports->m_gcHandleNew
                && reinterpret_cast<uintptr_t(IL2CPP_CALLTYPE)(void*, uint8_t)>(exports->m_gcHandleNew)(str, uint8_t{0}))
                s_cache.push_back({ std::string(text), str });
            return str;
        }

    public:
        Input() = delete;

        [[nodiscard]] static float GetAxis(std::string_view axisName) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetAxis"), 1);
            void* str = CachedString(axisName);
            if (!str) return 0.f;
            void* params[] = { str };
            return MethodHandler::invoke<float>(m, nullptr, params);
        }

        [[nodiscard]] static float GetAxisRaw(std::string_view axisName) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetAxisRaw"), 1);
            void* str = CachedString(axisName);
            if (!str) return 0.f;
            void* params[] = { str };
            return MethodHandler::invoke<float>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetKey(KeyCode key) {
            static ICall ic;
            int k = static_cast<int>(key);
            if (auto fn = KeyFn(ic, IL2CPP_STR("UnityEngine.Input::GetKeyInt(UnityEngine.KeyCode)"), k, 1, kDirectKeyEnd)) return fn(k);
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetKey"), 1);
            void* params[] = { &k };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetKeyDown(KeyCode key) {
            static ICall ic;
            int k = static_cast<int>(key);
            if (auto fn = KeyFn(ic, IL2CPP_STR("UnityEngine.Input::GetKeyDownInt(UnityEngine.KeyCode)"), k, 1, kDirectKeyEnd)) return fn(k);
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetKeyDown"), 1);
            void* params[] = { &k };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetKeyUp(KeyCode key) {
            static ICall ic;
            int k = static_cast<int>(key);
            if (auto fn = KeyFn(ic, IL2CPP_STR("UnityEngine.Input::GetKeyUpInt(UnityEngine.KeyCode)"), k, 1, kDirectKeyEnd)) return fn(k);
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetKeyUp"), 1);
            void* params[] = { &k };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetMouseButton(int button) {
            static ICall ic;
            if (auto fn = KeyFn(ic, IL2CPP_STR("UnityEngine.Input::GetMouseButton(System.Int32)"), button, 0, kDirectMouseButtons)) return fn(button);
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetMouseButton"), 1);
            void* params[] = { &button };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetMouseButtonDown(int button) {
            static ICall ic;
            if (auto fn = KeyFn(ic, IL2CPP_STR("UnityEngine.Input::GetMouseButtonDown(System.Int32)"), button, 0, kDirectMouseButtons)) return fn(button);
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetMouseButtonDown"), 1);
            void* params[] = { &button };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetMouseButtonUp(int button) {
            static ICall ic;
            if (auto fn = KeyFn(ic, IL2CPP_STR("UnityEngine.Input::GetMouseButtonUp(System.Int32)"), button, 0, kDirectMouseButtons)) return fn(button);
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetMouseButtonUp"), 1);
            void* params[] = { &button };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetAnyKey() {
            static ICall ic;
            if (!ic.ready()) ic.resolve(nullptr, nullptr, 0, IL2CPP_STR("UnityEngine.Input::get_anyKey()"));
            if (auto fn = ic.as<FlagIcall>()) return fn();
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("get_anyKey"), 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static bool GetAnyKeyDown() {
            static ICall ic;
            if (!ic.ready()) ic.resolve(nullptr, nullptr, 0, IL2CPP_STR("UnityEngine.Input::get_anyKeyDown()"));
            if (auto fn = ic.as<FlagIcall>()) return fn();
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("get_anyKeyDown"), 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static Vector3 GetMousePosition() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("get_mousePosition"), 0);
            return MethodHandler::invoke<Vector3>(m, nullptr);
        }

        [[nodiscard]] static Vector2 GetMouseScrollDelta() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("get_mouseScrollDelta"), 0);
            return MethodHandler::invoke<Vector2>(m, nullptr);
        }

        [[nodiscard]] static bool GetMousePresent() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("get_mousePresent"), 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static bool GetButton(std::string_view buttonName) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetButton"), 1);
            void* str = CachedString(buttonName);
            if (!str) return false;
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetButtonDown(std::string_view buttonName) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetButtonDown"), 1);
            void* str = CachedString(buttonName);
            if (!str) return false;
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static bool GetButtonUp(std::string_view buttonName) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("GetButtonUp"), 1);
            void* str = CachedString(buttonName);
            if (!str) return false;
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static int GetTouchCount() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("get_touchCount"), 0);
            return MethodHandler::invoke<int>(m, nullptr);
        }

        [[nodiscard]] static bool IsTouchSupported() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("get_touchSupported"), 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static Vector3 GetAcceleration() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Input"), IL2CPP_STR("get_acceleration"), 0);
            return MethodHandler::invoke<Vector3>(m, nullptr);
        }
    };

} // namespace IL2CPP::Module::Unity
