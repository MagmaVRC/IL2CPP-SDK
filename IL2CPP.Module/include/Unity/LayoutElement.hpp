#pragma once
#include "UIBehaviour.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class LayoutElement : public UIBehaviour {
    public:
        using UIBehaviour::UIBehaviour;

        [[nodiscard]] bool GetIgnoreLayout() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("get_ignoreLayout"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetIgnoreLayout(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("set_ignoreLayout"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetMinWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("get_minWidth"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetMinWidth(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("set_minWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetMinHeight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("get_minHeight"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetMinHeight(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("set_minHeight"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetPreferredWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("get_preferredWidth"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetPreferredWidth(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("set_preferredWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetPreferredHeight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("get_preferredHeight"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetPreferredHeight(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("set_preferredHeight"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetFlexibleWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("get_flexibleWidth"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetFlexibleWidth(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("set_flexibleWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetFlexibleHeight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("get_flexibleHeight"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetFlexibleHeight(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("set_flexibleHeight"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetLayoutPriority() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("get_layoutPriority"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetLayoutPriority(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutElement"), IL2CPP_STR("set_layoutPriority"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
