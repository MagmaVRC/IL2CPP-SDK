#pragma once
#include "../MethodHandler.hpp"
#include "../ICall.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <cstdint>

namespace IL2CPP::Module::Unity {

    enum class FullScreenMode : int {
        ExclusiveFullScreen = 0,
        FullScreenWindow = 1,
        MaximizedWindow = 2,
        Windowed = 3
    };

    struct Resolution {
        int width;
        int height;
        int refreshRate;
    };

    class Screen {
    public:
        Screen() = delete;

        [[nodiscard]] static int GetWidth() {
            static ICall ic;
            if (!ic.ready()) ic.resolve(nullptr, nullptr, 0, IL2CPP_STR("UnityEngine.Screen::get_width()"));
            if (auto fn = ic.as<int32_t(IL2CPP_CALLTYPE)()>()) return fn();
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Screen"), IL2CPP_STR("get_width"), 0);
            return MethodHandler::invoke<int>(m, nullptr);
        }

        [[nodiscard]] static int GetHeight() {
            static ICall ic;
            if (!ic.ready()) ic.resolve(nullptr, nullptr, 0, IL2CPP_STR("UnityEngine.Screen::get_height()"));
            if (auto fn = ic.as<int32_t(IL2CPP_CALLTYPE)()>()) return fn();
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Screen"), IL2CPP_STR("get_height"), 0);
            return MethodHandler::invoke<int>(m, nullptr);
        }

        [[nodiscard]] static float GetDpi() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Screen"), IL2CPP_STR("get_dpi"), 0);
            return MethodHandler::invoke<float>(m, nullptr);
        }

        [[nodiscard]] static bool GetFullScreen() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Screen"), IL2CPP_STR("get_fullScreen"), 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        static void SetFullScreen(bool fullScreen) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Screen"), IL2CPP_STR("set_fullScreen"), 1);
            void* params[] = { &fullScreen };
            MethodHandler::invoke(m, nullptr, params);
        }

        [[nodiscard]] static FullScreenMode GetFullScreenMode() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Screen"), IL2CPP_STR("get_fullScreenMode"), 0);
            return static_cast<FullScreenMode>(MethodHandler::invoke<int>(m, nullptr));
        }

        /// <summary>Not present on every build - VRChat 1832 ships only the
        /// getter, so check the handle before relying on this; SetFullScreen or
        /// SetResolution is the portable way to change presentation.</summary>
        static void SetFullScreenMode(FullScreenMode mode) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Screen"), IL2CPP_STR("set_fullScreenMode"), 1);
            int val = static_cast<int>(mode);
            void* params[] = { &val };
            MethodHandler::invoke(m, nullptr, params);
        }

        [[nodiscard]] static Resolution GetCurrentResolution() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Screen"), IL2CPP_STR("get_currentResolution"), 0);
            return MethodHandler::invoke<Resolution>(m, nullptr);
        }

        static void SetResolution(int width, int height, bool fullScreen, int refreshRate = 0) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Screen"), IL2CPP_STR("SetResolution"), 4);
            void* params[] = { &width, &height, &fullScreen, &refreshRate };
            MethodHandler::invoke(m, nullptr, params);
        }
    };

} // namespace IL2CPP::Module::Unity
