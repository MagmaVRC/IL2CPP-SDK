#pragma once
#include "MaskableGraphic.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class RawImage : public MaskableGraphic {
    public:
        using MaskableGraphic::MaskableGraphic;

        [[nodiscard]] void* GetTexture() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RawImage"), IL2CPP_STR("get_texture"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetTexture(void* texture) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RawImage"), IL2CPP_STR("set_texture"), 1);
            void* params[] = { texture };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Rect GetUvRect() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RawImage"), IL2CPP_STR("get_uvRect"), 0);
            return MethodHandler::invoke<Rect>(m, raw());
        }
        void SetUvRect(const Rect& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.RawImage"), IL2CPP_STR("set_uvRect"), 1);
            Rect r = value;
            void* params[] = { &r };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
