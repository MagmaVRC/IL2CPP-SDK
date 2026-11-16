#pragma once
#include "UIBehaviour.hpp"
#include "Scrollbar.hpp"
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

        /// <summary>Scrollbar driven by, and driving, the vertical offset; pass the Scrollbar's raw pointer.
        /// Wiring hands the bar's <c>value</c> and <c>size</c> to this ScrollRect: it recomputes both from its own
        /// viewport/content bounds on every layout pass and overwrites whatever a caller wrote, while a value
        /// change on the bar writes back through SetVerticalNormalizedPosition to the content's anchoredPosition.
        /// Published contract: wire the bar and then never write its value or size - read the offset from the
        /// ScrollRect instead. A caller that wants to drive value and size itself must leave the bar unwired and
        /// subscribe to onValueChanged by hand. Reserve the gutter yourself; see
        /// <c>SetVerticalScrollbarVisibility</c>.</summary>
        void SetVerticalScrollbar(void* scrollbar) {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_verticalScrollbar"), 1);
            void* params[] = { scrollbar };
            MethodHandler::invoke(m, raw(), params);
        }
        void SetVerticalScrollbar(const Scrollbar& scrollbar) {
            SetVerticalScrollbar(scrollbar.raw());
        }
        [[nodiscard]] void* GetVerticalScrollbarRaw() const {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_verticalScrollbar"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        /// <summary>0 Permanent, 1 AutoHide, 2 AutoHideAndExpandViewport. Permanent leaves the viewport rect
        /// entirely to the caller: the ScrollRect never resizes it and any gutter is the caller's to reserve.</summary>
        void SetVerticalScrollbarVisibility(int visibility) {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_verticalScrollbarVisibility"), 1);
            void* params[] = { &visibility };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] int GetVerticalScrollbarVisibility() const {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_verticalScrollbarVisibility"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        /// <summary>Gap between the viewport and the vertical scrollbar; only applied under
        /// AutoHideAndExpandViewport visibility.</summary>
        void SetVerticalScrollbarSpacing(float spacing) {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("set_verticalScrollbarSpacing"), 1);
            void* params[] = { &spacing };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] float GetVerticalScrollbarSpacing() const {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.ScrollRect"), IL2CPP_STR("get_verticalScrollbarSpacing"), 0);
            return MethodHandler::invoke<float>(m, raw());
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
