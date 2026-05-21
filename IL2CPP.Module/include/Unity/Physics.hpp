#pragma once
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Physics {
    public:
        Physics() = delete;

        /// <summary>Cast a ray and check for collisions.</summary>
        [[nodiscard]] static bool Raycast(const Vector3& origin, const Vector3& direction, RaycastHit& hit,
                                          float maxDistance = 1e10f, LayerMask layerMask = LayerMask(-1)) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Physics"), IL2CPP_STR("Raycast"), 5);
            Vector3 o = origin, d = direction;
            float md = maxDistance;
            int mask = layerMask.value();
            void* params[] = { &o, &d, &hit, &md, &mask };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        /// <summary>Cast a ray without needing hit info (simple collision check).</summary>
        [[nodiscard]] static bool Raycast(const Vector3& origin, const Vector3& direction,
                                          float maxDistance = 1e10f, LayerMask layerMask = LayerMask(-1)) {
            RaycastHit hit;
            return Raycast(origin, direction, hit, maxDistance, layerMask);
        }

        /// <summary>Cast a ray from a Ray struct.</summary>
        [[nodiscard]] static bool Raycast(const Ray& ray, RaycastHit& hit,
                                          float maxDistance = 1e10f, LayerMask layerMask = LayerMask(-1)) {
            return Raycast(ray.origin, ray.direction, hit, maxDistance, layerMask);
        }

        /// <summary>Cast a ray from a Ray struct (simple collision check).</summary>
        [[nodiscard]] static bool Raycast(const Ray& ray, float maxDistance = 1e10f, LayerMask layerMask = LayerMask(-1)) {
            RaycastHit hit;
            return Raycast(ray.origin, ray.direction, hit, maxDistance, layerMask);
        }

        /// <summary>Cast a sphere along a ray and check for collisions.</summary>
        [[nodiscard]] static bool SphereCast(const Vector3& origin, float radius, const Vector3& direction,
                                             RaycastHit& hit, float maxDistance = 1e10f, LayerMask layerMask = LayerMask(-1)) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Physics"), IL2CPP_STR("SphereCast"), 6);
            Vector3 o = origin, d = direction;
            float r = radius, md = maxDistance;
            int mask = layerMask.value();
            void* params[] = { &o, &r, &d, &hit, &md, &mask };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        /// <summary>Cast a sphere along a ray (simple collision check).</summary>
        [[nodiscard]] static bool SphereCast(const Vector3& origin, float radius, const Vector3& direction,
                                             float maxDistance = 1e10f, LayerMask layerMask = LayerMask(-1)) {
            RaycastHit hit;
            return SphereCast(origin, radius, direction, hit, maxDistance, layerMask);
        }

        /// <summary>Cast a sphere from a Ray struct.</summary>
        [[nodiscard]] static bool SphereCast(const Ray& ray, float radius, RaycastHit& hit,
                                             float maxDistance = 1e10f, LayerMask layerMask = LayerMask(-1)) {
            return SphereCast(ray.origin, radius, ray.direction, hit, maxDistance, layerMask);
        }


        [[nodiscard]] static Ray CreateRayToward(const Vector3& from, const Vector3& to) {
            Vector3 dir = to - from;
            dir.NormalizeInPlace();
            return Ray(from, dir);
        }

        [[nodiscard]] static bool LineOfSight(const Vector3& from, const Vector3& to, LayerMask layerMask = LayerMask(-1)) {
            Vector3 dir = to - from;
            float dist = dir.Magnitude();
            if (dist < 0.0001f) return true;
            dir = dir / dist;
            RaycastHit hit;
            return !Raycast(from, dir, hit, dist, layerMask);
        }
    };

} // namespace IL2CPP::Module::Unity
