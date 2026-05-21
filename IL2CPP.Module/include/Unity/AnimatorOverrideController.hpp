#pragma once
#include "RuntimeAnimatorController.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class AnimatorOverrideController : public RuntimeAnimatorController {
    public:
        using RuntimeAnimatorController::RuntimeAnimatorController;

        [[nodiscard]] RuntimeAnimatorController GetRuntimeAnimatorController() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AnimatorOverrideController"), IL2CPP_STR("get_runtimeAnimatorController"), 0);
            return RuntimeAnimatorController{ MethodHandler::invoke<void*>(m, raw()) };
        }
        void SetRuntimeAnimatorController(RuntimeAnimatorController controller) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AnimatorOverrideController"), IL2CPP_STR("set_runtimeAnimatorController"), 1);
            void* r = controller.raw();
            void* params[] = { r };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetOverridesCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AnimatorOverrideController"), IL2CPP_STR("get_overridesCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] AnimationClip GetClipOverride(AnimationClip original) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AnimatorOverrideController"), IL2CPP_STR("get_Item"), 1);
            void* r = original.raw();
            void* params[] = { r };
            return AnimationClip{ MethodHandler::invoke<void*>(m, raw(), params) };
        }
        void SetClipOverride(AnimationClip original, AnimationClip overrideClip) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AnimatorOverrideController"), IL2CPP_STR("set_Item"), 2);
            void* r1 = original.raw();
            void* r2 = overrideClip.raw();
            void* params[] = { r1, r2 };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
