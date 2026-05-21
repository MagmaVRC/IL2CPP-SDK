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

        [[nodiscard]] Rigidbody GetAttachedRigidbody() const;
    };

} // namespace IL2CPP::Module::Unity
