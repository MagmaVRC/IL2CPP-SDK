#pragma once
#include "Graphic.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class MaskableGraphic : public Graphic {
    public:
        using Graphic::Graphic;

        [[nodiscard]] bool GetMaskable() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.MaskableGraphic"), IL2CPP_STR("get_maskable"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetMaskable(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.MaskableGraphic"), IL2CPP_STR("set_maskable"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetModifiedMaterial(void* baseMaterial) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.MaskableGraphic"), IL2CPP_STR("GetModifiedMaterial"), 1);
            void* params[] = { baseMaterial };
            return MethodHandler::invoke<void*>(m, raw(), params);
        }

        void RecalculateClipping() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.MaskableGraphic"), IL2CPP_STR("RecalculateClipping"), 0);
            MethodHandler::invoke(m, raw());
        }

        void RecalculateMasking() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.MaskableGraphic"), IL2CPP_STR("RecalculateMasking"), 0);
            MethodHandler::invoke(m, raw());
        }

        void SetClipRect(const Rect& clipRect, bool validRect) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.MaskableGraphic"), IL2CPP_STR("SetClipRect"), 2);
            Rect r = clipRect;
            void* params[] = { &r, &validRect };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetClipSoftness(const Vector2& clipSoftness) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.MaskableGraphic"), IL2CPP_STR("SetClipSoftness"), 1);
            Vector2 v = clipSoftness;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        void Cull(const Rect& clipRect, bool validRect) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.MaskableGraphic"), IL2CPP_STR("Cull"), 2);
            Rect r = clipRect;
            void* params[] = { &r, &validRect };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
