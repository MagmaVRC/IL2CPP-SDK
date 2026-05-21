#pragma once
#include "Renderer.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class LineRenderer : public Renderer {
    public:
        using Renderer::Renderer;

        [[nodiscard]] int GetPositionCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("get_positionCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetPositionCount(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_positionCount"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetStartWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("get_startWidth"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetStartWidth(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_startWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] float GetEndWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("get_endWidth"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetEndWidth(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_endWidth"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Color GetStartColor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("get_startColor"), 0);
            return MethodHandler::invoke<Color>(m, raw());
        }
        void SetStartColor(const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_startColor"), 1);
            Color c = value;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] Color GetEndColor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("get_endColor"), 0);
            return MethodHandler::invoke<Color>(m, raw());
        }
        void SetEndColor(const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_endColor"), 1);
            Color c = value;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetLoop() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("get_loop"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetLoop(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_loop"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetUseWorldSpace() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("get_useWorldSpace"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetUseWorldSpace(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_useWorldSpace"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetPosition(int index, const Vector3& position) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("SetPosition"), 2);
            Vector3 pos = position;
            void* params[] = { &index, &pos };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector3 GetPosition(int index) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("GetPosition"), 1);
            void* params[] = { &index };
            return MethodHandler::invoke<Vector3>(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
