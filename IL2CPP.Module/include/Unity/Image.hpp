#pragma once
#include "MaskableGraphic.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Image : public MaskableGraphic {
    public:
        using MaskableGraphic::MaskableGraphic;

        [[nodiscard]] void* GetSprite() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("get_sprite"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetSprite(void* sprite) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("set_sprite"), 1);
            void* params[] = { sprite };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetActiveSprite() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("get_activeSprite"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        [[nodiscard]] void* GetOverrideSprite() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("get_overrideSprite"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetOverrideSprite(void* sprite) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("set_overrideSprite"), 1);
            void* params[] = { sprite };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetFillAmount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("get_fillAmount"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetFillAmount(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("set_fillAmount"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Color GetColor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("get_color"), 0);
            return MethodHandler::invoke<Color>(m, raw());
        }
        void SetColor(const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("set_color"), 1);
            Color c = value;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetType() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("get_type"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetType(int type) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("set_type"), 1);
            void* params[] = { &type };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetPreserveAspect() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("get_preserveAspect"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetPreserveAspect(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Image"), IL2CPP_STR("set_preserveAspect"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
