#pragma once
#include "../ManagedObject.hpp"
#include "../Reflection.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <IL2CPP.Common/il2cpp_structs.hpp>

namespace IL2CPP::Module::System {

    class Delegate : public ManagedObject {
    public:
        static constexpr int kMethodPtrOffset   = 0x10;
        static constexpr int kInvokeImplOffset  = 0x18;
        static constexpr int kTargetOffset      = 0x20;
        static constexpr int kMethodInfoOffset  = 0x28;
        static constexpr int kExtraArgOffset    = 0x38;

        using ManagedObject::ManagedObject;


        [[nodiscard]] void* GetTarget() const {
            static auto m = MethodHandler::resolve("System.Delegate", "get_Target", 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        [[nodiscard]] void* GetMethodPtr() const {
            if (!valid()) return nullptr;
            return read<void*>(kMethodPtrOffset);
        }

        [[nodiscard]] void* GetInvokeImpl() const {
            if (!valid()) return nullptr;
            return read<void*>(kInvokeImplOffset);
        }


        static Delegate Combine(Delegate a, Delegate b) {
            static auto m = MethodHandler::resolve("System.Delegate", "Combine", 2);
            void* ra = a.raw();
            void* rb = b.raw();
            void* params[] = { ra, rb };
            return Delegate{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        static Delegate Remove(Delegate source, Delegate value) {
            static auto m = MethodHandler::resolve("System.Delegate", "Remove", 2);
            void* rs = source.raw();
            void* rv = value.raw();
            void* params[] = { rs, rv };
            return Delegate{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        void* DynamicInvoke(void** args = nullptr) {
            static auto m = MethodHandler::resolve("System.Delegate", "DynamicInvoke", 1);
            void* params[] = { args };
            return MethodHandler::invoke<void*>(m, raw(), params);
        }


        [[nodiscard]] static Delegate CreateForStaticMethod(std::string_view delegateTypeName, void* targetMethodInfo) {
            auto* e = GetExports();
            if (!e || !targetMethodInfo) return Delegate{};

            void* delegateClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)(
                std::string(delegateTypeName).c_str());
            if (!delegateClass) return Delegate{};

            void* delegateObj = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(e->m_objectNew)(delegateClass);
            if (!delegateObj) return Delegate{};

            auto* mi = static_cast<il2cppMethodInfo*>(targetMethodInfo);
            void* fnPtr = mi->m_pMethodPointer;

            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kMethodPtrOffset)  = fnPtr;
            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kInvokeImplOffset) = fnPtr;
            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kTargetOffset)     = nullptr;
            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kMethodInfoOffset) = targetMethodInfo;

            return Delegate{ delegateObj };
        }

        [[nodiscard]] static Delegate CreateForInstanceMethod(std::string_view delegateTypeName, void* target, void* targetMethodInfo) {
            auto* e = GetExports();
            if (!e || !target || !targetMethodInfo) return Delegate{};

            void* delegateClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)(
                std::string(delegateTypeName).c_str());
            if (!delegateClass) return Delegate{};

            void* delegateObj = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(e->m_objectNew)(delegateClass);
            if (!delegateObj) return Delegate{};

            auto* mi = static_cast<il2cppMethodInfo*>(targetMethodInfo);
            void* fnPtr = mi->m_pMethodPointer;

            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kMethodPtrOffset)  = fnPtr;
            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kInvokeImplOffset) = fnPtr;
            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kTargetOffset)     = target;
            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kMethodInfoOffset) = targetMethodInfo;

            return Delegate{ delegateObj };
        }


        /// Create a delegate that calls a native C++ function.
        ///
        /// Your function MUST use the IL2CPP calling convention:
        ///   void(__fastcall*)(void* target, <args...>, void* method)
        ///
        /// Example — hooking a Unity button click:
        ///
        ///   void __fastcall OnClick(void* target, void* method) {
        ///       // handle click
        ///   }
        ///
        ///   auto action = Action::CreateNative("System.Action", &OnClick);
        ///   // button.onClick.AddListener(action)  via raw invoke
        ///
        /// Example — Action<int>:
        ///
        ///   void __fastcall OnValueChanged(void* target, int value, void* method) {
        ///       // handle value
        ///   }
        ///
        ///   auto action = Delegate::CreateNative("System.Action`1", &OnValueChanged);
        ///
        /// @param delegateTypeName  Managed delegate type (e.g. "System.Action")
        /// @param nativeFn          Pointer to your C++ function
        /// @param context           Optional context pointer passed as 'target' (default: nullptr)
        [[nodiscard]] static Delegate CreateNative(std::string_view delegateTypeName, void* nativeFn, void* context = nullptr) {
            auto* e = GetExports();
            if (!e || !nativeFn) return Delegate{};

            void* delegateClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)(
                std::string(delegateTypeName).c_str());
            if (!delegateClass) return Delegate{};

            void* delegateObj = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(e->m_objectNew)(delegateClass);
            if (!delegateObj) return Delegate{};

            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kMethodPtrOffset)  = nativeFn;
            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kInvokeImplOffset) = nativeFn;
            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kTargetOffset)     = context;
            *reinterpret_cast<void**>(static_cast<char*>(delegateObj) + kMethodInfoOffset) = nullptr;

            return Delegate{ delegateObj };
        }

        /// Type-safe overload — pass any function pointer type directly.
        /// Casts to void* internally.
        template<typename Fn>
        [[nodiscard]] static Delegate CreateNative(std::string_view delegateTypeName, Fn fn, void* context = nullptr) {
            return CreateNative(delegateTypeName, reinterpret_cast<void*>(fn), context);
        }
    };


    class Action : public Delegate {
    public:
        using Delegate::Delegate;

        void Invoke() {
            static auto m = MethodHandler::resolve("System.Action", "Invoke", 0);
            MethodHandler::invoke(m, raw());
        }

        /// Directly call the underlying function pointer (faster than managed Invoke).
        void InvokeDirect() {
            if (!valid()) return;
            auto fn = reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void*)>(GetInvokeImpl());
            if (fn) fn(GetTarget(), nullptr);
        }

        /// Create an Action from a native C++ function.
        /// Signature: void(__fastcall*)(void* target, void* method)
        [[nodiscard]] static Action CreateNative(void* nativeFn, void* context = nullptr) {
            Delegate d = Delegate::CreateNative("System.Action", nativeFn, context);
            return Action{ d.raw() };
        }

        template<typename Fn>
        [[nodiscard]] static Action CreateNative(Fn fn, void* context = nullptr) {
            return CreateNative(reinterpret_cast<void*>(fn), context);
        }
    };


    template<typename T>
    class Action1 : public Delegate {
    public:
        using Delegate::Delegate;

        /// Directly call the underlying function pointer.
        /// Signature: void(__fastcall*)(void* target, T arg, void* method)
        void InvokeDirect(T arg) {
            if (!valid()) return;
            auto fn = reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, T, void*)>(GetInvokeImpl());
            if (fn) fn(GetTarget(), arg, nullptr);
        }

        /// Create from a native C++ function.
        /// Your fn signature: void(__fastcall*)(void* target, T arg, void* method)
        template<typename Fn>
        [[nodiscard]] static Action1 CreateNative(Fn fn, void* context = nullptr) {
            Delegate d = Delegate::CreateNative("System.Action`1", reinterpret_cast<void*>(fn), context);
            return Action1{ d.raw() };
        }
    };


    template<typename T1, typename T2>
    class Action2 : public Delegate {
    public:
        using Delegate::Delegate;

        void InvokeDirect(T1 a1, T2 a2) {
            if (!valid()) return;
            auto fn = reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, T1, T2, void*)>(GetInvokeImpl());
            if (fn) fn(GetTarget(), a1, a2, nullptr);
        }

        template<typename Fn>
        [[nodiscard]] static Action2 CreateNative(Fn fn, void* context = nullptr) {
            Delegate d = Delegate::CreateNative("System.Action`2", reinterpret_cast<void*>(fn), context);
            return Action2{ d.raw() };
        }
    };


    template<typename TResult>
    class Func : public Delegate {
    public:
        using Delegate::Delegate;

        /// Signature: TResult(__fastcall*)(void* target, void* method)
        TResult InvokeDirect() {
            if (!valid()) return TResult{};
            auto fn = reinterpret_cast<TResult(IL2CPP_CALLTYPE)(void*, void*)>(GetInvokeImpl());
            if (!fn) return TResult{};
            return fn(GetTarget(), nullptr);
        }

        template<typename Fn>
        [[nodiscard]] static Func CreateNative(Fn fn, void* context = nullptr) {
            Delegate d = Delegate::CreateNative("System.Func`1", reinterpret_cast<void*>(fn), context);
            return Func{ d.raw() };
        }
    };


    template<typename T, typename TResult>
    class Func1 : public Delegate {
    public:
        using Delegate::Delegate;

        /// Signature: TResult(__fastcall*)(void* target, T arg, void* method)
        TResult InvokeDirect(T arg) {
            if (!valid()) return TResult{};
            auto fn = reinterpret_cast<TResult(IL2CPP_CALLTYPE)(void*, T, void*)>(GetInvokeImpl());
            if (!fn) return TResult{};
            return fn(GetTarget(), arg, nullptr);
        }

        template<typename Fn>
        [[nodiscard]] static Func1 CreateNative(Fn fn, void* context = nullptr) {
            Delegate d = Delegate::CreateNative("System.Func`2", reinterpret_cast<void*>(fn), context);
            return Func1{ d.raw() };
        }
    };

} // namespace IL2CPP::Module::System
