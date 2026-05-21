#pragma once
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    enum class CursorLockMode : int {
        None = 0,       // Cursor is free to move
        Locked = 1,     // Cursor is locked to center of screen
        Confined = 2    // Cursor is confined to game window
    };

    class Cursor {
    public:
        Cursor() = delete;

        [[nodiscard]] static bool GetVisible() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Cursor"), IL2CPP_STR("get_visible"), 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        static void SetVisible(bool visible) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Cursor"), IL2CPP_STR("set_visible"), 1);
            void* params[] = { &visible };
            MethodHandler::invoke(m, nullptr, params);
        }

        [[nodiscard]] static CursorLockMode GetLockState() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Cursor"), IL2CPP_STR("get_lockState"), 0);
            return static_cast<CursorLockMode>(MethodHandler::invoke<int>(m, nullptr));
        }

        static void SetLockState(CursorLockMode lockMode) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Cursor"), IL2CPP_STR("set_lockState"), 1);
            int val = static_cast<int>(lockMode);
            void* params[] = { &val };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void Lock() { SetLockState(CursorLockMode::Locked); }
        static void Unlock() { SetLockState(CursorLockMode::None); }
        static void Confine() { SetLockState(CursorLockMode::Confined); }
        static void Show() { SetVisible(true); }
        static void Hide() { SetVisible(false); }
    };

} // namespace IL2CPP::Module::Unity
