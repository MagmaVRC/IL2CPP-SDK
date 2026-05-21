#pragma once
#include "Component.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>

namespace IL2CPP::Module::Unity {

    class Transform : public Component {
    public:
        using Component::Component;


        [[nodiscard]] Vector3 GetPosition() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_position"), 0);
            return MethodHandler::invoke<Vector3>(m, raw());
        }
        void SetPosition(const Vector3& pos) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("set_position"), 1);
            Vector3 p = pos;
            void* params[] = { &p };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector3 GetLocalPosition() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_localPosition"), 0);
            return MethodHandler::invoke<Vector3>(m, raw());
        }
        void SetLocalPosition(const Vector3& pos) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("set_localPosition"), 1);
            Vector3 p = pos;
            void* params[] = { &p };
            MethodHandler::invoke(m, raw(), params);
        }


        [[nodiscard]] Quaternion GetRotation() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_rotation"), 0);
            return MethodHandler::invoke<Quaternion>(m, raw());
        }
        void SetRotation(const Quaternion& rot) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("set_rotation"), 1);
            Quaternion r = rot;
            void* params[] = { &r };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Quaternion GetLocalRotation() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_localRotation"), 0);
            return MethodHandler::invoke<Quaternion>(m, raw());
        }
        void SetLocalRotation(const Quaternion& rot) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("set_localRotation"), 1);
            Quaternion r = rot;
            void* params[] = { &r };
            MethodHandler::invoke(m, raw(), params);
        }


        [[nodiscard]] Vector3 GetLocalScale() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_localScale"), 0);
            return MethodHandler::invoke<Vector3>(m, raw());
        }
        void SetLocalScale(const Vector3& scale) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("set_localScale"), 1);
            Vector3 s = scale;
            void* params[] = { &s };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector3 GetLossyScale() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_lossyScale"), 0);
            return MethodHandler::invoke<Vector3>(m, raw());
        }


        [[nodiscard]] Vector3 GetForward() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_forward"), 0);
            return MethodHandler::invoke<Vector3>(m, raw());
        }
        void SetForward(const Vector3& fwd) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("set_forward"), 1);
            Vector3 f = fwd;
            void* params[] = { &f };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector3 GetRight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_right"), 0);
            return MethodHandler::invoke<Vector3>(m, raw());
        }
        void SetRight(const Vector3& r) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("set_right"), 1);
            Vector3 rv = r;
            void* params[] = { &rv };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector3 GetUp() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_up"), 0);
            return MethodHandler::invoke<Vector3>(m, raw());
        }
        void SetUp(const Vector3& u) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("set_up"), 1);
            Vector3 uv = u;
            void* params[] = { &uv };
            MethodHandler::invoke(m, raw(), params);
        }


        [[nodiscard]] Transform GetParent() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_parent"), 0);
            return Transform{ MethodHandler::invoke<void*>(m, raw()) };
        }
        void SetParent(Transform parent, bool worldPositionStays = true) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("SetParent"), 2);
            void* p = parent.raw();
            void* params[] = { p, &worldPositionStays };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Transform GetRoot() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_root"), 0);
            return Transform{ MethodHandler::invoke<void*>(m, raw()) };
        }

        [[nodiscard]] int GetChildCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_childCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] Transform GetChild(int index) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("GetChild"), 1);
            void* params[] = { &index };
            return Transform{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        [[nodiscard]] int GetHierarchyCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_hierarchyCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] bool IsChildOf(Transform parent) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("IsChildOf"), 1);
            void* p = parent.raw();
            void* params[] = { p };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }

        void DetachChildren() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("DetachChildren"), 0);
            MethodHandler::invoke(m, raw());
        }

        [[nodiscard]] Transform Find(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("Find"), 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return Transform{};
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(name).c_str());
            void* params[] = { il2cppStr };
            return Transform{ MethodHandler::invoke<void*>(m, raw(), params) };
        }


        void SetPositionAndRotation(const Vector3& pos, const Quaternion& rot) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("SetPositionAndRotation"), 2);
            Vector3 p = pos;
            Quaternion r = rot;
            void* params[] = { &p, &r };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetLocalPositionAndRotation(const Vector3& pos, const Quaternion& rot) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("SetLocalPositionAndRotation"), 2);
            Vector3 p = pos;
            Quaternion r = rot;
            void* params[] = { &p, &r };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector3 TransformDirection(const Vector3& dir) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("TransformDirection"), 1);
            Vector3 d = dir;
            void* params[] = { &d };
            return MethodHandler::invoke<Vector3>(m, raw(), params);
        }

        [[nodiscard]] Vector3 InverseTransformDirection(const Vector3& dir) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("InverseTransformDirection"), 1);
            Vector3 d = dir;
            void* params[] = { &d };
            return MethodHandler::invoke<Vector3>(m, raw(), params);
        }

        [[nodiscard]] Vector3 TransformPoint(const Vector3& point) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("TransformPoint"), 1);
            Vector3 p = point;
            void* params[] = { &p };
            return MethodHandler::invoke<Vector3>(m, raw(), params);
        }


        /// <summary>Rotate around a point in world space.</summary>
        void RotateAround(const Vector3& point, const Vector3& axis, float angle) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("RotateAround"), 3);
            Vector3 p = point;
            Vector3 a = axis;
            void* params[] = { &p, &a, &angle };
            MethodHandler::invoke(m, raw(), params);
        }

        /// <summary>Rotate around a point in local space.</summary>
        void RotateAroundLocal(const Vector3& point, const Vector3& axis, float angle) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("RotateAroundLocal"), 3);
            Vector3 p = point;
            Vector3 a = axis;
            void* params[] = { &p, &a, &angle };
            MethodHandler::invoke(m, raw(), params);
        }


        /// <summary>Make the transform look at a target position.</summary>
        void LookAt(const Vector3& worldPosition) {
            Vector3 dir = worldPosition - GetPosition();
            if (dir.SqrMagnitude() < 0.0001f) return;
            dir.NormalizeInPlace();
            Quaternion rot;
            rot = rot.Euler(Vector3(
                std::atan2(-dir.y, std::sqrt(dir.x * dir.x + dir.z * dir.z)) * RAD2DEG,
                std::atan2(dir.x, dir.z) * RAD2DEG,
                0.f
            ));
            SetRotation(rot);
        }

        /// <summary>Make the transform look at another transform's position.</summary>
        void LookAt(Transform target) {
            if (!target) return;
            LookAt(target.GetPosition());
        }

        /// <summary>Rotate the transform by euler angles (degrees).</summary>
        void Rotate(const Vector3& eulers, Space relativeTo = Space::Self) {
            Quaternion deltaRot;
            deltaRot = deltaRot.Euler(eulers);
            if (relativeTo == Space::Self) {
                SetLocalRotation(GetLocalRotation() * deltaRot);
            } else {
                SetRotation(deltaRot * GetRotation());
            }
        }

        /// <summary>Rotate the transform around an axis by an angle (degrees).</summary>
        void Rotate(const Vector3& axis, float angle, Space relativeTo = Space::Self) {
            float halfAngle = angle * DEG2RAD * 0.5f;
            float s = std::sin(halfAngle);
            Quaternion deltaRot(axis.x * s, axis.y * s, axis.z * s, std::cos(halfAngle));
            if (relativeTo == Space::Self) {
                SetLocalRotation(GetLocalRotation() * deltaRot);
            } else {
                SetRotation(deltaRot * GetRotation());
            }
        }

        /// <summary>Translate the transform by a vector.</summary>
        void Translate(const Vector3& translation, Space relativeTo = Space::Self) {
            if (relativeTo == Space::Self) {
                Vector3 worldTranslation = TransformDirection(translation);
                SetPosition(GetPosition() + worldTranslation);
            } else {
                SetPosition(GetPosition() + translation);
            }
        }

        /// <summary>Translate relative to another transform.</summary>
        void Translate(const Vector3& translation, Transform relativeTo) {
            if (relativeTo) {
                Vector3 worldTranslation = relativeTo.TransformDirection(translation);
                SetPosition(GetPosition() + worldTranslation);
            } else {
                SetPosition(GetPosition() + translation);
            }
        }


        [[nodiscard]] Matrix4x4 GetWorldToLocalMatrix() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_worldToLocalMatrix"), 0);
            return MethodHandler::invoke<Matrix4x4>(m, raw());
        }

        [[nodiscard]] Matrix4x4 GetLocalToWorldMatrix() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Transform"), IL2CPP_STR("get_localToWorldMatrix"), 0);
            return MethodHandler::invoke<Matrix4x4>(m, raw());
        }
    };


    inline Transform Component::GetTransform() const {
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("get_transform"), 0);
        return Transform{ MethodHandler::invoke<void*>(m, raw()) };
    }

    inline GameObject Component::GetGameObject() const {
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("get_gameObject"), 0);
        return GameObject{ MethodHandler::invoke<void*>(m, raw()) };
    }

    inline Transform GameObject::GetTransform() const {
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("get_transform"), 0);
        return Transform{ MethodHandler::invoke<void*>(m, raw()) };
    }


    inline Object Object::InstantiateWithParent(Transform parent, bool WorldPositionStays) const {
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Object"), IL2CPP_STR("Internal_CloneSingleWithParent"), 3);
        void* self = raw();
        void* p = parent.raw();
		bool wps = WorldPositionStays;
        void* params[] = { self, p, &wps };
        return Object{ MethodHandler::invoke<void*>(m, nullptr, params) };
    }

} // namespace IL2CPP::Module::Unity
