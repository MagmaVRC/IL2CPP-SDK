#pragma once
#include "UIBehaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Graphic : public UIBehaviour {
    public:
        using UIBehaviour::UIBehaviour;

        [[nodiscard]] Color GetColor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_color"), 0);
            return MethodHandler::invoke<Color>(m, raw());
        }
        void SetColor(const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("set_color"), 1);
            Color c = value;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetRaycastTarget() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_raycastTarget"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetRaycastTarget(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("set_raycastTarget"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector4 GetRaycastPadding() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_raycastPadding"), 0);
            return MethodHandler::invoke<Vector4>(m, raw());
        }
        void SetRaycastPadding(const Vector4& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("set_raycastPadding"), 1);
            Vector4 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetDepth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_depth"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] void* GetRectTransformRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_rectTransform"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        [[nodiscard]] void* GetCanvasRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_canvas"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        [[nodiscard]] void* GetCanvasRendererRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_canvasRenderer"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        [[nodiscard]] void* GetMaterial() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_material"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetMaterial(void* material) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("set_material"), 1);
            void* params[] = { material };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetDefaultMaterial() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_defaultMaterial"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        [[nodiscard]] void* GetMaterialForRendering() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_materialForRendering"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        [[nodiscard]] void* GetMainTexture() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("get_mainTexture"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        void SetAllDirty() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("SetAllDirty"), 0);
            MethodHandler::invoke(m, raw());
        }

        void SetLayoutDirty() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("SetLayoutDirty"), 0);
            MethodHandler::invoke(m, raw());
        }

        void SetVerticesDirty() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("SetVerticesDirty"), 0);
            MethodHandler::invoke(m, raw());
        }

        void SetMaterialDirty() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("SetMaterialDirty"), 0);
            MethodHandler::invoke(m, raw());
        }

        void Rebuild(int canvasUpdate) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("Rebuild"), 1);
            void* params[] = { &canvasUpdate };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetNativeSize() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("SetNativeSize"), 0);
            MethodHandler::invoke(m, raw());
        }

        void CrossFadeAlpha(float alpha, float duration, bool ignoreTimeScale) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("CrossFadeAlpha"), 3);
            void* params[] = { &alpha, &duration, &ignoreTimeScale };
            MethodHandler::invoke(m, raw(), params);
        }

        void CrossFadeColor(const Color& targetColor, float duration, bool ignoreTimeScale, bool useAlpha) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("CrossFadeColor"), 4);
            Color c = targetColor;
            void* params[] = { &c, &duration, &ignoreTimeScale, &useAlpha };
            MethodHandler::invoke(m, raw(), params);
        }

        void GraphicUpdateComplete() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Graphic"), IL2CPP_STR("GraphicUpdateComplete"), 0);
            MethodHandler::invoke(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
