#pragma once
#include "Renderer.hpp"
#include "../MethodHandler.hpp"
#include "../System/Array.hpp"
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

        [[nodiscard]] float GetWidthMultiplier() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("get_widthMultiplier"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetWidthMultiplier(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_widthMultiplier"), 1);
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

        /// <summary>Extra vertices rounding each corner of the line.</summary>
        void SetNumCornerVertices(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_numCornerVertices"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
        /// <summary>Extra vertices rounding each end of the line.</summary>
        void SetNumCapVertices(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_numCapVertices"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
        /// <summary>UnityEngine.LineTextureMode: 0 Stretch, 1 Tile, 2 DistributePerSegment, 3 RepeatPerSegment.</summary>
        void SetTextureMode(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_textureMode"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
        /// <summary>UnityEngine.LineAlignment: 0 View, 1 TransformZ.</summary>
        void SetAlignment(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.LineRenderer"), IL2CPP_STR("set_alignment"), 1);
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

        /// <summary>A managed Vector3[] for SetPositions, meant to be created once and reused.</summary>
        [[nodiscard]] static System::Array<Vector3> NewPositionArray(uintptr_t count) {
            return System::Array<Vector3>::Create(IL2CPP_STR("UnityEngine.Vector3"), count);
        }

        /// <summary>Uploads all points in one call. Unity copies at most positionCount entries.</summary>
        /// <param name="vector3Array">A managed Vector3[]; the NativeArray and NativeSlice overloads are never taken.</param>
        /// <returns>False when the Vector3[] overload is not found or the array is null.</returns>
        bool SetPositions(void* vector3Array) {
            static Method m{};
            if (!m) {
                constexpr int kTypeSzArray = 0x1D;
                if (Class klass = Class::find(IL2CPP_STR("UnityEngine.LineRenderer")))
                    for (const Method& overload : klass.get_overloads(IL2CPP_STR("SetPositions"), 1, false))
                        if (overload.get_param_type(0).type_enum() == kTypeSzArray) { m = overload; break; }
                if (!m) return false;
            }
            if (!vector3Array) return false;
            void* params[] = { vector3Array };
            MethodHandler::invoke(m, raw(), params);
            return true;
        }
        bool SetPositions(const System::Array<Vector3>& positions) {
            return SetPositions(positions.raw());
        }
    };

} // namespace IL2CPP::Module::Unity
