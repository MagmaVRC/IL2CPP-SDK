#pragma once
#include "Collider.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class BoxCollider : public Collider {
    public:
        using Collider::Collider;

        [[nodiscard]] Vector3 GetCenter() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.BoxCollider"), IL2CPP_STR("get_center"), 0);
            return MethodHandler::invoke<Vector3>(m, raw());
        }
        void SetCenter(const Vector3& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.BoxCollider"), IL2CPP_STR("set_center"), 1);
            Vector3 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector3 GetSize() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.BoxCollider"), IL2CPP_STR("get_size"), 0);
            return MethodHandler::invoke<Vector3>(m, raw());
        }
        void SetSize(const Vector3& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.BoxCollider"), IL2CPP_STR("set_size"), 1);
            Vector3 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
