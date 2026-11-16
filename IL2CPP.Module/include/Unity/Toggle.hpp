#pragma once
#include "Selectable.hpp"
#include "../MethodHandler.hpp"
#include "../Reflection.hpp"

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
            static auto f = Class::find(IL2CPP_STR("UnityEngine.UI.Toggle")).get_field(IL2CPP_STR("onValueChanged"));
            if (!f || !raw()) return nullptr;
            return *reinterpret_cast<void**>(static_cast<char*>(raw()) + f.offset());
        }

        /// <summary>Set the checkmark graphic Unity fades with isOn; null disables the fade.</summary>
        void SetGraphicRaw(void* graphic) {
            static auto f = Class::find(IL2CPP_STR("UnityEngine.UI.Toggle")).get_field(IL2CPP_STR("graphic"));
            if (!f || !raw()) return;
            if (graphic) f.set_value(raw(), graphic);
            else *reinterpret_cast<void**>(static_cast<char*>(raw()) + f.offset()) = nullptr;
        }
    };

} // namespace IL2CPP::Module::Unity
