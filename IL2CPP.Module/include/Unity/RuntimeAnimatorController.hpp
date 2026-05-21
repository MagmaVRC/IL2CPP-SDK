#pragma once
#include "Object.hpp"
#include "AnimationClip.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class RuntimeAnimatorController : public Object {
    public:
        using Object::Object;

        [[nodiscard]] std::vector<AnimationClip> GetAnimationClips() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RuntimeAnimatorController"), IL2CPP_STR("get_animationClips"), 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            return Object::FromArray<AnimationClip>(arr);
        }
    };

} // namespace IL2CPP::Module::Unity
