#include <MethodHandler.hpp>
#include <il2cpp_module.hpp>
#include <string>

namespace IL2CPP::Module {

    Method MethodHandler::resolve(std::string_view className, std::string_view methodName, int argc) {
        auto* e = GetExports();
        if (!e || !e->m_helperMethodResolve) return Method{};

        auto fn = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*, const char*, int)>(
            e->m_helperMethodResolve);

        void* info = fn(
            std::string(className).c_str(),
            std::string(methodName).c_str(),
            argc);

        return Method{ info };
    }

    void* MethodHandler::invoke_raw(const Method& method, void* obj, void** params) {
        void* raw = method.raw();
        if (!raw) return nullptr;
        auto* e = GetExports();  // inline: a single load of the cached exports pointer
        if (!e || !e->m_helperMethodInvoke) return nullptr;
        // One hop to Core (native): it dispatches capability handles, runs runtime_invoke,
        // and formats/handles exceptions — all natively. Doing that work here would run it
        // under the VM (~2x) for no benefit. Keep the VM side to a single marshalled hop.
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, void*, void**)>(
            e->m_helperMethodInvoke)(raw, obj, params);
    }

} // namespace IL2CPP::Module
