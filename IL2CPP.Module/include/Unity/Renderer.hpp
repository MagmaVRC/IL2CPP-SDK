#pragma once
#include "Component.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Renderer : public Component {
    public:
        using Component::Component;

        [[nodiscard]] bool GetEnabled() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_enabled"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetEnabled(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_enabled"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetMaterial() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_material"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetMaterial(void* material) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_material"), 1);
            void* params[] = { material };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetSharedMaterial() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_sharedMaterial"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetSharedMaterial(void* material) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_sharedMaterial"), 1);
            void* params[] = { material };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Bounds GetBounds() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_bounds"), 0);
            return MethodHandler::invoke<Bounds>(m, raw());
        }

        [[nodiscard]] bool GetIsPartOfStaticBatch() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_isPartOfStaticBatch"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] int GetSortingOrder() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_sortingOrder"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetSortingOrder(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_sortingOrder"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetSortingLayerID() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_sortingLayerID"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetSortingLayerID(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_sortingLayerID"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetIsVisible() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_isVisible"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] bool GetReceiveShadows() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_receiveShadows"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetReceiveShadows(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_receiveShadows"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetShadowCastingMode() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_shadowCastingMode"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetShadowCastingMode(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_shadowCastingMode"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void GetPropertyBlock(void* properties) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("GetPropertyBlock"), 1);
            void* params[] = { properties };
            MethodHandler::invoke(m, raw(), params);
        }
        void SetPropertyBlock(void* properties) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("SetPropertyBlock"), 1);
            void* params[] = { properties };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
