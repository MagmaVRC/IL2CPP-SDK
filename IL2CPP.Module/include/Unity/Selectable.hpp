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
    };

} // namespace IL2CPP::Module::Unity
