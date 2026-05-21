#pragma once
#include "Behaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Canvas : public Behaviour {
    public:
        using Behaviour::Behaviour;

        [[nodiscard]] int GetRenderMode() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("get_renderMode"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetRenderMode(int mode) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("set_renderMode"), 1);
            void* params[] = { &mode };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetSortingOrder() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("get_sortingOrder"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetSortingOrder(int order) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("set_sortingOrder"), 1);
            void* params[] = { &order };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetScaleFactor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("get_scaleFactor"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetScaleFactor(float factor) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("set_scaleFactor"), 1);
            void* params[] = { &factor };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetOverrideSorting() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("get_overrideSorting"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetOverrideSorting(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("set_overrideSorting"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetPixelPerfect() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("get_pixelPerfect"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetPixelPerfect(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("set_pixelPerfect"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetWorldCamera() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("get_worldCamera"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        [[nodiscard]] Canvas GetRootCanvas() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("get_rootCanvas"), 0);
            return Canvas{ MethodHandler::invoke<void*>(m, raw()) };
        }

        static void ForceUpdateCanvases() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Canvas"), IL2CPP_STR("ForceUpdateCanvases"), 0);
            MethodHandler::invoke(m, nullptr);
        }
    };

} // namespace IL2CPP::Module::Unity
