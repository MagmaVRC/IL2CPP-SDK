#pragma once
#include "Behaviour.hpp"
#include "AnimationClip.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    /// <summary>UnityEngine.AnimationState, a TrackedReference rather than a UnityEngine.Object.</summary>
    class AnimationState : public ManagedObject {
    public:
        using ManagedObject::ManagedObject;

        [[nodiscard]] AnimationClip GetClip() const {
            static Method m;
            if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AnimationState"), IL2CPP_STR("get_clip"), 0);
            return m ? AnimationClip{ MethodHandler::invoke<void*>(m, raw()) } : AnimationClip{};
        }
    };

    /// <summary>The legacy UnityEngine.Animation component.</summary>
    class Animation : public Behaviour {
    public:
        using Behaviour::Behaviour;

        [[nodiscard]] AnimationClip GetClip() const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("get_clip"), 0)) return {};
            return AnimationClip{ MethodHandler::invoke<void*>(m, raw()) };
        }

        [[nodiscard]] int GetStateCount() const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("GetStateCount"), 0)) return 0;
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] AnimationState GetStateAtIndex(int index) const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("GetStateAtIndex"), 1)) return {};
            void* params[] = { &index };
            return AnimationState{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

    private:
        static bool Lazy(Method& m, const char* name, int argc) {
            if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animation"), name, argc);
            return static_cast<bool>(m);
        }
    };

} // namespace IL2CPP::Module::Unity
