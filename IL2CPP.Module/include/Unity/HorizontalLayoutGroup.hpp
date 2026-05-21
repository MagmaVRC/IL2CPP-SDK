#pragma once
#include "LayoutGroup.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class HorizontalLayoutGroup : public LayoutGroup {
    public:
        using LayoutGroup::LayoutGroup;

        [[nodiscard]] float GetSpacing() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("get_spacing"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetSpacing(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("set_spacing"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetChildForceExpandWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("get_childForceExpandWidth"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetChildForceExpandWidth(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("set_childForceExpandWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetChildForceExpandHeight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("get_childForceExpandHeight"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetChildForceExpandHeight(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("set_childForceExpandHeight"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetChildControlWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("get_childControlWidth"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetChildControlWidth(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("set_childControlWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetChildControlHeight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("get_childControlHeight"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetChildControlHeight(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("set_childControlHeight"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetChildScaleWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("get_childScaleWidth"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetChildScaleWidth(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("set_childScaleWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetChildScaleHeight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("get_childScaleHeight"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetChildScaleHeight(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("set_childScaleHeight"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetReverseArrangement() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("get_reverseArrangement"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetReverseArrangement(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.HorizontalLayoutGroup"), IL2CPP_STR("set_reverseArrangement"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
