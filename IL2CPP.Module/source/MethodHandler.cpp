#include <MethodHandler.hpp>
#include <BindMiss.hpp>
#include <il2cpp_module.hpp>
#include <string>

namespace IL2CPP::Module {

    Method MethodHandler::resolve(std::string_view className, std::string_view methodName, int argc) {
        auto* e = GetExports();
        if (!e || !e->m_helperMethodResolve) {
            BindMiss::Record(className, methodName, argc);
            return Method{};
        }

        auto fn = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*, const char*, int)>(
            e->m_helperMethodResolve);

        void* info = fn(
            std::string(className).c_str(),
            std::string(methodName).c_str(),
            argc);

        if (!info) BindMiss::Record(className, methodName, argc);

        return Method{ info };
    }

    bool MethodHandler::invoke_discard(const Method& method, void* obj,
                                       std::initializer_list<void*> args) {
        void* code = method.pointer();
        if (!code) return false;

        // is_static() answers false when the flags export is missing, and guessing wrong
        // here shifts every argument by a register. Refuse rather than mis-call.
        auto* e = GetExports();
        if (!e || !e->m_methodGetFlags) return false;

        // Only a value type wider than a register comes back through a hidden pointer;
        // 1/2/4/8 bytes are returned in RAX with no such argument, and passing one would
        // shift everything along. A reference return reports the pointer's own size and
        // is refused by the same test.
        const Class returns = method.return_type().get_class();
        if (!returns || returns.value_size() <= 8) return false;

        // The hidden first argument every struct-returning function takes. Oversized on
        // purpose: the callee writes its own return type's worth and nothing reads it.
        alignas(16) unsigned char sret[128]{};
        void* mi = method.raw();
        void* const* a = args.begin();

        // A static method has no `this`, so its arguments start one register earlier.
        if (method.is_static()) {
            using F0 = void(IL2CPP_CALLTYPE)(void*, void*);
            using F1 = void(IL2CPP_CALLTYPE)(void*, void*, void*);
            using F2 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*);
            using F3 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*);
            using F4 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*, void*);
            using F5 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*, void*, void*);
            using F6 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*, void*, void*, void*);
            switch (args.size()) {
                case 0: reinterpret_cast<F0>(code)(sret, mi); return true;
                case 1: reinterpret_cast<F1>(code)(sret, a[0], mi); return true;
                case 2: reinterpret_cast<F2>(code)(sret, a[0], a[1], mi); return true;
                case 3: reinterpret_cast<F3>(code)(sret, a[0], a[1], a[2], mi); return true;
                case 4: reinterpret_cast<F4>(code)(sret, a[0], a[1], a[2], a[3], mi); return true;
                case 5: reinterpret_cast<F5>(code)(sret, a[0], a[1], a[2], a[3], a[4], mi); return true;
                case 6: reinterpret_cast<F6>(code)(sret, a[0], a[1], a[2], a[3], a[4], a[5], mi); return true;
                default: return false;
            }
        }

        using M0 = void(IL2CPP_CALLTYPE)(void*, void*, void*);
        using M1 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*);
        using M2 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*);
        using M3 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*, void*);
        using M4 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*, void*, void*);
        using M5 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*, void*, void*, void*);
        using M6 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*, void*, void*, void*, void*);
        switch (args.size()) {
            case 0: reinterpret_cast<M0>(code)(sret, obj, mi); return true;
            case 1: reinterpret_cast<M1>(code)(sret, obj, a[0], mi); return true;
            case 2: reinterpret_cast<M2>(code)(sret, obj, a[0], a[1], mi); return true;
            case 3: reinterpret_cast<M3>(code)(sret, obj, a[0], a[1], a[2], mi); return true;
            case 4: reinterpret_cast<M4>(code)(sret, obj, a[0], a[1], a[2], a[3], mi); return true;
            case 5: reinterpret_cast<M5>(code)(sret, obj, a[0], a[1], a[2], a[3], a[4], mi); return true;
            case 6: reinterpret_cast<M6>(code)(sret, obj, a[0], a[1], a[2], a[3], a[4], a[5], mi); return true;
            default: return false;
        }
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
