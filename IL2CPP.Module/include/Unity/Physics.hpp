#pragma once
#include "../MethodHandler.hpp"
#include "../ICall.hpp"
#include "../System/Array.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

namespace IL2CPP::Module::Unity {

    class Physics {
        [[nodiscard]] static bool DefaultScene(int32_t& out) {
            static std::atomic<bool> s_ok{ false };
            static std::atomic<int32_t> s_handle{ 0 };
            if (s_ok.load(std::memory_order_acquire)) { out = s_handle.load(std::memory_order_relaxed); return true; }
            auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Physics"), IL2CPP_STR("get_defaultPhysicsScene"), 0);
            if (!m) return false;
            void* boxed = MethodHandler::invoke_raw(m, nullptr);
            if (!boxed) return false;
            out = *static_cast<int32_t*>(IL2CPP::Unbox(boxed));
            s_handle.store(out, std::memory_order_relaxed);
            s_ok.store(true, std::memory_order_release);
            return true;
        }

    public:
        Physics() = delete;

        /// <summary>Cast a ray and check for collisions.</summary>
        /// <param name="layerMask">Layers to test; defaults to Unity's DefaultRaycastLayers (Ignore Raycast excluded).</param>
        /// <param name="queryTrigger">0 = UseGlobal, 1 = Collide (hit triggers), 2 = Ignore.</param>
        [[nodiscard]] static bool Raycast(const Vector3& origin, const Vector3& direction, RaycastHit& hit,
                                          float maxDistance = 1e10f, LayerMask layerMask = LayerMask(~(1 << 2)),
                                          int queryTrigger = 2) {
            static ICall ic;
            if (!ic.ready()) ic.resolve(IL2CPP_STR("UnityEngine.PhysicsScene"), nullptr, 6,
                IL2CPP_STR("UnityEngine.PhysicsScene::Internal_Raycast_Injected(UnityEngine.PhysicsScene&,UnityEngine.Ray&,System.Single,UnityEngine.RaycastHit&,System.Int32,UnityEngine.QueryTriggerInteraction)"));
            if (auto fn = ic.as<bool(IL2CPP_CALLTYPE)(int32_t*, Ray*, float, RaycastHit*, int32_t, int32_t)>()) {
                int32_t scene = 0;
                if (DefaultScene(scene)) {
                    hit = RaycastHit{};
                    const float mag = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
                    if (!(mag > std::numeric_limits<float>::denorm_min())) return false;
                    Vector3 dir{ direction.x / mag, direction.y / mag, direction.z / mag };
                    const float unit = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
                    dir = unit > 1e-5f ? Vector3{ dir.x / unit, dir.y / unit, dir.z / unit } : Vector3{ 0.f, 0.f, 0.f };
                    Ray ray{ origin, dir };
                    return fn(&scene, &ray, maxDistance, &hit, layerMask.value(), queryTrigger);
                }
            }
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Physics"), IL2CPP_STR("Raycast"), 6);
            Vector3 o = origin, d = direction;
            float md = maxDistance;
            int mask = layerMask.value();
            int qti = queryTrigger;
            void* params[] = { &o, &d, &hit, &md, &mask, &qti };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        /// <summary>Cast a ray without needing hit info (simple collision check).</summary>
        /// <param name="queryTrigger">0 = UseGlobal, 1 = Collide (hit triggers), 2 = Ignore.</param>
        [[nodiscard]] static bool Raycast(const Vector3& origin, const Vector3& direction,
                                          float maxDistance = 1e10f, LayerMask layerMask = LayerMask(~(1 << 2)),
                                          int queryTrigger = 2) {
            RaycastHit hit;
            return Raycast(origin, direction, hit, maxDistance, layerMask, queryTrigger);
        }

        /// <summary>Cast a ray from a Ray struct.</summary>
        /// <param name="queryTrigger">0 = UseGlobal, 1 = Collide (hit triggers), 2 = Ignore.</param>
        [[nodiscard]] static bool Raycast(const Ray& ray, RaycastHit& hit,
                                          float maxDistance = 1e10f, LayerMask layerMask = LayerMask(~(1 << 2)),
                                          int queryTrigger = 2) {
            return Raycast(ray.origin, ray.direction, hit, maxDistance, layerMask, queryTrigger);
        }

        /// <summary>Cast a ray from a Ray struct (simple collision check).</summary>
        /// <param name="queryTrigger">0 = UseGlobal, 1 = Collide (hit triggers), 2 = Ignore.</param>
        [[nodiscard]] static bool Raycast(const Ray& ray, float maxDistance = 1e10f,
                                          LayerMask layerMask = LayerMask(~(1 << 2)), int queryTrigger = 2) {
            RaycastHit hit;
            return Raycast(ray.origin, ray.direction, hit, maxDistance, layerMask, queryTrigger);
        }

        /// <summary>Cast a ray and return ALL colliders hit along it, nearest first not guaranteed.</summary>
        /// <param name="queryTrigger">0 = UseGlobal, 1 = Collide (hit triggers), 2 = Ignore.</param>
        [[nodiscard]] static std::vector<RaycastHit> RaycastAll(const Vector3& origin, const Vector3& direction,
                                                                float maxDistance = 1e10f, LayerMask layerMask = LayerMask(~(1 << 2)),
                                                                int queryTrigger = 1) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Physics"), IL2CPP_STR("RaycastAll"), 5);
            Vector3 o = origin, d = direction;
            float md = maxDistance;
            int mask = layerMask.value();
            int qti = queryTrigger;
            void* params[] = { &o, &d, &md, &mask, &qti };
            void* arr = MethodHandler::invoke<void*>(m, nullptr, params);
            return System::Array<RaycastHit>(arr).to_vector();
        }

