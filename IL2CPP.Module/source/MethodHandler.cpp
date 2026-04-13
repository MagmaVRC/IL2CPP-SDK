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
        auto* e = GetExports();
        if (!e || !e->m_helperMethodInvoke || !method.raw()) return nullptr;

        auto fn = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, void*, void**)>(
            e->m_helperMethodInvoke);

        return fn(method.raw(), obj, params);
    }

} // namespace IL2CPP::Module
