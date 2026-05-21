#pragma once
#include "Renderer.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class TrailRenderer : public Renderer {
    public:
        using Renderer::Renderer;

        [[nodiscard]] float GetTime() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("get_time"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetTime(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("set_time"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetStartWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("get_startWidth"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetStartWidth(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("set_startWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] float GetEndWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("get_endWidth"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetEndWidth(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("set_endWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Color GetStartColor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("get_startColor"), 0);
            return MethodHandler::invoke<Color>(m, raw());
        }
        void SetStartColor(const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("set_startColor"), 1);
            Color c = value;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] Color GetEndColor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("get_endColor"), 0);
            return MethodHandler::invoke<Color>(m, raw());
        }
        void SetEndColor(const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("set_endColor"), 1);
            Color c = value;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetMinVertexDistance() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("get_minVertexDistance"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetMinVertexDistance(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("set_minVertexDistance"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetAutodestruct() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("get_autodestruct"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetAutodestruct(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("set_autodestruct"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetEmitting() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("get_emitting"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetEmitting(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("set_emitting"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetPositionCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("get_positionCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        void Clear() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.TrailRenderer"), IL2CPP_STR("Clear"), 0);
            MethodHandler::invoke(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
