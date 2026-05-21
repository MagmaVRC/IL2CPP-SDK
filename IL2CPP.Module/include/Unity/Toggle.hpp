#pragma once
#include "Selectable.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class Toggle : public Selectable {
    public:
        using Selectable::Selectable;

        [[nodiscard]] bool GetIsOn() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Toggle"), IL2CPP_STR("get_isOn"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetIsOn(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Toggle"), IL2CPP_STR("set_isOn"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetIsOnWithoutNotify(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Toggle"), IL2CPP_STR("SetIsOnWithoutNotify"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetOnValueChangedRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Toggle"), IL2CPP_STR("get_onValueChanged"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
