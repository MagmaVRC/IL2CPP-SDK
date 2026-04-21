#pragma once
#include <IL2CPP.Common/il2cpp_shared.hpp>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::Unity {

    class Callbacks {
    public:
        Callbacks() = delete;

        /// Initialize the Unity callback system.
        /// Must be called before registering any callbacks.
        static void Initialize() {
            auto* e = GetExports();
            if (!e || !e->m_helperCallbackInitialize) return;
            reinterpret_cast<void(IL2CPP_CALLTYPE)()>(e->m_helperCallbackInitialize)();
        }

        /// Uninitialize the Unity callback system.
        /// Call during module shutdown to clean up hooks.
        static void Uninitialize() {
            auto* e = GetExports();
            if (!e || !e->m_helperCallbackUninitialize) return;
            reinterpret_cast<void(IL2CPP_CALLTYPE)()>(e->m_helperCallbackUninitialize)();
        }

        /// Register a callback function for a Unity event.
        /// @param callback Function pointer to register
        /// @param event The Unity event to hook (Update, LateUpdate, etc.)
        static void Register(void* callback, UnityEvent event = UnityEvent::Update) {
            auto* e = GetExports();
            if (!e || !e->m_helperCallbackRegister || !callback) return;
            reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, uint8_t)>(
                e->m_helperCallbackRegister)(callback, static_cast<uint8_t>(event));
        }

        /// <summary>Register a callback function (template version for function pointers).</summary>
        template <auto Fn>
        static void Register(UnityEvent event = UnityEvent::Update) {
            Register(reinterpret_cast<void*>(Fn), event);
        }

        /// <summary>Register a callback function (template version for lambdas/functors).</summary>
        template <typename Fn>
        static void Register(Fn fn, UnityEvent event = UnityEvent::Update) {
            Register(reinterpret_cast<void*>(+fn), event);
        }

        /// <summary>Register a callback function (explicit function pointer version).</summary>
        template <typename R, typename... Args>
        static void Register(R(*fn)(Args...), UnityEvent event = UnityEvent::Update) {
            Register(reinterpret_cast<void*>(fn), event);
        }
    };

} // namespace IL2CPP::Module::Unity
