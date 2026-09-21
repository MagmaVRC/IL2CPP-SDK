#pragma once
#include "Reflection.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <IL2CPP.Common/il2cpp_structs.hpp>
#include <initializer_list>
#include <string_view>
#include <type_traits>

namespace IL2CPP::Module {

    class MethodHandler {
    public:
        MethodHandler() = delete;

        /// Resolve a method by class name, method name, and argument count.
        /// Core caches the result; subsequent calls with the same key are fast lookups.
        /// @param className  Full class name (e.g. "UnityEngine.Canvas")
        /// @param methodName Method name (e.g. "get_renderMode")
        /// @param argc       Expected parameter count (-1 = any)
        /// @return A Method handle wrapping the il2cppMethodInfo*, or invalid Method on failure.
        [[nodiscard]] static Method resolve(
            std::string_view className, std::string_view methodName, int argc = -1);

        /// Invoke a method whose return value is a value type, discarding the result.
        ///
        /// runtime_invoke boxes a value-type return through il2cpp's generated invoker,
        /// and for an inflated generic struct -- UniTask`1 being the one that bites --
        /// that boxing copies with a garbage size and faults. This calls the method's own
        /// code with the hidden return-buffer argument the ABI gives it, then throws the
        /// result away.
        ///
        /// @param obj   Instance, or nullptr for a static method (decided by the method,
        ///              not by this argument).
        /// @param args  Arguments as the native ABI wants them, NOT as runtime_invoke wants
        ///              them: a pointer for a reference type, and the value ITSELF widened
        ///              to a pointer for a value type -- `(void*)1` for a true Boolean, not
        ///              its address. Floating-point arguments are not supported: they travel
        ///              in XMM registers by position. Up to six.
        /// @return False when the method carries no code, takes more arguments than this can
        ///         place, or does not return a value type wider than a register -- anything
        ///         else has no hidden return argument, and passing one would shift every
        ///         argument along.
        static bool invoke_discard(const Method& method, void* obj,
                                   std::initializer_list<void*> args = {});

        /// Invoke a resolved method safely through Core's runtime_invoke.
        /// Core catches exceptions internally and returns nullptr on failure.
        /// @return Raw il2cppObject* result (boxed for value types), or nullptr.
        static void* invoke_raw(const Method& method, void* obj, void** params = nullptr);

        /// Typed invoke convenience wrapper.
        /// - void return: just calls invoke_raw
        /// - Pointer return: reinterpret_cast from result
        /// - Value return: unboxes the result
        /// Returns TReturn{} (default) on null result.
        template<typename TReturn = void>
        static TReturn invoke(const Method& method, void* obj, void** params = nullptr) {
            if constexpr (std::is_void_v<TReturn>) {
                invoke_raw(method, obj, params);
            } else {
                void* result = invoke_raw(method, obj, params);
                if (!result) return TReturn{};
                if constexpr (std::is_pointer_v<TReturn>)
                    return reinterpret_cast<TReturn>(result);
                else
                    return *reinterpret_cast<TReturn*>(IL2CPP::Unbox(result));
            }
        }
    };

} // namespace IL2CPP::Module
