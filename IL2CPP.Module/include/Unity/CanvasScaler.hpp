#pragma once
#include "UIBehaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class CanvasScaler : public UIBehaviour {
    public:
        using UIBehaviour::UIBehaviour;

        [[nodiscard]] int GetUiScaleMode() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("get_uiScaleMode"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetUiScaleMode(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("set_uiScaleMode"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetScaleFactor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("get_scaleFactor"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetScaleFactor(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("set_scaleFactor"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetReferenceResolution() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("get_referenceResolution"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetReferenceResolution(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("set_referenceResolution"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetScreenMatchMode() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("get_screenMatchMode"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetScreenMatchMode(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("set_screenMatchMode"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetMatchWidthOrHeight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("get_matchWidthOrHeight"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetMatchWidthOrHeight(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("set_matchWidthOrHeight"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetReferencePixelsPerUnit() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("get_referencePixelsPerUnit"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetReferencePixelsPerUnit(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.CanvasScaler"), IL2CPP_STR("set_referencePixelsPerUnit"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
