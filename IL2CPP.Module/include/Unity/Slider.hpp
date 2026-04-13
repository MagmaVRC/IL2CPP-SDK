#pragma once
#include "Selectable.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class Slider : public Selectable {
    public:
        using Selectable::Selectable;

        [[nodiscard]] float GetValue() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.Slider", "get_value", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetValue(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.Slider", "set_value", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetMinValue() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.Slider", "get_minValue", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetMinValue(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.Slider", "set_minValue", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetMaxValue() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.Slider", "get_maxValue", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetMaxValue(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.Slider", "set_maxValue", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetWholeNumbers() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.Slider", "get_wholeNumbers", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetWholeNumbers(bool value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.Slider", "set_wholeNumbers", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetOnValueChangedRaw() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.Slider", "get_onValueChanged", 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
