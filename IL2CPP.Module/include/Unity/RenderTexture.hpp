#pragma once
#include "Object.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class RenderTexture : public Object {
    public:
        using Object::Object;

        static RenderTexture Create(int width, int height, int depth = 24) {
            auto klass = Class::find(IL2CPP_STR("UnityEngine.RenderTexture"));
            if (!klass) return {};
            auto obj = klass.new_object();
            if (!obj) return {};

            static auto ctor = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RenderTexture"), IL2CPP_STR(".ctor"), 3);
            if (!ctor) return {};
            void* params[] = { &width, &height, &depth };
            MethodHandler::invoke(ctor, obj.raw(), params);
            return RenderTexture{obj.raw()};
        }

        void Release() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RenderTexture"), IL2CPP_STR("Release"), 0);
            if (m) MethodHandler::invoke(m, raw());
        }

        [[nodiscard]] int GetWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RenderTexture"), IL2CPP_STR("get_width"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] int GetHeight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RenderTexture"), IL2CPP_STR("get_height"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        static void* GetActive() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RenderTexture"), IL2CPP_STR("get_active"), 0);
            return MethodHandler::invoke<void*>(m, nullptr);
        }

        static void SetActive(void* rt) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.RenderTexture"), IL2CPP_STR("set_active"), 1);
            void* params[] = { rt };
            MethodHandler::invoke(m, nullptr, params);
        }
    };

} // namespace IL2CPP::Module::Unity
