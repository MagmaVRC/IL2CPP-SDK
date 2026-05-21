#pragma once
#include "UIBehaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class RectMask2D : public UIBehaviour {
    public:
        using UIBehaviour::UIBehaviour;

        [[nodiscard]] Vector4 GetPadding() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RectMask2D"), IL2CPP_STR("get_padding"), 0);
            return MethodHandler::invoke<Vector4>(m, raw());
        }
        void SetPadding(const Vector4& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RectMask2D"), IL2CPP_STR("set_padding"), 1);
            Vector4 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2Int GetSoftness() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RectMask2D"), IL2CPP_STR("get_softness"), 0);
            return MethodHandler::invoke<Vector2Int>(m, raw());
        }
        void SetSoftness(const Vector2Int& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RectMask2D"), IL2CPP_STR("set_softness"), 1);
            Vector2Int v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        void PerformClipping() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RectMask2D"), IL2CPP_STR("PerformClipping"), 0);
            MethodHandler::invoke(m, raw());
        }

        void AddClippable(void* clippable) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RectMask2D"), IL2CPP_STR("AddClippable"), 1);
            void* params[] = { clippable };
            MethodHandler::invoke(m, raw(), params);
        }
        void RemoveClippable(void* clippable) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RectMask2D"), IL2CPP_STR("RemoveClippable"), 1);
            void* params[] = { clippable };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
