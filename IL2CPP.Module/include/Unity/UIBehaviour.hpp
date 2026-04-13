#pragma once
#include "MonoBehaviour.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class UIBehaviour : public MonoBehaviour {
    public:
        using MonoBehaviour::MonoBehaviour;

        [[nodiscard]] bool IsActive() const {
            static auto m = MethodHandler::resolve("UnityEngine.EventSystems.UIBehaviour", "IsActive", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] bool IsDestroyed() const {
            static auto m = MethodHandler::resolve("UnityEngine.EventSystems.UIBehaviour", "IsDestroyed", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