        /// <summary>Cast a ray (Ray struct) and return ALL colliders hit along it.</summary>
        /// <param name="queryTrigger">0 = UseGlobal, 1 = Collide (hit triggers), 2 = Ignore.</param>
        [[nodiscard]] static std::vector<RaycastHit> RaycastAll(const Ray& ray, float maxDistance = 1e10f,
                                                                LayerMask layerMask = LayerMask(~(1 << 2)), int queryTrigger = 1) {
            return RaycastAll(ray.origin, ray.direction, maxDistance, layerMask, queryTrigger);
        }

        /// <summary>Cast a sphere along a ray and check for collisions.</summary>
        /// <param name="layerMask">Layers to test; defaults to Unity's DefaultRaycastLayers (Ignore Raycast excluded).</param>
        /// <param name="queryTrigger">0 = UseGlobal, 1 = Collide (hit triggers), 2 = Ignore.</param>
        [[nodiscard]] static bool SphereCast(const Vector3& origin, float radius, const Vector3& direction,
                                             RaycastHit& hit, float maxDistance = 1e10f,
                                             LayerMask layerMask = LayerMask(~(1 << 2)), int queryTrigger = 2) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Physics"), IL2CPP_STR("SphereCast"), 7);
            Vector3 o = origin, d = direction;
            float r = radius, md = maxDistance;
            int mask = layerMask.value();
            int qti = queryTrigger;
            void* params[] = { &o, &r, &d, &hit, &md, &mask, &qti };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        /// <summary>Cast a sphere along a ray (simple collision check).</summary>
        /// <param name="queryTrigger">0 = UseGlobal, 1 = Collide (hit triggers), 2 = Ignore.</param>
        [[nodiscard]] static bool SphereCast(const Vector3& origin, float radius, const Vector3& direction,
                                             float maxDistance = 1e10f, LayerMask layerMask = LayerMask(~(1 << 2)),
                                             int queryTrigger = 2) {
            RaycastHit hit;
            return SphereCast(origin, radius, direction, hit, maxDistance, layerMask, queryTrigger);
        }

        /// <summary>Cast a sphere from a Ray struct.</summary>
        /// <param name="queryTrigger">0 = UseGlobal, 1 = Collide (hit triggers), 2 = Ignore.</param>
        [[nodiscard]] static bool SphereCast(const Ray& ray, float radius, RaycastHit& hit,
                                             float maxDistance = 1e10f, LayerMask layerMask = LayerMask(~(1 << 2)),
                                             int queryTrigger = 2) {
            return SphereCast(ray.origin, radius, ray.direction, hit, maxDistance, layerMask, queryTrigger);
        }


        /// <summary>Set whether the physics engine ignores contacts between two layers.</summary>
        static void IgnoreLayerCollision(int layer1, int layer2, bool ignore = true) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Physics"), IL2CPP_STR("IgnoreLayerCollision"), 3);
            int a = layer1, b = layer2;
            bool i = ignore;
            void* params[] = { &a, &b, &i };
            MethodHandler::invoke<void>(m, nullptr, params);
        }

        /// <summary>Whether the physics engine currently ignores contacts between two layers.</summary>
        [[nodiscard]] static bool GetIgnoreLayerCollision(int layer1, int layer2) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Physics"), IL2CPP_STR("GetIgnoreLayerCollision"), 2);
            int a = layer1, b = layer2;
            void* params[] = { &a, &b };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static Ray CreateRayToward(const Vector3& from, const Vector3& to) {
            Vector3 dir = to - from;
            dir.NormalizeInPlace();
            return Ray(from, dir);
        }

        [[nodiscard]] static bool LineOfSight(const Vector3& from, const Vector3& to,
                                              LayerMask layerMask = LayerMask(~(1 << 2))) {
            Vector3 dir = to - from;
            float dist = dir.Magnitude();
            if (dist < 0.0001f) return true;
            dir = dir / dist;
            RaycastHit hit;
            return !Raycast(from, dir, hit, dist, layerMask);
        }
    };

} // namespace IL2CPP::Module::Unity
