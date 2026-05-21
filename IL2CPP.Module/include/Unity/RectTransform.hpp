#pragma once
#include "Transform.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class RectTransform : public Transform {
    public:
        using Transform::Transform;

        [[nodiscard]] Vector2 GetAnchoredPosition() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("get_anchoredPosition"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetAnchoredPosition(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("set_anchoredPosition"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetSizeDelta() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("get_sizeDelta"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetSizeDelta(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("set_sizeDelta"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetAnchorMin() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("get_anchorMin"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetAnchorMin(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("set_anchorMin"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetAnchorMax() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("get_anchorMax"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetAnchorMax(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("set_anchorMax"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetPivot() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("get_pivot"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetPivot(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("set_pivot"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetOffsetMin() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("get_offsetMin"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetOffsetMin(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("set_offsetMin"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetOffsetMax() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("get_offsetMax"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetOffsetMax(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("set_offsetMax"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Rect GetRect() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RectTransform"), IL2CPP_STR("get_rect"), 0);
            return MethodHandler::invoke<Rect>(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
