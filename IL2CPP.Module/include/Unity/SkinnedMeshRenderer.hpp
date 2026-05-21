#pragma once
#include "Renderer.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class SkinnedMeshRenderer : public Renderer {
    public:
        using Renderer::Renderer;

        [[nodiscard]] void* GetSharedMesh() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("get_sharedMesh"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetSharedMesh(void* mesh) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("set_sharedMesh"), 1);
            void* params[] = { mesh };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetRootBone() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("get_rootBone"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetRootBone(void* bone) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("set_rootBone"), 1);
            void* params[] = { bone };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetBones() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("get_bones"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetBones(void* bones) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("set_bones"), 1);
            void* params[] = { bones };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetQuality() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("get_quality"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetQuality(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("set_quality"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetUpdateWhenOffscreen() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("get_updateWhenOffscreen"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetUpdateWhenOffscreen(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("set_updateWhenOffscreen"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void BakeMesh(void* mesh) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("BakeMesh"), 1);
            void* params[] = { mesh };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetBlendShapeWeight(int index) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("GetBlendShapeWeight"), 1);
            void* params[] = { &index };
            return MethodHandler::invoke<float>(m, raw(), params);
        }
        void SetBlendShapeWeight(int index, float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SkinnedMeshRenderer"), IL2CPP_STR("SetBlendShapeWeight"), 2);
            void* params[] = { &index, &value };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
