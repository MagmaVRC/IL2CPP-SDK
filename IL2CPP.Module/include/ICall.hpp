#pragma once
#include "ManagedObject.hpp"
#include <atomic>
#include <cstdint>
#include <intrin.h>

namespace IL2CPP::Module {

    /// <summary>A Unity internal call resolved once and called directly, bypassing runtime_invoke.</summary>
    class ICall {
        std::atomic<void*>    m_fn{ nullptr };
        std::atomic<bool>     m_dead{ false };
        std::atomic<uint32_t> m_attempts{ 0 };

    public:
        /// <summary>True once resolution has succeeded or been ruled out for this build.</summary>
        [[nodiscard]] bool ready() const noexcept {
            return m_fn.load(std::memory_order_acquire) || m_dead.load(std::memory_order_relaxed);
        }

        /// <summary>The resolved entry point cast to function-pointer type F, or null when unavailable
        /// or when called off the Unity main thread.</summary>
        template<typename F>
        [[nodiscard]] F as() const noexcept {
            void* fn = m_fn.load(std::memory_order_acquire);
            return fn && on_main_thread() ? reinterpret_cast<F>(fn) : nullptr;
        }

        /// <summary>True on the Unity main thread, where icalls cannot raise a thread-affinity exception.</summary>
        [[nodiscard]] static bool on_main_thread() noexcept {
            static std::atomic<unsigned long> s_main{ 0 };
            // TEB ClientId.UniqueThread: GetCurrentThreadId without pulling windows.h into the header.
            const unsigned long self = __readgsdword(0x48);
            const unsigned long main = s_main.load(std::memory_order_relaxed);
            if (main) return main == self;
            auto* e = GetExports();
            if (!e || !e->m_dispatchIsMainThread
                || !reinterpret_cast<bool(IL2CPP_CALLTYPE)()>(e->m_dispatchIsMainThread)()) return false;
            s_main.store(self, std::memory_order_relaxed);
            return true;
        }

        /// <summary>Resolve the icall. Failures retry, throttled, unless the build rules it out.</summary>
        /// <param name="className">Declaring class, used to check the managed declaration.</param>
        /// <param name="injectedName">Managed _Injected method whose first parameter must be System.IntPtr,
        /// or null for an icall with no native self argument.</param>
        /// <param name="argc">Parameter count of the managed declaration.</param>
        /// <param name="icallName">Fully qualified icall name passed to il2cpp_resolve_icall.</param>
        void resolve(const char* className, const char* injectedName, int argc, const char* icallName) {
            // Only the main thread can use the result (see as<F>()), and the metadata reads
            // below need an attached thread, so an off-main caller just takes the invoke path.
            if (!on_main_thread()) return;
            if ((m_attempts.fetch_add(1, std::memory_order_relaxed) & 0x3FF) != 0) return;
            auto* e = GetExports();
            if (!e || !e->m_resolveFunction || !e->m_gcHandleGetTarget) return;
            if (injectedName) {
                Class klass = Class::find(className);
                if (!klass) return;
                Method m = klass.get_method(injectedName, argc);
                // 2022.3 declares _Injected with the managed object as self (or not at all).
                constexpr int IL2CPP_TYPE_I = 0x18;
                if (!m || m.get_param_type(0).type_enum() != IL2CPP_TYPE_I) {
                    m_dead.store(true, std::memory_order_relaxed);
                    return;
                }
            }
            void* fn = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_resolveFunction)(icallName);
            if (fn) m_fn.store(fn, std::memory_order_release);
        }

        /// <summary>The native object behind a UnityEngine.Object (m_CachedPtr), or null when unreadable.</summary>
        [[nodiscard]] static void* unity_self(void* obj) noexcept {
            if ((reinterpret_cast<uintptr_t>(obj) & 7u) != 0 || !IsValidPointer(obj)) return nullptr;
            void* cached = nullptr;
            if (!ReadPointerGuarded(static_cast<char*>(obj) + g_layoutOffsets.objectCachedPtr, &cached)) return nullptr;
            return IsValidPointer(cached) ? cached : nullptr;
        }

        /// <summary>Turn an icall's marshalled UnityEngine.Object return into the managed object.</summary>
        [[nodiscard]] static void* unmarshal_object(void* handle) noexcept {
            if (!handle) return nullptr;
            if ((reinterpret_cast<uintptr_t>(handle) & 1u) != 0)
                return reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(GetExports()->m_gcHandleGetTarget)(handle);
            return *static_cast<void**>(handle);
        }
    };

} // namespace IL2CPP::Module
