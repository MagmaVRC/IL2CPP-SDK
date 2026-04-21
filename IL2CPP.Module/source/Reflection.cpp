#ifndef UNITY_VERSION_2022_3_8_HIGHER
#define UNITY_VERSION_2022_3_8_HIGHER
#endif

#include <Reflection.hpp>
#include <ManagedObject.hpp>
#include <Deobfuscation.hpp>
#include <il2cpp_module.hpp>
#include <cstring>

namespace IL2CPP::Module {

    namespace {
        inline il2cpp_exports const* E() {
            return GetExports();
        }

        inline std::string build_member_key(const char* className, const char* memberName) {
            if (!className || !memberName) return memberName ? memberName : "";
            return std::string(className) + "::" + memberName;
        }

        inline const char* extract_member_name(const char* result) {
            const char* colonPos = std::strstr(result, "::");
            return colonPos ? colonPos + 2 : result;
        }

        inline const char* resolve_member_name(const char* rawClassName, const char* rawMemberName) {
            const char* stableClassName = Deobfuscation::GetStableName(rawClassName);

            const char* humanClassName = Deobfuscation::GetStableName(stableClassName);
            if (humanClassName && humanClassName != stableClassName &&
                std::strcmp(humanClassName, stableClassName) != 0) {
                std::string humanKey = build_member_key(humanClassName, rawMemberName);
                const char* deobf = Deobfuscation::GetStableName(humanKey.c_str());
                if (deobf && deobf != humanKey.c_str() && std::strcmp(deobf, humanKey.c_str()) != 0)
                    return extract_member_name(deobf);
            }

            std::string key = build_member_key(stableClassName, rawMemberName);
            const char* stable = Deobfuscation::GetStableName(key.c_str());
            if (stable && stable != key.c_str() && std::strcmp(stable, key.c_str()) != 0)
                return extract_member_name(stable);

            return nullptr;
        }
    }

    const char* Type::raw_name() const {
        // Do NOT call il2cpp_type_get_name. Its internal worker recursively
        // walks generic-instance args and calls Class::Init on each; the
        // runtime never null-checks the result of class_from_il2cpp_type
        // before passing it to Init, so any reachable uninitialized inflated
        // generic type causes an AV at [klass+0x135].
        //
        // Direct read of the type's underlying class is sufficient for every
        // consumer (display fallback in inspector/dumper/parser). For complex
        // formatted names (e.g. "List<T>", "Int32[]") use Type::full_name()
        // which routes through our cached stable-name helper.
        Class klass = get_class();
        if (!klass) return "";
        return klass.raw_name();
    }

    const char* Type::raw_ns() const {
        Class klass = get_class();
        if (klass) return klass.raw_ns();
        return "";
    }

    const char* Type::ns() const {
        if (!m_native || !E()) return "";

        if (E()->m_helperGetTypeStableNamespace) {
            const char* result = reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*)>(
                E()->m_helperGetTypeStableNamespace)(m_native);
            return result ? result : "";
        }

