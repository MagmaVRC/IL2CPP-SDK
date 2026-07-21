#pragma once
#include "../ManagedObject.hpp"
#include "../MethodHandler.hpp"
#include "../System/Delegate.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <type_traits>

// Mirrors C# UnityEngine.Events. Kept in a nested namespace so the wrapper class
// `UnityEvent` does not shadow the pre-existing IL2CPP::UnityEvent lifecycle enum
// used by the callback system.
namespace IL2CPP::Module::Unity::Events {

    /// UnityEngine.Events.UnityAction — the void() delegate Unity events listen with.
    /// Derives from System::Delegate, so a UnityAction is implicitly usable anywhere a
    /// System::Delegate is expected (up-cast). Down-casting a System::Delegate to a
    /// UnityAction is explicit (you are asserting the managed type).
    class UnityAction : public System::Delegate {
    public:
        using System::Delegate::Delegate;
        UnityAction() = default;

        /// <summary>Explicit cast: reinterpret any delegate as a UnityAction (same object pointer).</summary>
        explicit UnityAction(const System::Delegate& d) : System::Delegate(d.raw()) {}

        /// Create a UnityAction that invokes a native C++ function.
        /// Signature: void(__fastcall*)(void* target, void* method)
        template<typename Fn>
        [[nodiscard]] static UnityAction CreateNative(Fn fn, void* context = nullptr) {
            System::Delegate d = System::Delegate::CreateNative(
                IL2CPP_STR("UnityEngine.Events.UnityAction"), reinterpret_cast<void*>(fn), context);
            return UnityAction{ d.raw() };
        }

        void InvokeDirect() {
            if (!valid()) return;
            auto fn = reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void*)>(GetInvokeImpl());
            if (fn) fn(GetTarget(), nullptr);
        }
    };

    /// UnityEngine.Events.UnityAction`1<T> — the void(T) listener delegate.
    template<typename T>
    class UnityAction1 : public System::Delegate {
    public:
        using System::Delegate::Delegate;
        UnityAction1() = default;

        explicit UnityAction1(const System::Delegate& d) : System::Delegate(d.raw()) {}

        /// Signature: void(__fastcall*)(void* target, T arg, void* method)
        template<typename Fn>
        [[nodiscard]] static UnityAction1 CreateNative(Fn fn, void* context = nullptr) {
            System::Delegate d = System::Delegate::CreateNative(
                IL2CPP_STR("UnityEngine.Events.UnityAction`1"), reinterpret_cast<void*>(fn), context);
            return UnityAction1{ d.raw() };
        }

        void InvokeDirect(T arg) {
            if (!valid()) return;
            auto fn = reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, T, void*)>(GetInvokeImpl());
            if (fn) fn(GetTarget(), arg, nullptr);
        }
    };


    /// UnityEngine.Events.UnityEvent — the parameterless event on things like Button.onClick.
    /// Wrap the raw pointer from get_onClick / get_onValueChanged etc.
    class UnityEvent : public ManagedObject {
    public:
        using ManagedObject::ManagedObject;

        /// <summary>Add a listener (any delegate; a UnityAction is the expected type).</summary>
        void AddListener(const System::Delegate& call) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Events.UnityEvent"), IL2CPP_STR("AddListener"), 1);
            void* p[] = { call.raw() };
            MethodHandler::invoke(m, raw(), p);
        }

        void RemoveListener(const System::Delegate& call) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Events.UnityEvent"), IL2CPP_STR("RemoveListener"), 1);
            void* p[] = { call.raw() };
            MethodHandler::invoke(m, raw(), p);
        }

        void Invoke() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Events.UnityEvent"), IL2CPP_STR("Invoke"), 0);
            MethodHandler::invoke(m, raw());
        }

        void RemoveAllListeners() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Events.UnityEvent"), IL2CPP_STR("RemoveAllListeners"), 0);
            MethodHandler::invoke(m, raw());
        }

        /// Wire a native C++ function straight to the event. Returns the created UnityAction
        /// so you can RemoveListener it later. The event keeps it alive while both are reachable.
        template<typename Fn>
        UnityAction AddNativeListener(Fn fn, void* context = nullptr) {
            UnityAction a = UnityAction::CreateNative(fn, context);
            if (a) AddListener(a);
            return a;
        }
    };

    /// UnityEngine.Events.UnityEvent`1<T> — the one-argument event (Toggle bool, Slider float, ...).
    template<typename T>
    class UnityEvent1 : public ManagedObject {
    public:
        using ManagedObject::ManagedObject;

        void AddListener(const System::Delegate& call) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Events.UnityEvent`1"), IL2CPP_STR("AddListener"), 1);
            void* p[] = { call.raw() };
            MethodHandler::invoke(m, raw(), p);
        }

        void RemoveListener(const System::Delegate& call) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Events.UnityEvent`1"), IL2CPP_STR("RemoveListener"), 1);
            void* p[] = { call.raw() };
            MethodHandler::invoke(m, raw(), p);
        }

        void Invoke(T arg) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Events.UnityEvent`1"), IL2CPP_STR("Invoke"), 1);
            void* p[1];
            if constexpr (std::is_pointer_v<T>) p[0] = reinterpret_cast<void*>(arg);
            else p[0] = &arg;
            MethodHandler::invoke(m, raw(), p);
        }

        void RemoveAllListeners() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Events.UnityEvent`1"), IL2CPP_STR("RemoveAllListeners"), 0);
            MethodHandler::invoke(m, raw());
        }

        template<typename Fn>
        UnityAction1<T> AddNativeListener(Fn fn, void* context = nullptr) {
            UnityAction1<T> a = UnityAction1<T>::CreateNative(fn, context);
            if (a) AddListener(a);
            return a;
        }
    };

} // namespace IL2CPP::Module::Unity::Events
