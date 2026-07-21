#pragma once
#include "../MethodHandler.hpp"
#include "../Reflection.hpp"

namespace IL2CPP::Module::Unity {

    class GraphicsBuffer {
        void* m_native = nullptr;
    public:
        // UnityEngine.GraphicsBuffer.Target (flags).
        enum class Target {
            Vertex            = 1,
            Index             = 2,
            CopySource        = 4,
            CopyDestination   = 8,
            Structured        = 16,
            Raw               = 32,
            Append            = 64,
            Counter           = 128,
            Constant          = 256,
            IndirectArguments = 512,
        };

        // UnityEngine.GraphicsBuffer.UsageFlags.
        enum class Usage {
            None             = 0,
            LockBufferForWrite = 1,
        };

        explicit GraphicsBuffer(void* raw = nullptr) : m_native(raw) {}

        [[nodiscard]] void* raw() const noexcept { return m_native; }
        [[nodiscard]] operator bool() const noexcept { return m_native != nullptr; }

        /// <summary>Create a GraphicsBuffer. @param stride element size in bytes.</summary>
        [[nodiscard]] static GraphicsBuffer New(Target target, int count, int stride) {
            Class klass = Class::find(IL2CPP_STR("UnityEngine.GraphicsBuffer"));
            if (!klass) return GraphicsBuffer{};
            int t = static_cast<int>(target);
            void* params[] = { &t, &count, &stride };
            return GraphicsBuffer{ klass.new_instance(params, 3).raw() };
        }

        /// <summary>Create a GraphicsBuffer with explicit usage flags (4-arg ctor).</summary>
        [[nodiscard]] static GraphicsBuffer New(Target target, Usage usage, int count, int stride) {
            Class klass = Class::find(IL2CPP_STR("UnityEngine.GraphicsBuffer"));
            if (!klass) return GraphicsBuffer{};
            int t = static_cast<int>(target), u = static_cast<int>(usage);
            void* params[] = { &t, &u, &count, &stride };
            return GraphicsBuffer{ klass.new_instance(params, 4).raw() };
        }

        [[nodiscard]] int GetCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GraphicsBuffer"), IL2CPP_STR("get_count"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] int GetStride() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GraphicsBuffer"), IL2CPP_STR("get_stride"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] bool IsValid() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GraphicsBuffer"), IL2CPP_STR("IsValid"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        // D3D11: returns the underlying ID3D11Buffer*.
        [[nodiscard]] void* GetNativeBufferPtr() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GraphicsBuffer"), IL2CPP_STR("GetNativeBufferPtr"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        void Release() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GraphicsBuffer"), IL2CPP_STR("Release"), 0);
            MethodHandler::invoke(m, raw());
        }

        void Dispose() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GraphicsBuffer"), IL2CPP_STR("Dispose"), 0);
            MethodHandler::invoke(m, raw());
        }

        // ponytail: SetData/GetData are generic methods (SetData<T>) — resolve-by-name returns the
        // open definition and needs generic inflation to invoke. Add once the resolver inflates
        // generic instance methods; until then upload via GetNativeBufferPtr() (D3D) or a compute shader.
    };

} // namespace IL2CPP::Module::Unity
