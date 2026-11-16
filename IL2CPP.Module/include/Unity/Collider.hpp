#pragma once
#include "Component.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class Rigidbody;  // Forward declaration

    class Collider : public Component {
    public:
        using Component::Component;

        [[nodiscard]] bool GetEnabled() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Collider"), IL2CPP_STR("get_enabled"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        void SetEnabled(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Collider"), IL2CPP_STR("set_enabled"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetIsTrigger() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Collider"), IL2CPP_STR("get_isTrigger"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        void SetIsTrigger(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Collider"), IL2CPP_STR("set_isTrigger"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        /// <summary>Get the collider's world-space bounding box.</summary>
        /// <returns>A zero box when the collider is disabled, inactive or the getter could not be resolved.</returns>
        [[nodiscard]] Bounds GetBounds() const {
            static Method m;
            if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Collider"), IL2CPP_STR("get_bounds"), 0);
            return m ? MethodHandler::invoke<Bounds>(m, raw()) : Bounds{};
        }

        [[nodiscard]] Rigidbody GetAttachedRigidbody() const;
    };

} // namespace IL2CPP::Module::Unity
