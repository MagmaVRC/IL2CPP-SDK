#pragma once
#include "Behaviour.hpp"
#include "../MethodHandler.hpp"
#include "../ICall.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <cstdint>

namespace IL2CPP::Module::Unity {

    class Camera : public Behaviour {
    public:
        using Behaviour::Behaviour;

        [[nodiscard]] static Camera GetMain() {
            static ICall ic;
            if (!ic.ready()) ic.resolve(nullptr, nullptr, 0, IL2CPP_STR("UnityEngine.Camera::get_main_Injected()"));
            if (auto fn = ic.as<void*(IL2CPP_CALLTYPE)()>())
                return Camera{ ICall::unmarshal_object(fn()) };
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_main"), 0);
            return Camera{ MethodHandler::invoke<void*>(m, nullptr) };
        }

        [[nodiscard]] static Camera GetCurrent() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_current"), 0);
            return Camera{ MethodHandler::invoke<void*>(m, nullptr) };
        }

        [[nodiscard]] Vector3 WorldToScreenPoint(const Vector3& worldPos) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("WorldToScreenPoint"), 1);
            Vector3 v = worldPos;
            void* params[] = { &v };
            return MethodHandler::invoke<Vector3>(m, raw(), params);
        }

        [[nodiscard]] Matrix4x4 GetWorldToCameraMatrix() const {
            static ICall ic;
            if (!ic.ready()) ic.resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_worldToCameraMatrix_Injected"), 2,
                IL2CPP_STR("UnityEngine.Camera::get_worldToCameraMatrix_Injected(System.IntPtr,UnityEngine.Matrix4x4&)"));
            if (auto fn = ic.as<void(IL2CPP_CALLTYPE)(void*, Matrix4x4*)>())
                if (void* self = ICall::unity_self(raw())) {
                    Matrix4x4 out{};
                    fn(self, &out);
                    return out;
                }
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_worldToCameraMatrix"), 0);
            return MethodHandler::invoke<Matrix4x4>(m, raw());
        }

        [[nodiscard]] Matrix4x4 GetProjectionMatrix() const {
            static ICall ic;
            if (!ic.ready()) ic.resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_projectionMatrix_Injected"), 2,
                IL2CPP_STR("UnityEngine.Camera::get_projectionMatrix_Injected(System.IntPtr,UnityEngine.Matrix4x4&)"));
            if (auto fn = ic.as<void(IL2CPP_CALLTYPE)(void*, Matrix4x4*)>())
                if (void* self = ICall::unity_self(raw())) {
                    Matrix4x4 out{};
                    fn(self, &out);
                    return out;
                }
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_projectionMatrix"), 0);
            return MethodHandler::invoke<Matrix4x4>(m, raw());
        }
        void SetProjectionMatrix(const Matrix4x4& matrix) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_projectionMatrix"), 1);
            Matrix4x4 mat = matrix;
            void* params[] = { &mat };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetPixelWidth() const {
            static ICall ic;
            if (!ic.ready()) ic.resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_pixelWidth_Injected"), 1,
                IL2CPP_STR("UnityEngine.Camera::get_pixelWidth_Injected(System.IntPtr)"));
            if (auto fn = ic.as<int32_t(IL2CPP_CALLTYPE)(void*)>())
                if (void* self = ICall::unity_self(raw())) return fn(self);
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_pixelWidth"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] int GetPixelHeight() const {
            static ICall ic;
            if (!ic.ready()) ic.resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_pixelHeight_Injected"), 1,
                IL2CPP_STR("UnityEngine.Camera::get_pixelHeight_Injected(System.IntPtr)"));
            if (auto fn = ic.as<int32_t(IL2CPP_CALLTYPE)(void*)>())
                if (void* self = ICall::unity_self(raw())) return fn(self);
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_pixelHeight"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] float GetFieldOfView() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_fieldOfView"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetFieldOfView(float fov) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_fieldOfView"), 1);
            void* params[] = { &fov };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetNearClipPlane() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_nearClipPlane"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetNearClipPlane(float nearClip) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_nearClipPlane"), 1);
            void* params[] = { &nearClip };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetFarClipPlane() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_farClipPlane"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetFarClipPlane(float farClip) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_farClipPlane"), 1);
            void* params[] = { &farClip };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetOrthographic() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_orthographic"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetOrthographic(bool ortho) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_orthographic"), 1);
            void* params[] = { &ortho };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetOrthographicSize() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_orthographicSize"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetOrthographicSize(float size) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_orthographicSize"), 1);
            void* params[] = { &size };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetDepth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_depth"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetDepth(float depth) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_depth"), 1);
            void* params[] = { &depth };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetCullingMask() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_cullingMask"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetCullingMask(int mask) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_cullingMask"), 1);
            void* params[] = { &mask };
            MethodHandler::invoke(m, raw(), params);
        }
        void SetCullingMask(LayerMask mask) { SetCullingMask(mask.value()); }

        // UnityEngine.CameraClearFlags: 1=Skybox 2=SolidColor 3=Depth 4=Nothing
        [[nodiscard]] int GetClearFlags() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_clearFlags"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetClearFlags(int flags) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_clearFlags"), 1);
            void* params[] = { &flags };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Rect GetPixelRect() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_pixelRect"), 0);
            return MethodHandler::invoke<Rect>(m, raw());
        }

        [[nodiscard]] Rect GetRect() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_rect"), 0);
            return MethodHandler::invoke<Rect>(m, raw());
        }
        void SetRect(const Rect& rect) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_rect"), 1);
            Rect r = rect;
            void* params[] = { &r };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector3 ScreenToWorldPoint(const Vector3& screenPos) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("ScreenToWorldPoint"), 1);
            Vector3 v = screenPos;
            void* params[] = { &v };
            return MethodHandler::invoke<Vector3>(m, raw(), params);
        }

        [[nodiscard]] Ray ScreenPointToRay(const Vector3& screenPos) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("ScreenPointToRay"), 1);
            Vector3 v = screenPos;
            void* params[] = { &v };
            return MethodHandler::invoke<Ray>(m, raw(), params);
        }

        [[nodiscard]] Vector3 ViewportToWorldPoint(const Vector3& viewportPos) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("ViewportToWorldPoint"), 1);
            Vector3 v = viewportPos;
            void* params[] = { &v };
            return MethodHandler::invoke<Vector3>(m, raw(), params);
        }

        [[nodiscard]] Vector3 WorldToViewportPoint(const Vector3& worldPos) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("WorldToViewportPoint"), 1);
            Vector3 v = worldPos;
            void* params[] = { &v };
            return MethodHandler::invoke<Vector3>(m, raw(), params);
        }

        [[nodiscard]] static int GetAllCamerasCount() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_allCamerasCount"), 0);
            return MethodHandler::invoke<int>(m, nullptr);
        }

        void ResetProjectionMatrix() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("ResetProjectionMatrix"), 0);
            MethodHandler::invoke(m, raw());
        }


        [[nodiscard]] bool IsWorldPointVisible(const Vector3& worldPos, float screenWidth, float screenHeight) const {
            Vector3 screenPoint = WorldToScreenPoint(worldPos);
            return screenPoint.z > 0.f &&
                   screenPoint.x >= 0.f && screenPoint.x <= screenWidth &&
                   screenPoint.y >= 0.f && screenPoint.y <= screenHeight;
        }

        [[nodiscard]] Matrix4x4 GetViewMatrix() const { return GetWorldToCameraMatrix(); }

        /// <summary>Camera state captured once so world points can be projected without a managed call per point.</summary>
        struct ScreenProjector {
            Matrix4x4 view{};
            Matrix4x4 viewProj{};
            float pixelWidth = 0.0f;
            float pixelHeight = 0.0f;
            bool valid = false;

            /// <summary>Projects a world point exactly as Camera.WorldToScreenPoint does.</summary>
            /// <param name="world">Point in world space.</param>
            /// <returns>Pixel x/y with z holding view-space depth; z is at or below zero behind the camera.</returns>
            [[nodiscard]] Vector3 Project(const Vector3& world) const {
                const float depth = -(view[0][2] * world.x + view[1][2] * world.y + view[2][2] * world.z + view[3][2]);
                const float cw = viewProj[0][3] * world.x + viewProj[1][3] * world.y + viewProj[2][3] * world.z + viewProj[3][3];
                if (cw == 0.0f) return Vector3{ 0.0f, 0.0f, depth };
                const float cx = viewProj[0][0] * world.x + viewProj[1][0] * world.y + viewProj[2][0] * world.z + viewProj[3][0];
                const float cy = viewProj[0][1] * world.x + viewProj[1][1] * world.y + viewProj[2][1] * world.z + viewProj[3][1];
                const float inv = 1.0f / cw;
                return Vector3{ (cx * inv * 0.5f + 0.5f) * pixelWidth,
                                (cy * inv * 0.5f + 0.5f) * pixelHeight,
                                depth };
            }
        };

        /// <summary>Captures the camera's view/projection state and viewport size for batched projection.</summary>
        /// <returns>A projector whose valid flag is false when the viewport has no pixels.</returns>
        [[nodiscard]] ScreenProjector GetScreenProjector() const {
            ScreenProjector p{};
            p.view = GetWorldToCameraMatrix();
            const Matrix4x4 proj = GetProjectionMatrix();
            for (int c = 0; c < 4; c++)
                for (int r = 0; r < 4; r++) {
                    float acc = 0.0f;
                    for (int k = 0; k < 4; k++)
                        acc += proj[k][r] * p.view[c][k];
                    p.viewProj[c][r] = acc;
                }
            p.pixelWidth = static_cast<float>(GetPixelWidth());
            p.pixelHeight = static_cast<float>(GetPixelHeight());
            p.valid = p.pixelWidth > 0.0f && p.pixelHeight > 0.0f;
            return p;
        }

        /// <summary>Projection * view, in Unity's column-major storage (m[column][row]).</summary>
        [[nodiscard]] Matrix4x4 GetViewProjectionMatrix() const {
            Matrix4x4 view = GetWorldToCameraMatrix();
            Matrix4x4 proj = GetProjectionMatrix();
            Matrix4x4 result;
            for (int c = 0; c < 4; c++)
                for (int r = 0; r < 4; r++) {
                    result[c][r] = 0.f;
                    for (int k = 0; k < 4; k++)
                        result[c][r] += proj[k][r] * view[c][k];
                }
            return result;
        }

    };

} // namespace IL2CPP::Module::Unity
