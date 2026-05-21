#pragma once
#include "Collider.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class MeshCollider : public Collider {
    public:
        using Collider::Collider;

        [[nodiscard]] void* GetSharedMesh() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MeshCollider"), IL2CPP_STR("get_sharedMesh"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetSharedMesh(void* mesh) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MeshCollider"), IL2CPP_STR("set_sharedMesh"), 1);
            void* params[] = { mesh };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetConvex() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MeshCollider"), IL2CPP_STR("get_convex"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetConvex(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MeshCollider"), IL2CPP_STR("set_convex"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetCookingOptions() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MeshCollider"), IL2CPP_STR("get_cookingOptions"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetCookingOptions(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MeshCollider"), IL2CPP_STR("set_cookingOptions"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
