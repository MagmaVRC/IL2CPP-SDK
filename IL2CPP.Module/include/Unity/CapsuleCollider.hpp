#pragma once
#include "Collider.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class CapsuleCollider : public Collider {
    public:
        using Collider::Collider;

        [[nodiscard]] Vector3 GetCenter() const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("get_center"))) return {};
            return MethodHandler::invoke<Vector3>(m, raw());
        }

        [[nodiscard]] float GetRadius() const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("get_radius"))) return 0.f;
            return MethodHandler::invoke<float>(m, raw());
        }

        [[nodiscard]] float GetHeight() const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("get_height"))) return 0.f;
            return MethodHandler::invoke<float>(m, raw());
        }

        /// <summary>The capsule's axis: 0 X, 1 Y, 2 Z.</summary>
        [[nodiscard]] int GetDirection() const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("get_direction"))) return 0;
            return MethodHandler::invoke<int>(m, raw());
        }

    private:
        static bool Lazy(Method& m, const char* name) {
            if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.CapsuleCollider"), name, 0);
            return static_cast<bool>(m);
        }
    };

} // namespace IL2CPP::Module::Unity
