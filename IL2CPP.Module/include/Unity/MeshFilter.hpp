#pragma once
#include "Component.hpp"
#include "Mesh.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class MeshFilter : public Component {
    public:
        using Component::Component;

        [[nodiscard]] Mesh GetSharedMesh() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MeshFilter"), IL2CPP_STR("get_sharedMesh"), 0);
            return Mesh{ MethodHandler::invoke<void*>(m, raw()) };
        }
    };

} // namespace IL2CPP::Module::Unity
