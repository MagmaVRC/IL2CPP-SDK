#pragma once
#include "UIBehaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class ScrollRect : public UIBehaviour {
    public:
        using UIBehaviour::UIBehaviour;

        [[nodiscard]] void* GetContent() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_content"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetContent(void* rectTransform) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_content"), 1);
            void* params[] = { rectTransform };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetHorizontal() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_horizontal"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetHorizontal(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_horizontal"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetVertical() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_vertical"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetVertical(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_vertical"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetMovementType() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_movementType"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetMovementType(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_movementType"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetElasticity() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_elasticity"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetElasticity(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_elasticity"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetInertia() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_inertia"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetInertia(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_inertia"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetDecelerationRate() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_decelerationRate"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetDecelerationRate(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_decelerationRate"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetScrollSensitivity() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_scrollSensitivity"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetScrollSensitivity(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_scrollSensitivity"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetHorizontalNormalizedPosition() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_horizontalNormalizedPosition"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetHorizontalNormalizedPosition(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_horizontalNormalizedPosition"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetVerticalNormalizedPosition() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_verticalNormalizedPosition"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetVerticalNormalizedPosition(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_verticalNormalizedPosition"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetNormalizedPosition() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_normalizedPosition"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetNormalizedPosition(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_normalizedPosition"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetVelocity() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_velocity"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetVelocity(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_velocity"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetViewport() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_viewport"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetViewport(void* rectTransform) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_viewport"), 1);
            void* params[] = { rectTransform };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetNormalizedPosition(float value, int axis) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("SetNormalizedPosition"), 2);
            void* params[] = { &value, &axis };
            MethodHandler::invoke(m, raw(), params);
        }

        void StopMovement() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("StopMovement"), 0);
            MethodHandler::invoke(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
