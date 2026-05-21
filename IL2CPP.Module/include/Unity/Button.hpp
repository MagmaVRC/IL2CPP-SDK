#pragma once
#include "Selectable.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class Button : public Selectable {
    public:
        using Selectable::Selectable;


        [[nodiscard]] void* GetOnClickRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Button"), IL2CPP_STR("get_onClick"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
