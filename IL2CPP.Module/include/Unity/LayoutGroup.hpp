#pragma once
#include "UIBehaviour.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class LayoutGroup : public UIBehaviour {
    public:
        using UIBehaviour::UIBehaviour;

        [[nodiscard]] void* GetPadding() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutGroup"), IL2CPP_STR("get_padding"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetPadding(void* value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutGroup"), IL2CPP_STR("set_padding"), 1);
            void* params[] = { value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetChildAlignment() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutGroup"), IL2CPP_STR("get_childAlignment"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetChildAlignment(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutGroup"), IL2CPP_STR("set_childAlignment"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetLayoutHorizontal() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutGroup"), IL2CPP_STR("SetLayoutHorizontal"), 0);
            MethodHandler::invoke(m, raw());
        }
        void SetLayoutVertical() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.LayoutGroup"), IL2CPP_STR("SetLayoutVertical"), 0);
            MethodHandler::invoke(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
