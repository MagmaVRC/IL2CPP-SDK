#pragma once
#include "Component.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Renderer : public Component {
    public:
        using Component::Component;

        [[nodiscard]] bool GetEnabled() const {
            static ICall ic;
            if (!ic.ready()) ic.resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_enabled_Injected"), 1,
                IL2CPP_STR("UnityEngine.Renderer::get_enabled_Injected(System.IntPtr)"));
            if (auto fn = ic.as<bool(IL2CPP_CALLTYPE)(void*)>())
                if (void* self = ICall::unity_self(raw())) return fn(self);
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

        /// <summary>Get the shared material array (Material[]).</summary>
        [[nodiscard]] void* GetSharedMaterials() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_sharedMaterials"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        /// <summary>Replace the shared material array (Material[]).</summary>
        void SetSharedMaterials(void* materials) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_sharedMaterials"), 1);
            void* params[] = { materials };
            MethodHandler::invoke<void>(m, raw(), params);
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

        // The matrix Unity actually renders this renderer with (for skinned
        // renderers this is not always the Transform's localToWorld).
        [[nodiscard]] Matrix4x4 GetLocalToWorldMatrix() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_localToWorldMatrix"), 0);
            return MethodHandler::invoke<Matrix4x4>(m, raw());
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

        [[nodiscard]] bool GetAllowOcclusionWhenDynamic() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_allowOcclusionWhenDynamic"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetAllowOcclusionWhenDynamic(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_allowOcclusionWhenDynamic"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        /// <summary>Whether the renderer is skipped by normal rendering. Command-buffer draws still reach it.</summary>
        [[nodiscard]] bool GetForceRenderingOff() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("get_forceRenderingOff"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetForceRenderingOff(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Renderer"), IL2CPP_STR("set_forceRenderingOff"), 1);
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
