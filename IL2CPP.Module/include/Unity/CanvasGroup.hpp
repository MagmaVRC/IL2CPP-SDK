#pragma once
#include "Behaviour.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class CanvasGroup : public Behaviour {
    public:
        using Behaviour::Behaviour;

        [[nodiscard]] float GetAlpha() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.CanvasGroup"), IL2CPP_STR("get_alpha"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetAlpha(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.CanvasGroup"), IL2CPP_STR("set_alpha"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetInteractable() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.CanvasGroup"), IL2CPP_STR("get_interactable"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetInteractable(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.CanvasGroup"), IL2CPP_STR("set_interactable"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetBlocksRaycasts() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.CanvasGroup"), IL2CPP_STR("get_blocksRaycasts"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetBlocksRaycasts(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.CanvasGroup"), IL2CPP_STR("set_blocksRaycasts"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetIgnoreParentGroups() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.CanvasGroup"), IL2CPP_STR("get_ignoreParentGroups"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetIgnoreParentGroups(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.CanvasGroup"), IL2CPP_STR("set_ignoreParentGroups"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