        Class klass = get_class();
        if (klass) return klass.ns();
        return "";
    }

    std::string Type::raw_full_name() const {
        Class klass = get_class();
        if (klass) return klass.raw_full_name();
        const char* n = raw_name();
        return n ? n : "";
    }

    std::string Type::full_name() const {
        if (!m_native || !E()) return "";

        if (E()->m_helperGetTypeStableFullName) {
            const char* result = reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*)>(
                E()->m_helperGetTypeStableFullName)(m_native);
            if (result && *result) return result;
        }

        Class klass = get_class();
        if (klass) return klass.full_name();

        const char* n = name();
        return n ? n : "";
    }

    // SEH-protected helper call for Type::get_class. Can't use __try in
    // Type::get_class itself because its caller/return has unwindable state;
    // factoring to a noexcept free function satisfies MSVC's SEH rules.
    static void* SafeCallClassFromIl2cppType(void* rawFn, void* type) noexcept {
        using Fn = void*(IL2CPP_CALLTYPE)(void*);
        __try {
            return reinterpret_cast<Fn>(rawFn)(type);
        } __except(1) {
            return nullptr;
        }
    }

    // Same Il2CppType structural probe as ClassResolver's. The runtime
    // __fastfails on bad Il2CppType structs, bypassing SEH — must reject
    // garbage pointers BEFORE the helper call.
    static bool TypeLooksValid(void* ptr) noexcept {
        if (!ptr) return false;
        auto p = reinterpret_cast<uintptr_t>(ptr);
        if (p < 0x10000 || p >= 0x7FFFFFFFFFFFull) return false;
        uint8_t typeEnum = 0;
        uint16_t attrs = 0;
        __try {
            typeEnum = *reinterpret_cast<const uint8_t*>(
                static_cast<const char*>(ptr) + 0x0A);
            attrs = *reinterpret_cast<const uint16_t*>(
                static_cast<const char*>(ptr) + 0x08);
        } __except(1) {
            return false;
        }
        if (typeEnum == 0 || typeEnum > 0x50) return false;
        if (attrs >= 0x8000) return false;
        return true;
    }

    Class Type::get_class() const {
        if (!m_native || !E()) return Class(nullptr);
        if (!TypeLooksValid(m_native)) return Class(nullptr);
        // Route through the filtered helper (m_helperClassFromType) rather than
        // calling m_classFromIl2cppType raw. The helper pre-filters complex type
        // kinds (GENERICINST/VAR/MVAR/ARRAY/etc) whose runtime resolution walks
        // metadata tables that may raise heap-corruption __fastfail, bypassing
        // SEH. Falling back to raw classFromIl2cppType here was the reason
        // Type::full_name() was still crashing after the vtable helper filter.
        if (E()->m_helperClassFromType)
            return Class(reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(E()->m_helperClassFromType)(m_native));
        if (E()->m_classFromIl2cppType)
            return Class(SafeCallClassFromIl2cppType(E()->m_classFromIl2cppType, m_native));
        return Class(nullptr);
    }

    const char* Type::name() const {
        if (!m_native || !E()) return "";

        if (E()->m_helperGetTypeStableName) {
            const char* result = reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*)>(
                E()->m_helperGetTypeStableName)(m_native);
            return (result && *result) ? result : "";
        }

        return "";
    }

    int Type::type_enum() const {
        if (!m_native || !E() || !E()->m_typeGetType) return -1;
        return reinterpret_cast<int(IL2CPP_CALLTYPE)(void*)>(E()->m_typeGetType)(m_native);
    }

    bool Type::is_byref() const {
        if (!m_native || !E() || !E()->m_typeIsByref) return false;
        return reinterpret_cast<bool(IL2CPP_CALLTYPE)(void*)>(E()->m_typeIsByref)(m_native);
    }

    const char* Type::assembly_qualified_name() const {
        if (!m_native || !E() || !E()->m_typeGetAssemblyQualifiedName) return "";
        return reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*)>(E()->m_typeGetAssemblyQualifiedName)(m_native);
    }

    void* Type::get_system_type_object() const {
        if (!m_native || !E() || !E()->m_typeGetObject) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(E()->m_typeGetObject)(m_native);
    }


    const char* Field::raw_name() const {
        if (!m_native) return "";
        return reinterpret_cast<il2cppFieldInfo*>(m_native)->name();
    }

    // SEH-protected pointer read — m_native can be an arbitrary pointer when
    // fingerprint/export resolution goes wrong, so direct dereferences may AV.
    // Factored to a noexcept free function so __try is legal (callers have
    // unwindable objects that block in-place __try).
    static void* SafeReadFieldQword(void* base, int32_t off) noexcept {
        if (!base) return nullptr;
        auto p = reinterpret_cast<uintptr_t>(base);
        if (p < 0x10000 || p >= 0x7FFFFFFFFFFFull) return nullptr;
        __try {
            return *reinterpret_cast<void**>(static_cast<char*>(base) + off);
        } __except(1) {
            return nullptr;
        }
    }
    static int32_t SafeReadFieldDword(void* base, int32_t off) noexcept {
        if (!base) return -1;
        auto p = reinterpret_cast<uintptr_t>(base);
        if (p < 0x10000 || p >= 0x7FFFFFFFFFFFull) return -1;
        __try {
            return *reinterpret_cast<int32_t*>(static_cast<char*>(base) + off);
        } __except(1) {
            return -1;
        }
    }

    const char* Field::name() const {
        const char* raw = raw_name();
        if (!raw || !*raw) return raw;

        int32_t parentOff = (E() && E()->m_offFieldParent >= 0) ? E()->m_offFieldParent : 0x10; // canonical: FieldInfo.parent;
        void* parentClass = SafeReadFieldQword(m_native, parentOff);
        if (IsValid(parentClass)) {
            int32_t classNameOff = (E() && E()->m_offClassName >= 0) ? E()->m_offClassName : 0x10;
            const char* rawClassName = reinterpret_cast<const char*>(
                SafeReadFieldQword(parentClass, classNameOff));
            if (IsValid(rawClassName)) {
                const char* resolved = resolve_member_name(rawClassName, raw);
                if (resolved) return resolved;
            }
        }

        return Deobfuscation::GetStableName(raw);
    }

    const char* Field::raw_type_name() const {
        Type t = type();
        return t ? t.raw_name() : "";
    }

    const char* Field::type_name() const {
        Type t = type();
        return t ? t.name() : "";
    }

    Type Field::type() const {
        if (!m_native) return Type{};
        int32_t off = (E() && E()->m_offFieldType >= 0) ? E()->m_offFieldType : 0x00;
        return Type{ SafeReadFieldQword(m_native, off) };
    }

    int Field::offset() const {
        if (!m_native) return -1;
        // Stride-0x30 (1832): offset@+0x20. Stride-0x28 (1830): offset@+0x20.
        // Stride-0x20 (Unity 2022 canonical): offset@+0x18. Default to 1832.
        int32_t off = (E() && E()->m_offFieldOffset >= 0) ? E()->m_offFieldOffset : 0x20;
        int32_t val = SafeReadFieldDword(m_native, off);
        if (val < 0 || val > 0x10000) return -1;
        return val;
    }

    uint32_t Field::attributes() const {
        if (!m_native) return 0;
        auto t = type();
        if (!t) return 0;
        void* typePtr = t.raw();
        if (!typePtr) return 0;
        return *reinterpret_cast<const uint16_t*>(static_cast<const char*>(typePtr) + 0x8);
    }

    bool Field::is_static() const {
        return (attributes() & FIELD_ATTRIBUTE_STATIC) != 0;
    }

    bool Field::is_public() const {
        return (attributes() & FIELD_ATTRIBUTE_ACCESS_MASK) == FIELD_ATTRIBUTE_PUBLIC;
    }

    bool Field::is_private() const {
        uint16_t access = attributes() & FIELD_ATTRIBUTE_ACCESS_MASK;
        return access == FIELD_ATTRIBUTE_PRIVATE || access == 0;  // PrivateScope == 0
    }

    bool Field::is_protected() const {
        uint16_t access = attributes() & FIELD_ATTRIBUTE_ACCESS_MASK;
        return access == FIELD_ATTRIBUTE_FAMILY || access == FIELD_ATTRIBUTE_FAM_OR_ASSEM;
    }

    bool Field::is_internal() const {
        uint16_t access = attributes() & FIELD_ATTRIBUTE_ACCESS_MASK;
        return access == FIELD_ATTRIBUTE_ASSEMBLY || access == FIELD_ATTRIBUTE_FAM_OR_ASSEM;
    }

    bool Field::is_readonly() const {
        return (attributes() & FIELD_ATTRIBUTE_INIT_ONLY) != 0;
    }

    bool Field::is_const() const {
        return (attributes() & FIELD_ATTRIBUTE_LITERAL) != 0;
    }

    const char* Field::access_modifier() const {
        uint16_t access = attributes() & FIELD_ATTRIBUTE_ACCESS_MASK;
        switch (access) {
            case FIELD_ATTRIBUTE_PUBLIC:        return "public";
            case FIELD_ATTRIBUTE_PRIVATE:       return "private";
            case FIELD_ATTRIBUTE_FAMILY:        return "protected";
            case FIELD_ATTRIBUTE_ASSEMBLY:      return "internal";
            case FIELD_ATTRIBUTE_FAM_OR_ASSEM:  return "protected internal";
            case FIELD_ATTRIBUTE_FAM_AND_ASSEM: return "private protected";
            default:                            return "private";  // PrivateScope
        }
    }

    // SEH-wrapped IL2CPP runtime calls. Static field access internally walks
    // field.parent.static_fields_data via metadata — if our FieldInfo* came
    // from a name match that resolved to a non-field struct, or the runtime's
    // metadata lookup fails, it AVs inside GameAssembly. Factored to noexcept
    // helpers so __try is legal (callers have C++ objects that would block it).
    static void SafeCallFieldStatic2(void* fn, void* a, void* b) noexcept {
        using Fn = void(IL2CPP_CALLTYPE)(void*, void*);
        __try {
            reinterpret_cast<Fn>(fn)(a, b);
        } __except(1) { }
    }
    static void SafeCallFieldStatic3(void* fn, void* a, void* b, void* c) noexcept {
        using Fn = void(IL2CPP_CALLTYPE)(void*, void*, void*);
        __try {
            reinterpret_cast<Fn>(fn)(a, b, c);
        } __except(1) { }
    }

    void Field::get_value(void* obj, void* out_value) const {
        if (!m_native || !obj || !out_value || !E() || !E()->m_fieldGetValue) return;
        SafeCallFieldStatic3(E()->m_fieldGetValue, obj, m_native, out_value);
    }

    void Field::set_value(void* obj, void* value) const {
        if (!m_native || !obj || !value || !E() || !E()->m_fieldSetValue) return;
        SafeCallFieldStatic3(E()->m_fieldSetValue, obj, m_native, value);
    }

    void Field::get_static_value(void* out_value) const {
        if (!m_native || !out_value || !E() || !E()->m_fieldStaticGetValue) return;
        SafeCallFieldStatic2(E()->m_fieldStaticGetValue, m_native, out_value);
    }

    void Field::set_static_value(void* value) const {
        if (!m_native || !value || !E() || !E()->m_fieldStaticSetValue) return;
        SafeCallFieldStatic2(E()->m_fieldStaticSetValue, m_native, value);
    }



    const char* Method::raw_name() const {
        if (!m_native || !E() || E()->m_offMethodName < 0) return "";
        auto* n = *reinterpret_cast<const char**>(static_cast<char*>(m_native) + E()->m_offMethodName);
        return n ? n : "";
    }

    const char* Method::name() const {
        const char* raw = raw_name();
        if (!raw || !*raw) return raw;

        int32_t declOff = (E() && E()->m_offMethodDeclType >= 0) ? E()->m_offMethodDeclType : 0x18; // canonical: MethodInfo.klass;
        void* parentClass = *reinterpret_cast<void**>(static_cast<char*>(m_native) + declOff);
        if (IsValid(parentClass)) {
            const char* rawClassName = *reinterpret_cast<const char**>(
                static_cast<char*>(parentClass) + ((E() && E()->m_offClassName >= 0) ? E()->m_offClassName : 0x10));

            if (IsValid(rawClassName)) {
                const char* resolved = resolve_member_name(rawClassName, raw);
                if (resolved) return resolved;
            }
        }

        return Deobfuscation::GetStableName(raw);
    }

    const char* Method::raw_return_type_name() const {
        Type t = return_type();
        return t ? t.raw_name() : "";
    }

    const char* Method::return_type_name() const {
        Type t = return_type();
        return t ? t.name() : "";
    }

    void* Method::pointer() const {
        if (!m_native) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*>(m_native)->code_pointer();
    }

    uint8_t Method::param_count() const {
        if (!m_native) return 0;
        if (E() && E()->m_offMethodArgCount >= 0) {
            return *reinterpret_cast<const uint8_t*>(
                static_cast<const char*>(m_native) + E()->m_offMethodArgCount);
        }
        if (E() && E()->m_methodGetParam) {
            uint8_t count = 0;
            auto fn = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uint32_t)>(E()->m_methodGetParam);
            for (uint8_t i = 0; i < 32; ++i) {
                if (!fn(m_native, i)) break;
                ++count;
            }
            return count;
        }
        return 0;
    }

    uint32_t Method::flags(uint32_t* impl_flags) const {
        if (!m_native || !E() || !E()->m_methodGetFlags) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*, uint32_t*)>(E()->m_methodGetFlags)(m_native, impl_flags);
    }

    bool Method::is_static() const {
        return (flags() & METHOD_ATTRIBUTE_STATIC) != 0;
    }

    bool Method::is_public() const {
        return (flags() & METHOD_ATTRIBUTE_ACCESS_MASK) == METHOD_ATTRIBUTE_PUBLIC;
    }

    bool Method::is_private() const {
        uint32_t access = flags() & METHOD_ATTRIBUTE_ACCESS_MASK;
        return access == METHOD_ATTRIBUTE_PRIVATE || access == 0;  // PrivateScope == 0
    }

    bool Method::is_protected() const {
        uint32_t access = flags() & METHOD_ATTRIBUTE_ACCESS_MASK;
        return access == METHOD_ATTRIBUTE_FAMILY || access == METHOD_ATTRIBUTE_FAM_OR_ASSEM;
    }

    bool Method::is_internal() const {
        uint32_t access = flags() & METHOD_ATTRIBUTE_ACCESS_MASK;
        return access == METHOD_ATTRIBUTE_ASSEMBLY || access == METHOD_ATTRIBUTE_FAM_OR_ASSEM;
    }

    bool Method::is_virtual() const {
        return (flags() & METHOD_ATTRIBUTE_VIRTUAL) != 0;
    }

    bool Method::is_abstract() const {
        return (flags() & METHOD_ATTRIBUTE_ABSTRACT) != 0;
    }

    bool Method::is_sealed() const {
        return (flags() & METHOD_ATTRIBUTE_FINAL) != 0;
    }

    const char* Method::access_modifier() const {
        uint32_t access = flags() & METHOD_ATTRIBUTE_ACCESS_MASK;
        switch (access) {
            case METHOD_ATTRIBUTE_PUBLIC:        return "public";
            case METHOD_ATTRIBUTE_PRIVATE:       return "private";
            case METHOD_ATTRIBUTE_FAMILY:        return "protected";
            case METHOD_ATTRIBUTE_ASSEMBLY:      return "internal";
            case METHOD_ATTRIBUTE_FAM_OR_ASSEM:  return "protected internal";
            case METHOD_ATTRIBUTE_FAM_AND_ASSEM: return "private protected";
            default:                             return "private";  // PrivateScope
        }
    }

    Type Method::get_param_type(uint32_t index) const {
        if (!m_native || !E() || !E()->m_methodGetParam) return Type{};
        return Type{ reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uint32_t)>(E()->m_methodGetParam)(m_native, index) };
    }

    const char* Method::get_param_name(uint32_t index) const {
        if (!m_native || !E() || !E()->m_methodGetParamName) return "";
        const char* name = reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*, uint32_t)>(E()->m_methodGetParamName)(m_native, index);
        if (name && *name) {
            for (const unsigned char* p = reinterpret_cast<const unsigned char*>(name); *p; ++p) {
                if (*p > 0x7E || *p < 0x20) {
                    static thread_local char buf[32];
                    snprintf(buf, sizeof(buf), "arg_%u", index);
                    return buf;
                }
            }
        }
        return name ? name : "";
    }

    Type Method::return_type() const {
        if (!m_native) return Type{};
        
        if (E() && E()->m_helperGetMethodReturnType) {
            void* retType = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(
                E()->m_helperGetMethodReturnType)(m_native);
            if (IsValid(retType)) {
                return Type{ retType };
            }
            return Type{};  // Return empty type if pointer is invalid
        }

        int32_t retOff = (E() && E()->m_offMethodRetType >= 0) ? E()->m_offMethodRetType : 0x20; // canonical: MethodInfo.return_type;
        void* retType = *reinterpret_cast<void**>(static_cast<char*>(m_native) + retOff);
        return IsValid(retType) ? Type{ retType } : Type{};
    }

    void* Method::invoke(void* obj, void** params, void** exc) const {
        if (!m_native || !E() || !E()->m_runtimeInvoke) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, void*, void**, void**)>(
            E()->m_runtimeInvoke)(m_native, obj, params, exc);
    }



    const char* Property::raw_name() const {
        if (!m_native) return "";
        return reinterpret_cast<il2cppPropertyInfo*>(m_native)->get_name();
    }

    const char* Property::name() const {
        const char* raw = raw_name();
        if (!raw || !*raw) return raw;

        void* parentClass = *reinterpret_cast<void**>(m_native);
        if (IsValid(parentClass)) {
            const char* rawClassName = *reinterpret_cast<const char**>(
                static_cast<char*>(parentClass) + ((E() && E()->m_offClassName >= 0) ? E()->m_offClassName : 0x10));
            if (IsValid(rawClassName)) {
                const char* resolved = resolve_member_name(rawClassName, raw);
                if (resolved) return resolved;
            }
        }

        return Deobfuscation::GetStableName(raw);
    }

    const char* Property::raw_type_name() const {
        Method g = getter();
        if (g) {
            return g.raw_return_type_name();
        }
        Method s = setter();
        if (s && s.param_count() > 0) {
            Type paramType = s.get_param_type(0);
            return paramType ? paramType.raw_name() : "";
        }
        return "";
    }

    const char* Property::type_name() const {
        Method g = getter();
        if (g) {
            return g.return_type_name();
        }
        Method s = setter();
        if (s && s.param_count() > 0) {
            Type paramType = s.get_param_type(0);
            return paramType ? paramType.name() : "";
        }
        return "";
    }

    Method Property::getter() const {
        if (!m_native) return Method{};
        return Method{ reinterpret_cast<il2cppPropertyInfo*>(m_native)->get_getter() };
    }

    Method Property::setter() const {
        if (!m_native) return Method{};
        return Method{ reinterpret_cast<il2cppPropertyInfo*>(m_native)->get_setter() };
    }

    void* Property::parent_class_raw() const {
        if (!m_native) return nullptr;
        return *reinterpret_cast<void**>(m_native);
    }

    bool Property::is_public() const {
        Method g = getter();
        Method s = setter();
        return (g && g.is_public()) || (s && s.is_public());
    }

    bool Property::is_private() const {
        Method g = getter();
        Method s = setter();
        bool getterPrivate = !g || g.is_private();
        bool setterPrivate = !s || s.is_private();
        return getterPrivate && setterPrivate && (g || s);  // Must have at least one accessor
    }

    bool Property::is_readonly() const {
        return getter() && !setter();
    }

    bool Property::is_writeonly() const {
        return setter() && !getter();
    }

    const char* Property::access_modifier() const {
        Method g = getter();
        Method s = setter();
        if ((g && g.is_public()) || (s && s.is_public())) return "public";
        if ((g && g.is_protected()) || (s && s.is_protected())) return "protected";
        if ((g && g.is_internal()) || (s && s.is_internal())) return "internal";
        return "private";
    }


    // Probe if `ptr` is readable — catches pointers in user-mode range that
    // land in RESERVE'd / uncommitted memory. Observed: some obfuscated
    // classes carry name pointers like 0x60AEC040 pointing into a reserved
    // 25MB region; strcmp on those AVs. Factored out so the SEH lives in a
    // function with no C++ objects (MSVC rule).
    static bool IsReadable1Byte(const void* p) noexcept {
        __try {
            volatile char probe = *static_cast<const volatile char*>(p);
            (void)probe;
            return true;
        } __except(1) {
            return false;
        }
    }

    // SEH-protected qword read at an arbitrary base+off. Probes the source
    // memory, not just the returned pointer — m_native itself can point to
    // FREE'd memory when resolver's fieldClass resolution gets stale ptrs.
    static const char* SafeReadClassString(void* native, int32_t off) noexcept {
        if (!native) return nullptr;
        auto n = reinterpret_cast<uintptr_t>(native);
        if (n < 0x10000 || n >= 0x7FFFFFFFFFFFull) return nullptr;
        const char* ptr = nullptr;
        __try {
            ptr = *reinterpret_cast<const char**>(static_cast<char*>(native) + off);
        } __except(1) {
            return nullptr;
        }
        if (!ptr) return nullptr;
        auto p = reinterpret_cast<uintptr_t>(ptr);
        if (p < 0x10000 || p >= 0x7FFFFFFFFFFFull) return nullptr;
        return IsReadable1Byte(ptr) ? ptr : nullptr;
    }

    const char* Class::raw_name() const {
        int32_t off = (E() && E()->m_offClassName >= 0) ? E()->m_offClassName : 0x10; // canonical: Il2CppClass.name
        const char* ptr = SafeReadClassString(m_native, off);
        return ptr ? ptr : "";
    }

    const char* Class::name() const {
        if (!m_native) return "";

        if (E() && E()->m_helperGetClassStableName) {
            // Wrap BOTH the helper call and the dereference in SEH — the
            // helper has been observed to (a) return a garbage pointer like
            // 0xFFFFFFFF........ that crashes on deref, and (b) AV inside
            // its own body when given certain obfuscated classes. Either
            // way, fall back to the direct struct read.
            const char* result = nullptr;
            __try {
                result = reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*)>(
                    E()->m_helperGetClassStableName)(m_native);
                auto p = reinterpret_cast<uintptr_t>(result);
                if (p >= 0x10000 && p < 0x7FFFFFFFFFFFull && *result)
                    return result;
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                // Fall through to raw_name() below.
            }
        }

        return raw_name();
    }

    const char* Class::raw_ns() const {
        int32_t off = (E() && E()->m_offClassNamespace >= 0) ? E()->m_offClassNamespace : 0x18;
        const char* ptr = SafeReadClassString(m_native, off);
        return ptr ? ptr : "";
    }

    const char* Class::ns() const {
        if (!m_native) return "";

        if (E() && E()->m_helperGetClassStableNamespace) {
            const char* result = nullptr;
            __try {
                result = reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*)>(
                    E()->m_helperGetClassStableNamespace)(m_native);
                auto p = reinterpret_cast<uintptr_t>(result);
                if (p >= 0x10000 && p < 0x7FFFFFFFFFFFull)
                    return result;
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                // Fall through.
            }
        }

        const char* raw = raw_ns();
        if (!raw || !*raw) return raw;
        return Deobfuscation::GetStableName(raw);
    }

    std::string Class::full_name() const {
        if (!m_native) return "";
        
        if (E() && E()->m_helperGetClassStableFullName) {
            const char* result = reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*)>(
                E()->m_helperGetClassStableFullName)(m_native);
            if (result && *result) return result;
        }

        const char* n = ns();
        const char* c = name();
        if (n && n[0]) return std::string(n) + "." + c;
        return c ? c : "";
    }

    std::string Class::raw_full_name() const {
        if (!m_native) return "";
        const char* n = raw_ns();
        const char* c = raw_name();
        if (n && n[0]) return std::string(n) + "." + c;
        return c ? c : "";
    }

    Class Class::parent() const {
        if (!m_native || !E() || !E()->m_classGetParent) return Class{};
        return Class{ reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(E()->m_classGetParent)(m_native) };
    }

    Type Class::get_type() const {
        if (!m_native || !E() || !E()->m_classGetType) return Type{};
        return Type{ reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(E()->m_classGetType)(m_native) };
    }

    bool Class::is_subclass_of(const Class& other) const {
        if (!m_native || !other.m_native || !E() || !E()->m_classIsSubclassOf) return false;
        return reinterpret_cast<bool(IL2CPP_CALLTYPE)(void*, void*)>(E()->m_classIsSubclassOf)(m_native, other.m_native);
    }

    bool Class::is_enum() const {
        if (!m_native || !E() || !E()->m_classIsEnum) return false;
        return reinterpret_cast<bool(IL2CPP_CALLTYPE)(void*)>(E()->m_classIsEnum)(m_native);
    }

    bool Class::is_generic() const {
        if (!m_native || !E() || !E()->m_classIsGeneric) return false;
        return reinterpret_cast<bool(IL2CPP_CALLTYPE)(void*)>(E()->m_classIsGeneric)(m_native);
    }


    uint32_t Class::instance_size() const {
        if (!m_native) return 0;
        constexpr size_t offset = 22 * sizeof(void*);
        return *reinterpret_cast<uint32_t*>(static_cast<char*>(m_native) + offset);
    }

    void* Class::static_field_data() const {
        if (!m_native || !E() || !E()->m_classGetStaticFieldData) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(E()->m_classGetStaticFieldData)(m_native);
    }

    Field Class::get_field(std::string_view fname) const {
        if (!m_native || !E() || !E()->m_classGetFieldFromName) return Field{};

        std::string nameStr(fname);
        void* result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, const char*)>(
            E()->m_classGetFieldFromName)(m_native, nameStr.c_str());
        if (result) return Field{ result };

        const char* original = Deobfuscation::GetOriginalName(nameStr.c_str());
        if (original && original != nameStr.c_str() && std::strcmp(original, nameStr.c_str()) != 0) {
            result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, const char*)>(
                E()->m_classGetFieldFromName)(m_native, original);
            if (result) return Field{ result };
        }

        std::string rawFull = raw_full_name();
        const char* stableName = Deobfuscation::GetStableName(rawFull.c_str());
        std::string qualifiedKey = build_member_key(stableName, nameStr.c_str());
        original = Deobfuscation::GetOriginalName(qualifiedKey.c_str());
        if (original && original != qualifiedKey.c_str() && std::strcmp(original, qualifiedKey.c_str()) != 0) {
            result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, const char*)>(
                E()->m_classGetFieldFromName)(m_native, extract_member_name(original));
        }

        return Field{ result };
    }

    // SEH-protected single iterator step. Lives outside get_fields/get_methods
    // because MSVC forbids __try in functions with C++ objects that need
    // unwinding (std::vector). Returns nullptr on AV or iterator end.
    namespace {
        using IterFn = void*(IL2CPP_CALLTYPE)(void*, void**);
        void* SafeIterStep(void* rawFn, void* klass, void** iter) noexcept {
            auto fn = reinterpret_cast<IterFn>(rawFn);
            __try {
                return fn(klass, iter);
            } __except(1) {  // 1 = EXCEPTION_EXECUTE_HANDLER
                return nullptr;
            }
        }

        bool TryGetDirectFieldArray(
            void* klass,
            il2cppFieldInfo*& fieldsBase,
            uint16_t& fieldCount,
            int32_t& fieldStride) noexcept
        {
            fieldsBase = nullptr;
            fieldCount = 0;
            fieldStride = -1;

            if (!klass || !E()) return false;
            if (E()->m_offClassFields < 0 || E()->m_offClassFieldCount < 0) return false;
            if (E()->m_fieldInfoStride <= 0) return false;

            __try {
                auto klassBytes = static_cast<const char*>(klass);
                uintptr_t fieldsPtr  = *reinterpret_cast<const uintptr_t*>(klassBytes + E()->m_offClassFields);
                uint32_t countDword  = *reinterpret_cast<const uint32_t*>(klassBytes + E()->m_offClassFieldCount);
                uint16_t count       = static_cast<uint16_t>(countDword & 0xFFFF);
                if (count == 0) {
                    fieldStride = E()->m_fieldInfoStride;
                    return true;
                }
                if (fieldsPtr < 0x10000 || fieldsPtr > 0x7FFFFFFFFFFF) return false;

                fieldsBase = reinterpret_cast<il2cppFieldInfo*>(fieldsPtr);
                fieldCount = count;
                fieldStride = E()->m_fieldInfoStride;
                return true;
            } __except(1) {
                return false;
            }
        }

        il2cppFieldInfo* DirectFieldAt(
            il2cppFieldInfo* fieldsBase, int32_t fieldStride, uint16_t index) noexcept
        {
            if (!fieldsBase || fieldStride <= 0) return nullptr;
            return reinterpret_cast<il2cppFieldInfo*>(
                reinterpret_cast<uint8_t*>(fieldsBase) + (static_cast<size_t>(index) * static_cast<size_t>(fieldStride)));
        }

        bool NeedsFieldMaterialization(void* klass) noexcept {
            if (!klass || !E()) return false;
            if (E()->m_offClassFields < 0 || E()->m_offClassFieldCount < 0) return false;

            __try {
                auto klassBytes = static_cast<const char*>(klass);
                uintptr_t fieldsPtr = *reinterpret_cast<const uintptr_t*>(
                    klassBytes + E()->m_offClassFields);
                uint32_t countDword = *reinterpret_cast<const uint32_t*>(
                    klassBytes + E()->m_offClassFieldCount);
                uint16_t fieldCount = static_cast<uint16_t>(countDword & 0xFFFF);
                return fieldCount != 0 && fieldsPtr == 0;
            } __except(1) {
                return false;
            }
        }

        bool TryRuntimeClassInit(void* rawFn, void* klass) noexcept {
            if (!rawFn || !klass) return false;
            using InitFn = void(IL2CPP_CALLTYPE)(void*);
            __try {
                reinterpret_cast<InitFn>(rawFn)(klass);
                return true;
            } __except(1) {
                return false;
            }
        }

        void EnsureClassInitializedForFields(void* klass) noexcept {
            if (!NeedsFieldMaterialization(klass)) return;

            static void* initFn = reinterpret_cast<void*>(
                ResolveCall("il2cpp_runtime_class_init"));
            if (!initFn) return;

            TryRuntimeClassInit(initFn, klass);
        }
    }

    std::vector<Field> Class::get_fields() const {
        std::vector<Field> result;
        if (!m_native || !E() || !E()->m_classGetFields) return result;

        EnsureClassInitializedForFields(m_native);
        il2cppFieldInfo* fieldsBase = nullptr;
        uint16_t directFieldCount = 0;
        int32_t fieldStride = -1;
        if (TryGetDirectFieldArray(m_native, fieldsBase, directFieldCount, fieldStride) && fieldsBase) {
            result.reserve(directFieldCount);
            for (uint16_t i = 0; i < directFieldCount; ++i) {
                auto* f = DirectFieldAt(fieldsBase, fieldStride, i);
                if (!f) continue;
                result.emplace_back(f);
            }
            return result;
        }

        void* iter = nullptr;
        while (true) {
            auto* f = SafeIterStep(E()->m_classGetFields, m_native, &iter);
            if (!f) break;
            result.emplace_back(f);
        }
        return result;
    }

    Method Class::get_method(std::string_view mname, int argc) const {
        if (!m_native || !E() || !E()->m_classGetMethodFromName) return Method{};

        std::string nameStr(mname);
        void* result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, const char*, int)>(
            E()->m_classGetMethodFromName)(m_native, nameStr.c_str(), argc);
        if (result) return Method{ result };

        const char* original = Deobfuscation::GetOriginalName(nameStr.c_str());
        if (original && original != nameStr.c_str() && std::strcmp(original, nameStr.c_str()) != 0) {
            result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, const char*, int)>(
                E()->m_classGetMethodFromName)(m_native, original, argc);
            if (result) return Method{ result };
        }

        std::string rawFull = raw_full_name();
        const char* stableName = Deobfuscation::GetStableName(rawFull.c_str());
        std::string qualifiedKey = build_member_key(stableName, nameStr.c_str());
        original = Deobfuscation::GetOriginalName(qualifiedKey.c_str());
        if (original && original != qualifiedKey.c_str() && std::strcmp(original, qualifiedKey.c_str()) != 0) {
            result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, const char*, int)>(
                E()->m_classGetMethodFromName)(m_native, extract_member_name(original), argc);
        }

        return Method{ result };
    }

    void* Class::get_method_pointer(std::string_view mname, int argc) const {
        auto m = get_method(mname, argc);
        return m ? m.pointer() : nullptr;
    }

    std::vector<Method> Class::get_methods() const {
        std::vector<Method> result;
        if (!m_native || !E() || !E()->m_classGetMethods) return result;
        void* iter = nullptr;
        while (auto* m = SafeIterStep(E()->m_classGetMethods, m_native, &iter)) {
            result.emplace_back(m);
        }
        return result;
    }

    Property Class::get_property(std::string_view pname) const {
        if (!m_native || !E() || !E()->m_classGetPropertyFromName) return Property{};

        std::string nameStr(pname);
        void* result = Module::GetPropertyByName(
            static_cast<il2cppClass*>(m_native), nameStr.c_str());
        if (result) return Property{ result };

        const char* original = Deobfuscation::GetOriginalName(nameStr.c_str());
        if (original && original != nameStr.c_str() && std::strcmp(original, nameStr.c_str()) != 0) {
            result = Module::GetPropertyByName(
                static_cast<il2cppClass*>(m_native), original);
            if (result) return Property{ result };
        }

        std::string rawFull = raw_full_name();
        const char* stableName = Deobfuscation::GetStableName(rawFull.c_str());
        std::string qualifiedKey = build_member_key(stableName, nameStr.c_str());
        original = Deobfuscation::GetOriginalName(qualifiedKey.c_str());
        if (original && original != qualifiedKey.c_str() && std::strcmp(original, qualifiedKey.c_str()) != 0) {
            result = Module::GetPropertyByName(
                static_cast<il2cppClass*>(m_native), extract_member_name(original));
        }

        return Property{ result };
    }

    std::vector<Property> Class::get_properties() const {
        std::vector<Property> result;
        if (!m_native || !E() || !E()->m_classGetProperties) return result;
        void* iter = nullptr;
        while (auto* p = SafeIterStep(E()->m_classGetProperties, m_native, &iter)) {
            result.emplace_back(p);
        }
        return result;
    }

    Class Class::find(std::string_view full_name) {
        if (!E() || !E()->m_helperFindClass) return Class{};

        std::string nameStr(full_name);
        void* result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(
            E()->m_helperFindClass)(nameStr.c_str());
        if (result) return Class{ result };

        const char* original = Deobfuscation::GetOriginalName(nameStr.c_str());
        if (original && original != nameStr.c_str() && std::strcmp(original, nameStr.c_str()) != 0) {
            result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(
                E()->m_helperFindClass)(original);
        }

        return Class{ result };
    }

    ManagedObject Class::new_object() const {
        if (!m_native || !E() || !E()->m_objectNew) return ManagedObject{};
        return ManagedObject{ reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(E()->m_objectNew)(m_native) };
    }


    const char* Image::name() const {
        if (!m_native) return "";
        ptrdiff_t off = 0;
        if (E() && E()->m_offImgName >= 0) off = E()->m_offImgName;
        auto* ptr = *reinterpret_cast<const char* const*>(
            reinterpret_cast<const char*>(m_native) + off);
        if (reinterpret_cast<uintptr_t>(ptr) < 0x10000 || reinterpret_cast<uintptr_t>(ptr) > 0x7FFFFFFFFFFF)
            return "";
        return ptr;
    }

    uint32_t Image::class_count() const {
        if (!m_native || !E() || !E()->m_imageGetClassCount) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*)>(E()->m_imageGetClassCount)(m_native);
    }

    Class Image::get_class(uint32_t index) const {
        if (!m_native || !E() || !E()->m_imageGetClass) return Class{};
        return Class{ reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uint32_t)>(
            E()->m_imageGetClass)(m_native, index) };
    }

    std::vector<Class> Image::get_classes() const {
        std::vector<Class> result;
        uint32_t count = class_count();
        result.reserve(count);
        for (uint32_t i = 0; i < count; ++i) {
            auto klass = get_class(i);
            if (klass) result.push_back(klass);
        }
        return result;
    }


    Image Assembly::get_image() const {
        if (!m_native) return Image{};
        if (E() && E()->m_assemblyGetImage) {
            return Image{ reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(
                E()->m_assemblyGetImage)(m_native) };
        }
        return Image{ *reinterpret_cast<void**>(m_native) };
    }

    std::vector<Assembly> Assembly::get_all() {
        std::vector<Assembly> result;
        if (!E() || !E()->m_domainGetAssemblies) return result;

        void* domain = nullptr;
        if (E()->m_domainGet) {
            domain = reinterpret_cast<void*(IL2CPP_CALLTYPE)()>(E()->m_domainGet)();
        }
        if (!domain) return result;

        size_t count = 0;
        void** assemblies = reinterpret_cast<void**(IL2CPP_CALLTYPE)(void*, size_t*)>(
            E()->m_domainGetAssemblies)(domain, &count);

        if (!assemblies || count == 0) return result;

        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            if (assemblies[i]) {
                result.emplace_back(assemblies[i]);
            }
        }

        return result;
    }

} // namespace IL2CPP::Module
