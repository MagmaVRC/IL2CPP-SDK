#pragma once
#include "UIBehaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Selectable : public UIBehaviour {
    public:
        using UIBehaviour::UIBehaviour;

        [[nodiscard]] bool GetInteractable() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("get_interactable"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetInteractable(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("set_interactable"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool IsInteractable() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("IsInteractable"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] void* GetTargetGraphicRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("get_targetGraphic"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetTargetGraphicRaw(void* graphic) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("set_targetGraphic"), 1);
            void* params[] = { graphic };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetImageRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("get_image"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetImageRaw(void* image) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("set_image"), 1);
            void* params[] = { image };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetTransition() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("get_transition"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetTransition(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("set_transition"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void Select() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("Select"), 0);
            MethodHandler::invoke(m, raw());
        }

        /// <summary>UnityEngine.UI.ColorBlock: the ColorTint transition colours multiplied onto the target graphic.</summary>
        struct ColorBlock {
            Color normalColor{ 1, 1, 1, 1 };
            Color highlightedColor{ 1, 1, 1, 1 };
            Color pressedColor{ 1, 1, 1, 1 };
            Color selectedColor{ 1, 1, 1, 1 };
            Color disabledColor{ 1, 1, 1, 0.5f };
            float colorMultiplier = 1.0f;
            float fadeDuration = 0.1f;
        };
        static_assert(sizeof(ColorBlock) == 5 * sizeof(Color) + 2 * sizeof(float));

        void SetColors(const ColorBlock& colors) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Selectable"), IL2CPP_STR("set_colors"), 1);
            ColorBlock c = colors;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
