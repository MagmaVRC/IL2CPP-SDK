#pragma once
#include "Behaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Camera : public Behaviour {
    public:
        using Behaviour::Behaviour;

        [[nodiscard]] static Camera GetMain() {
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
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_worldToCameraMatrix"), 0);
            return MethodHandler::invoke<Matrix4x4>(m, raw());
        }

        [[nodiscard]] Matrix4x4 GetProjectionMatrix() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("get_projectionMatrix"), 0);
            return MethodHandler::invoke<Matrix4x4>(m, raw());
        }
        void SetProjectionMatrix(const Matrix4x4& matrix) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Camera"), IL2CPP_STR("set_projectionMatrix"), 1);
            Matrix4x4 mat = matrix;
            void* params[] = { &mat };
            MethodHandler::invoke(m, raw(), params);
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

        [[nodiscard]] Matrix4x4 GetViewProjectionMatrix() const {
            Matrix4x4 view = GetWorldToCameraMatrix();
            Matrix4x4 proj = GetProjectionMatrix();
            Matrix4x4 result;
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++) {
                    result[i][j] = 0.f;
                    for (int k = 0; k < 4; k++)
                        result[i][j] += proj[i][k] * view[k][j];
                }
            return result;
        }

    };

} // namespace IL2CPP::Module::Unity
