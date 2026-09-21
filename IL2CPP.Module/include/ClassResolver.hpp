#pragma once
#include "Reflection.hpp"
#include "Deobfuscation.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <vector>
#include <string>
#include <optional>
#include <utility>
#include <cstring>

namespace IL2CPP::Module {

    // Optional trace hook — Bootstrap sets this at startup to forward lines
    // to its logger. When null, validate/apply run silently. Used to pinpoint
    // which class kills ClassResolver::resolve — the last printed line is
    // the class that crashed during field/method iteration.
    using ResolverTraceFn = void(*)(const char* msg);
    inline ResolverTraceFn g_resolverTrace = nullptr;

    inline void SetResolverTrace(ResolverTraceFn fn) { g_resolverTrace = fn; }

    inline void ResolverTrace(const char* msg) {
        if (g_resolverTrace) g_resolverTrace(msg);
    }

    class FieldQuery;
    class MethodQuery;
    class PropertyQuery;
    class FieldCounter;
    class IndexedFieldCollector;
    class ClassResolver;


    namespace detail {
        // A mapping keyed on nothing but the prefix can never be looked up, and
        // every such registration under the same prefix collides with the last,
        // so the survivor is arbitrary. Drop them at the source instead.
        // Names resolve in two hops: obfuscated -> generated stable name, then
        // stable -> human. The obfuscated key is already taken by the first hop,
        // so a human name registered against it is rejected as a duplicate and
        // never reached; it belongs on the stable name.
        inline void AddNamedMapping(std::string_view prefix, const char* rawName,
                                    std::string_view deobfName) {
            if (!rawName || !*rawName) return;
            std::string key = std::string(prefix) + rawName;
            const char* stable = Deobfuscation::GetStableName(key.c_str());
            Deobfuscation::AddMapping((stable && *stable) ? std::string(stable) : key, deobfName);
        }

        inline std::string_view ShortTypeName(std::string_view name) noexcept {
            if (name.empty()) return name;
            size_t pos = name.find_last_of(".+/");
            return (pos == std::string_view::npos) ? name : name.substr(pos + 1);
        }

        inline bool TypeNameEquals(std::string_view lhs, std::string_view rhs) noexcept {
            if (lhs.empty() || rhs.empty()) return false;
            if (lhs == rhs) return true;
            return ShortTypeName(lhs) == ShortTypeName(rhs);
        }

        // Field-side candidate names derived from a field's declared type.
        // field.type_name()/raw_type_name() are Type::name()/raw_name() on the
        // same type, and Type::raw_full_name() is Class::raw_full_name() once
        // the class resolved — only the entries that can differ are collected.
        inline std::vector<std::string> CollectFieldTypeNames(Field field, Class fieldClass) {
            std::vector<std::string> names;
            names.reserve(8);
            if (fieldClass) {
                names.emplace_back(fieldClass.name());
                names.emplace_back(fieldClass.raw_name());
                names.push_back(fieldClass.full_name());
                names.push_back(fieldClass.raw_full_name());
            }
            if (Type fieldType = field.type()) {
                names.emplace_back(fieldType.name());
                names.emplace_back(fieldType.raw_name());
                names.push_back(fieldType.full_name());
                if (!fieldClass) names.push_back(fieldType.raw_full_name());
            }
            return names;
        }

        // Per-field state shared by every query in one validate() pass. The
        // candidate-name set is built at most once, on first use.
        struct FieldMatchCtx {
            Field       field;
            Class       fieldClass;
            bool        isStatic = false;
            const char* name = nullptr;

            const std::vector<std::string>& type_names() const {
                if (!m_built) {
                    m_names = CollectFieldTypeNames(field, fieldClass);
                    m_built = true;
                }
                return m_names;
            }

        private:
            mutable std::vector<std::string> m_names;
            mutable bool                     m_built = false;
        };

        inline bool FieldTypeNamesContain(const std::vector<std::string>& names,
                                          std::string_view expected) {
            if (expected.empty()) return true;
            for (const std::string& n : names)
                if (TypeNameEquals(n, expected)) return true;
            return false;
        }

        inline bool FieldTypeNameMatches(const FieldMatchCtx& f, std::string_view expected) {
            if (expected.empty()) return true;
            return FieldTypeNamesContain(f.type_names(), expected);
        }

        inline bool FieldMatchesTargetClass(const FieldMatchCtx& f, Class targetClass) {
            if (!targetClass) return false;
            if (f.fieldClass && f.fieldClass.raw() == targetClass.raw()) return true;

            const std::vector<std::string>& names = f.type_names();

            if (FieldTypeNamesContain(names, targetClass.name())) return true;
            if (FieldTypeNamesContain(names, targetClass.raw_name())) return true;

            std::string fullName = targetClass.full_name();
            if (!fullName.empty() && FieldTypeNamesContain(names, fullName)) return true;

            std::string rawFullName = targetClass.raw_full_name();
            if (!rawFullName.empty() && FieldTypeNamesContain(names, rawFullName)) return true;

            return false;
        }

        // SEH-protected helper call. Factored into noexcept function so it can
        // use __try even when callers have C++ objects needing unwinding.
        inline void* SafeCallClassFromType(void* rawFn, void* type) noexcept {
            using Fn = void*(IL2CPP_CALLTYPE)(void*);
            __try {
                return reinterpret_cast<Fn>(rawFn)(type);
            } __except(1) {
                return nullptr;
            }
        }

        // Structural probe — verifies a pointer looks like a real Il2CppType
        // before handing it to m_helperClassFromType. The helper validates its
        // input and __fastfail's on garbage (bypassing our SEH); we must
        // reject bogus pointers BEFORE the call.
        //
        // Il2CppType layout (canonical):
        //   +0x00  data ptr (or TypeIndex union, readable)
        //   +0x08  attrs (u16)  — sane values have high bits zero
        //   +0x0A  type enum (u8) — 0x01..0x50 (TYPE_VOID..TYPE_PINNED)
        inline bool LooksLikeIl2CppType(void* ptr) noexcept {
            if (!ptr) return false;
            auto p = reinterpret_cast<uintptr_t>(ptr);
            if (p < 0x10000 || p >= 0x7FFFFFFFFFFFull) return false;
            uint8_t typeEnum = 0;
            uint16_t attrs = 0;
            __try {
                // Read type-enum byte at +0x0A
                typeEnum = *reinterpret_cast<const uint8_t*>(
                    static_cast<const char*>(ptr) + 0x0A);
                // Read attrs u16 at +0x08
                attrs = *reinterpret_cast<const uint16_t*>(
                    static_cast<const char*>(ptr) + 0x08);
            } __except(1) {
                return false;
            }
            if (typeEnum == 0 || typeEnum > 0x50) return false;
            // Attrs top 8 bits should be zero (it's a u16 field; the upper
            // dword at +0x08..+0x0B contains attrs u16 + type u8 + byref/pinned
            // flag byte). If the value looks like a pointer (high bits set),
            // it's not a valid Il2CppType header.
            if (attrs >= 0x8000) return false;
            return true;
        }

        inline Class ClassFromFieldType(Field field) {
            if (!field) return Class{};
            Type t = field.type();
            if (!t) return Class{};
            // Prevalidate before calling helper — the runtime __fastfails on
            // bad type structs, bypassing any SEH we could wrap around the
            // call. Reject garbage up front.
            if (!LooksLikeIl2CppType(t.raw())) return Class{};
            auto* e = GetExports();
            if (!e || !e->m_helperClassFromType) return Class{};
            return Class{ SafeCallClassFromType(e->m_helperClassFromType, t.raw()) };
        }

        inline Class ClassFromMethodReturnType(Method method) {
            if (!method) return Class{};
            Type t = method.return_type();
            if (!t) return Class{};
            if (!LooksLikeIl2CppType(t.raw())) return Class{};
            auto* e = GetExports();
            if (!e || !e->m_helperClassFromType) return Class{};
            return Class{ SafeCallClassFromType(e->m_helperClassFromType, t.raw()) };
        }

        inline Class ClassFromMethodParam(Method method, int index) {
            if (!method) return Class{};
            auto* e = GetExports();
            if (!e || !e->m_helperGetMethodParamType || !e->m_helperClassFromType) return Class{};
            void* paramType = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uint32_t)>(
                e->m_helperGetMethodParamType)(method.raw(), static_cast<uint32_t>(index));
            if (!paramType || !LooksLikeIl2CppType(paramType)) return Class{};
            return Class{ SafeCallClassFromType(e->m_helperClassFromType, paramType) };
        }

        // Property-side candidate names. Property::type_name() already goes
        // through the deobf map; the getter's return class covers the cases
        // where the property has no usable declared-type string.
        inline std::vector<std::string> CollectPropertyTypeNames(Property prop, Class getterClass) {
            std::vector<std::string> names;
            names.reserve(8);
            if (const char* n = prop.type_name())     names.emplace_back(n);
            if (const char* n = prop.raw_type_name()) names.emplace_back(n);
            if (getterClass) {
                names.emplace_back(getterClass.name());
                names.emplace_back(getterClass.raw_name());
                names.push_back(getterClass.full_name());
                names.push_back(getterClass.raw_full_name());
            }
            return names;
        }

        // SEH-protected attrs read — some obfuscated classes have garbage
        // type pointers; accessing type+0x08 on them AVs. Factored to a
        // noexcept helper so __try can be used (validate() has C++ objects
        // that would otherwise block SEH).
        inline uint16_t SafeReadTypeAttrs(const void* typePtr) noexcept {
            if (!typePtr) return 0;
            auto p = reinterpret_cast<uintptr_t>(typePtr);
            if (p < 0x10000 || p >= 0x7FFFFFFFFFFFull) return 0;
            __try {
                return *reinterpret_cast<const uint16_t*>(
                    static_cast<const char*>(typePtr) + 0x08);
            } __except(1) {
                return 0;
            }
        }

        inline bool is_field_static(Field field) {
            if (!field) return false;
            Type t = field.type();
            if (!t || !t.raw()) return false;
            uint16_t attrs = SafeReadTypeAttrs(t.raw());
            return (attrs & FIELD_ATTRIBUTE_STATIC) != 0;
        }

        inline bool str_eq(const char* a, const char* b) {
            if (!a || !b) return a == b;
            return std::strcmp(a, b) == 0;
        }
    }


    class FieldQuery {
        friend class ClassResolver;

        Class       m_targetType;
        std::string m_typeName;
        std::string m_name;
        std::optional<bool> m_static;
        bool        m_selfType = false;
        bool        m_hasTypeFilter = false;
        bool        m_required = false;

        int*        m_offsetDest = nullptr;
        void**      m_ptrDest = nullptr;       // raw il2cppFieldInfo*
        void**      m_fieldRawDest = nullptr;   // raw il2cppFieldInfo*
        std::string* m_nameDest = nullptr;
        std::string m_deobfPrefix;
        std::string m_deobfName;
        std::string m_label;

        Field       m_matched;

    public:
        /// <summary>Human-readable name for miss reporting (defaults to the deobf name).</summary>
        FieldQuery& label(std::string l) {
            m_label = std::move(l);
            return *this;
        }

        [[nodiscard]] std::string describe() const {
            if (!m_label.empty()) return m_label;
            if (!m_deobfName.empty()) return m_deobfName;
            if (!m_name.empty()) return m_name;
            if (!m_typeName.empty()) return "field:" + m_typeName;
            if (m_selfType) return "field:self";
            if (m_hasTypeFilter) return std::string("field:") +
                (m_targetType ? m_targetType.raw_name() : "<null-type>");
            return "field:?";
        }

        /// <summary>Match fields whose type class matches exactly.</summary>
        FieldQuery& byType(Class t) {
            m_targetType = t;
            m_hasTypeFilter = true;
            return *this;
        }

        /// <summary>Match fields whose type class name matches.</summary>
        FieldQuery& byTypeName(std::string_view n) {
            m_typeName = n;
            return *this;
        }

        /// <summary>Match fields by name.</summary>
        FieldQuery& byName(std::string_view n) {
            m_name = n;
            return *this;
        }

        /// <summary>Only match static fields.</summary>
        FieldQuery& isStatic() {
            m_static = true;
            return *this;
        }

        /// <summary>Only match non-static (instance) fields.</summary>
        FieldQuery& notStatic() {
            m_static = false;
            return *this;
        }

        /// <summary>Only match fields whose type equals the enclosing class.</summary>
        FieldQuery& isSelf() {
            m_selfType = true;
            return *this;
        }

        /// <summary>Mark this query as required -- resolve() fails if not matched.</summary>
        FieldQuery& required() {
            m_required = true;
            return *this;
        }

        /// <summary>Capture the matched field's offset.</summary>
        FieldQuery& toOffset(int& dest) {
            m_offsetDest = &dest;
            return *this;
        }

        /// <summary>Capture the raw il2cppFieldInfo* pointer.</summary>
        FieldQuery& toPtr(void*& dest) {
            m_ptrDest = &dest;
            return *this;
        }

        /// <summary>Capture the raw il2cppFieldInfo* pointer (alias).</summary>
        FieldQuery& toFieldRaw(void*& dest) {
            m_fieldRawDest = &dest;
            return *this;
        }

        /// <summary>Capture the matched field's name.</summary>
        FieldQuery& toName(std::string& dest) {
            m_nameDest = &dest;
            return *this;
        }

        /// <summary>Register a deobfuscation mapping when this field matches.</summary>
        FieldQuery& deobfuscate(std::string prefix, std::string name) {
            m_deobfPrefix = std::move(prefix);
            m_deobfName = std::move(name);
            return *this;
        }

        [[nodiscard]] bool matches(Class klass, const detail::FieldMatchCtx& f) const {
            if (m_matched) return false;
            if (!m_name.empty() && !detail::str_eq(f.name, m_name.c_str())) return false;
            if (m_static.has_value() && m_static.value() != f.isStatic) return false;
            if (m_hasTypeFilter && !detail::FieldMatchesTargetClass(f, m_targetType)) return false;
            if (m_selfType && !detail::FieldMatchesTargetClass(f, klass)) return false;
            if (!m_typeName.empty() && !detail::FieldTypeNameMatches(f, m_typeName)) return false;
            return true;
        }

        [[nodiscard]] bool valid() const { return !m_required || static_cast<bool>(m_matched); }
        [[nodiscard]] bool matched() const { return static_cast<bool>(m_matched); }

        void capture(Field field) { m_matched = field; }

        void apply() const {
            if (!m_matched) return;
            if (m_offsetDest)   *m_offsetDest = m_matched.offset();
            if (m_ptrDest)      *m_ptrDest = m_matched.raw();
            if (m_fieldRawDest) *m_fieldRawDest = m_matched.raw();
            if (m_nameDest)     *m_nameDest = m_matched.name();  // Returns deobfuscated
            if (!m_deobfName.empty()) {
                detail::AddNamedMapping(m_deobfPrefix, m_matched.raw_name(), m_deobfName);
            }
        }

        void reset() { m_matched = Field{}; }
    };


    class MethodQuery {
        friend class ClassResolver;

        std::string m_name;
        int         m_paramCount = -1;
        std::vector<std::pair<int, Class>> m_paramTypes;
        Class       m_returnType;
        bool        m_hasReturnFilter = false;
        bool        m_collectAll = false;
        bool        m_required = false;
        std::string m_deobfPrefix;
        std::string m_deobfName;
        std::string m_label;

        void**      m_ptrDest = nullptr;
        void**      m_methodRawDest = nullptr;
        std::vector<void*>* m_ptrListDest = nullptr;

        std::vector<Method> m_matched;

    public:
        /// <summary>Human-readable name for miss reporting (defaults to the deobf name).</summary>
        MethodQuery& label(std::string l) {
            m_label = std::move(l);
            return *this;
        }

        [[nodiscard]] std::string describe() const {
            if (!m_label.empty()) return m_label;
            if (!m_deobfName.empty()) return m_deobfName;
            if (!m_name.empty()) return m_name;
            return "method:?";
        }

        /// <summary>Match methods by name.</summary>
        MethodQuery& byName(std::string_view n) {
            m_name = n;
            return *this;
        }

        /// <summary>Match methods with exactly N parameters.</summary>
        MethodQuery& withParams(int count) {
            m_paramCount = count;
            return *this;
        }

        /// <summary>Match methods where parameter at `index` has the given type class. Call it
        /// once per parameter to constrain; every filter added has to hold.</summary>
        MethodQuery& paramType(int index, Class type) {
            m_paramTypes.emplace_back(index, type);
            return *this;
        }

        /// <summary>Match methods whose return type equals the given class.</summary>
        MethodQuery& returnType(Class type) {
            m_returnType = type;
            m_hasReturnFilter = true;
            return *this;
        }

        /// <summary>Collect all matching methods (instead of just the first).</summary>
        MethodQuery& collectAll() {
            m_collectAll = true;
            return *this;
        }

        /// <summary>Mark this query as required -- resolve() fails if not matched.</summary>
        MethodQuery& required() {
            m_required = true;
            return *this;
        }

        /// <summary>Capture the first matched method's function pointer.</summary>
        MethodQuery& toPtr(void*& dest) {
            m_ptrDest = &dest;
            return *this;
        }

        /// <summary>Capture the raw il2cppMethodInfo* pointer of the first match.</summary>
        MethodQuery& toMethodRaw(void*& dest) {
            m_methodRawDest = &dest;
            return *this;
        }

        /// <summary>Collect all matched method pointers into a vector.</summary>
        MethodQuery& toPtrList(std::vector<void*>& dest) {
            m_ptrListDest = &dest;
            m_collectAll = true;
            return *this;
        }

        /// <summary>Register a deobfuscation mapping for the first matched method.</summary>
        MethodQuery& deobfuscate(std::string prefix, std::string name) {
            m_deobfPrefix = std::move(prefix);
            m_deobfName = std::move(name);
            return *this;
        }

        [[nodiscard]] bool matches(Method method) const {
            if (!m_collectAll && !m_matched.empty()) return false;
            if (!m_name.empty() && !detail::str_eq(method.name(), m_name.c_str())) return false;
            if (m_paramCount >= 0 && static_cast<int>(method.param_count()) != m_paramCount) return false;
            for (const auto& [index, type] : m_paramTypes) {
                Class paramClass = detail::ClassFromMethodParam(method, index);
                if (!paramClass || paramClass.raw() != type.raw()) return false;
            }
            if (m_hasReturnFilter) {
                Class retClass = detail::ClassFromMethodReturnType(method);
                if (!retClass || retClass.raw() != m_returnType.raw()) return false;
            }
            return true;
        }

        [[nodiscard]] bool valid() const { return !m_required || !m_matched.empty(); }
        [[nodiscard]] bool matched() const { return !m_matched.empty(); }

        void capture(Method method) { m_matched.push_back(method); }

        void apply() const {
            if (!m_matched.empty()) {
                if (m_ptrDest)      *m_ptrDest = m_matched[0].pointer();
                if (m_methodRawDest)*m_methodRawDest = m_matched[0].raw();
                if (!m_deobfName.empty()) {
                    detail::AddNamedMapping(m_deobfPrefix, m_matched[0].raw_name(), m_deobfName);
                }
            }
            if (m_ptrListDest) {
                m_ptrListDest->clear();
                for (auto& m : m_matched)
                    m_ptrListDest->push_back(m.pointer());
            }
        }

        void reset() { m_matched.clear(); }
    };


    class PropertyQuery {
        friend class ClassResolver;

        Class       m_targetType;
        bool        m_hasTypeFilter = false;
        std::string m_typeName;
        std::string m_name;
        int         m_index = 0;        // nth property matching the filters
        bool        m_requireSetter = false;
        bool        m_requireGetter = true;
        std::optional<bool> m_static;
        bool        m_required = false;
        std::string m_deobfPrefix;
        std::string m_deobfName;
        std::string m_label;

        void**      m_getterPtrDest = nullptr;   // native code pointer
        void**      m_setterPtrDest = nullptr;
        void**      m_getterRawDest = nullptr;   // il2cppMethodInfo*
        void**      m_setterRawDest = nullptr;
        std::string* m_nameDest = nullptr;

        int         m_seen = 0;
        Property    m_matched;

    public:
        /// <summary>Match properties whose type class matches.</summary>
        PropertyQuery& byType(Class t) {
            m_targetType = t;
            m_hasTypeFilter = true;
            return *this;
        }

        /// <summary>Match properties whose type name matches.</summary>
        PropertyQuery& byTypeName(std::string_view n) {
            m_typeName = n;
            return *this;
        }

        /// <summary>Match by property name (only useful for non-obfuscated names).</summary>
        PropertyQuery& byName(std::string_view n) {
            m_name = n;
            return *this;
        }

        /// <summary>Take the nth property matching the filters (declaration order).</summary>
        PropertyQuery& atIndex(int i) {
            m_index = i;
            return *this;
        }

        /// <summary>Only match static properties. Read off the accessor.</summary>
        PropertyQuery& isStatic() {
            m_static = true;
            return *this;
        }

        /// <summary>Only match instance properties.</summary>
        PropertyQuery& notStatic() {
            m_static = false;
            return *this;
        }

        /// <summary>Only match properties that have a setter.</summary>
        PropertyQuery& hasSetter() {
            m_requireSetter = true;
            return *this;
        }

        /// <summary>Also match write-only properties (no getter).</summary>
        PropertyQuery& allowNoGetter() {
            m_requireGetter = false;
            return *this;
        }

        PropertyQuery& required() {
            m_required = true;
            return *this;
        }

        PropertyQuery& toGetter(void*& dest) { m_getterPtrDest = &dest; return *this; }
        PropertyQuery& toSetter(void*& dest) { m_setterPtrDest = &dest; return *this; }
        PropertyQuery& toGetterRaw(void*& dest) { m_getterRawDest = &dest; return *this; }
        PropertyQuery& toSetterRaw(void*& dest) { m_setterRawDest = &dest; return *this; }
        PropertyQuery& toName(std::string& dest) { m_nameDest = &dest; return *this; }

        /// <summary>Register a deobfuscation mapping when this property matches.</summary>
        PropertyQuery& deobfuscate(std::string prefix, std::string name) {
            m_deobfPrefix = std::move(prefix);
            m_deobfName = std::move(name);
            return *this;
        }

        PropertyQuery& label(std::string l) {
            m_label = std::move(l);
            return *this;
        }

        [[nodiscard]] std::string describe() const {
            if (!m_label.empty()) return m_label;
            if (!m_deobfName.empty()) return m_deobfName;
            if (!m_name.empty()) return m_name;
            if (!m_typeName.empty()) return "prop:" + m_typeName + "[" + std::to_string(m_index) + "]";
            return "prop:?";
        }

        [[nodiscard]] bool matches(Property prop, Class getterClass) const {
            if (m_matched) return false;
            if (m_requireGetter && !prop.getter()) return false;
            if (m_requireSetter && !prop.setter()) return false;
            if (m_static.has_value()) {
                // A property has no flags of its own; its accessors carry them.
                Method accessor = prop.getter() ? prop.getter() : prop.setter();
                if (!accessor || accessor.is_static() != m_static.value()) return false;
            }
            if (!m_name.empty() && !detail::str_eq(prop.name(), m_name.c_str())) return false;
            if (m_hasTypeFilter) {
                if (!m_targetType) return false;
                if (getterClass && getterClass.raw() == m_targetType.raw()) return true;
                std::vector<std::string> names = detail::CollectPropertyTypeNames(prop, getterClass);
                if (detail::FieldTypeNamesContain(names, m_targetType.name())) return true;
                if (detail::FieldTypeNamesContain(names, m_targetType.raw_name())) return true;
                std::string full = m_targetType.full_name();
                if (!full.empty() && detail::FieldTypeNamesContain(names, full)) return true;
                return false;
            }
            if (!m_typeName.empty()) {
                std::vector<std::string> names = detail::CollectPropertyTypeNames(prop, getterClass);
                if (!detail::FieldTypeNamesContain(names, m_typeName)) return false;
            }
            return true;
        }

        void capture(Property prop) {
            if (m_seen++ == m_index) m_matched = prop;
        }

        [[nodiscard]] bool valid() const { return !m_required || static_cast<bool>(m_matched); }
        [[nodiscard]] bool matched() const { return static_cast<bool>(m_matched); }

        void apply() const {
            if (!m_matched) return;
            Method getter = m_matched.getter();
            Method setter = m_matched.setter();
            if (m_getterPtrDest && getter) *m_getterPtrDest = getter.pointer();
            if (m_setterPtrDest && setter) *m_setterPtrDest = setter.pointer();
            if (m_getterRawDest && getter) *m_getterRawDest = getter.raw();
            if (m_setterRawDest && setter) *m_setterRawDest = setter.raw();
            if (m_nameDest)                *m_nameDest = m_matched.name();
            if (!m_deobfName.empty()) {
                detail::AddNamedMapping(m_deobfPrefix, m_matched.raw_name(), m_deobfName);
                // Accessors carry their own obfuscated names -- map them too so
                // dumps show get_X/set_X instead of Method_Public_...
                if (getter)
                    detail::AddNamedMapping(m_deobfPrefix, getter.raw_name(), "get_" + m_deobfName);
                if (setter)
                    detail::AddNamedMapping(m_deobfPrefix, setter.raw_name(), "set_" + m_deobfName);
            }
        }

        void reset() { m_matched = Property{}; m_seen = 0; }
    };


    class FieldCounter {
        friend class ClassResolver;

        Class       m_targetType;
        bool        m_hasTypeFilter = false;
        std::string m_typeName;
        std::optional<bool> m_static;
        int         m_count = 0;
        int         m_expectedMin = -1;
        int         m_expectedMax = -1;
        int         m_expectedExact = -1;

    public:
        FieldCounter& byType(Class t) {
            m_targetType = t;
            m_hasTypeFilter = true;
            return *this;
        }

        FieldCounter& byTypeName(std::string_view n) {
            m_typeName = n;
            return *this;
        }

        FieldCounter& isStatic() { m_static = true; return *this; }
        FieldCounter& notStatic() { m_static = false; return *this; }

        FieldCounter& expectExact(int n) { m_expectedExact = n; return *this; }
        FieldCounter& expectMin(int n)   { m_expectedMin = n; return *this; }
        FieldCounter& expectMax(int n)   { m_expectedMax = n; return *this; }
        FieldCounter& expectRange(int min, int max) {
            m_expectedMin = min;
            m_expectedMax = max;
            return *this;
        }

        [[nodiscard]] bool matches(const detail::FieldMatchCtx& f) const {
            if (m_static.has_value() && m_static.value() != f.isStatic) return false;
            if (m_hasTypeFilter && !detail::FieldMatchesTargetClass(f, m_targetType)) return false;
            if (!m_typeName.empty() && !detail::FieldTypeNameMatches(f, m_typeName)) return false;
            return true;
        }

        void increment() { ++m_count; }

        [[nodiscard]] bool valid() const {
            if (m_expectedExact >= 0 && m_count != m_expectedExact) return false;
            if (m_expectedMin >= 0 && m_count < m_expectedMin) return false;
            if (m_expectedMax >= 0 && m_count > m_expectedMax) return false;
            return true;
        }

        [[nodiscard]] int count() const { return m_count; }
        void reset() { m_count = 0; }

        [[nodiscard]] std::string describe() const {
            std::string what = !m_typeName.empty() ? m_typeName
                : (m_targetType ? std::string(m_targetType.raw_name()) : std::string("?"));
            return "count:" + what + "=" + std::to_string(m_count);
        }
    };


    class IndexedFieldCollector {
        friend class ClassResolver;

        Class       m_targetType;
        bool        m_hasTypeFilter = false;
        std::string m_typeName;
        std::optional<bool> m_static;
        int         m_requiredCount = 0;

        struct Binding {
            int         index;
            int*        offsetDest = nullptr;
            void**      ptrDest = nullptr;
            void**      fieldRawDest = nullptr;
            std::string* nameDest = nullptr;
            std::string deobfPrefix;
            std::string deobfName;
        };
        std::vector<Binding> m_bindings;
        std::vector<Field>   m_captured;

    public:
        IndexedFieldCollector& byType(Class t) {
            m_targetType = t;
            m_hasTypeFilter = true;
            return *this;
        }

        IndexedFieldCollector& byTypeName(std::string_view n) {
            m_typeName = n;
            return *this;
        }

        IndexedFieldCollector& isStatic() { m_static = true; return *this; }
        IndexedFieldCollector& notStatic() { m_static = false; return *this; }

        IndexedFieldCollector& requireCount(int n) {
            m_requiredCount = n;
            return *this;
        }

        IndexedFieldCollector& requireMinCount(int n) {
            m_requiredCount = n;
            return *this;
        }

        IndexedFieldCollector& bindOffset(int index, int& offsetDest) {
            m_bindings.push_back({ .index = index, .offsetDest = &offsetDest });
            return *this;
        }

        IndexedFieldCollector& bindPtr(int index, void*& ptrDest) {
            m_bindings.push_back({ .index = index, .ptrDest = &ptrDest });
            return *this;
        }

        IndexedFieldCollector& bindFieldRaw(int index, void*& dest) {
            m_bindings.push_back({ .index = index, .fieldRawDest = &dest });
            return *this;
        }

        IndexedFieldCollector& bind(int index, int& offsetDest, void*& ptrDest) {
            m_bindings.push_back({ .index = index, .offsetDest = &offsetDest, .ptrDest = &ptrDest });
            return *this;
        }

        IndexedFieldCollector& bind(int index, int& offsetDest, std::string& nameDest) {
            m_bindings.push_back({ .index = index, .offsetDest = &offsetDest, .nameDest = &nameDest });
            return *this;
        }

        IndexedFieldCollector& bind(int index, void*& ptrDest, std::string& nameDest) {
            m_bindings.push_back({ .index = index, .ptrDest = &ptrDest, .nameDest = &nameDest });
            return *this;
        }

        IndexedFieldCollector& bindWithDeobf(int index, int& offsetDest, std::string deobfPrefix, std::string deobfName) {
            m_bindings.push_back({ .index = index, .offsetDest = &offsetDest,
                .deobfPrefix = std::move(deobfPrefix), .deobfName = std::move(deobfName) });
            return *this;
        }

        IndexedFieldCollector& bindWithDeobf(int index, void*& ptrDest, std::string deobfPrefix, std::string deobfName) {
            m_bindings.push_back({ .index = index, .ptrDest = &ptrDest,
                .deobfPrefix = std::move(deobfPrefix), .deobfName = std::move(deobfName) });
            return *this;
        }

        IndexedFieldCollector& deobfuscate(int index, std::string deobfPrefix, std::string deobfName) {
            m_bindings.push_back({ .index = index, 
                .deobfPrefix = std::move(deobfPrefix), .deobfName = std::move(deobfName) });
            return *this;
        }

        [[nodiscard]] bool matches(const detail::FieldMatchCtx& f) const {
            if (m_static.has_value() && m_static.value() != f.isStatic) return false;
            if (m_hasTypeFilter && !detail::FieldMatchesTargetClass(f, m_targetType)) return false;
            if (!m_typeName.empty() && !detail::FieldTypeNameMatches(f, m_typeName)) return false;
            return true;
        }

        void capture(Field field) { m_captured.push_back(field); }

        [[nodiscard]] bool valid() const {
            return m_requiredCount <= 0 || static_cast<int>(m_captured.size()) >= m_requiredCount;
        }

        [[nodiscard]] int collected() const { return static_cast<int>(m_captured.size()); }

        [[nodiscard]] std::string describe() const {
            std::string what = !m_typeName.empty() ? m_typeName
                : (m_targetType ? std::string(m_targetType.raw_name()) : std::string("?"));
            return "collect:" + what + " got " + std::to_string(m_captured.size()) +
                   "/" + std::to_string(m_requiredCount);
        }

        void apply() const {
            for (const auto& b : m_bindings) {
                if (b.index < 0 || b.index >= static_cast<int>(m_captured.size())) continue;
                auto& field = m_captured[b.index];
                if (b.offsetDest)   *b.offsetDest = field.offset();
                if (b.ptrDest)      *b.ptrDest = field.raw();
                if (b.fieldRawDest) *b.fieldRawDest = field.raw();
                if (b.nameDest)     *b.nameDest = field.name();  // Returns deobfuscated
                if (!b.deobfName.empty()) {
                    detail::AddNamedMapping(b.deobfPrefix, field.raw_name(), b.deobfName);
                }
            }
        }

        void reset() { m_captured.clear(); }
    };


    class ClassResolver {
        Class                               m_klass;
        std::vector<FieldQuery>             m_fieldQueries;
        std::vector<MethodQuery>            m_methodQueries;
        std::vector<PropertyQuery>          m_propertyQueries;
        std::vector<FieldCounter>           m_fieldCounters;
        std::vector<IndexedFieldCollector>  m_indexedCollectors;
        std::string                         m_deobfName;
        std::vector<std::string>            m_misses;
        bool                                m_includeInherited = false;

    public:
        /// <summary>Construct a resolver for the given class handle.</summary>
        explicit ClassResolver(Class klass) : m_klass(klass) {}

        /// <summary>Construct from a raw il2cppClass* pointer.</summary>
        explicit ClassResolver(void* rawClass) : m_klass(Class{ rawClass }) {}

        /// <summary>Add a field query -- returns a reference to configure it fluently.</summary>
        FieldQuery& field() { return m_fieldQueries.emplace_back(); }

        /// <summary>Add a method query -- returns a reference to configure it fluently.</summary>
        MethodQuery& method() { return m_methodQueries.emplace_back(); }

        /// <summary>Add a property query -- returns a reference to configure it fluently.</summary>
        PropertyQuery& property() { return m_propertyQueries.emplace_back(); }

        /// <summary>Add a field counter -- returns a reference to configure it fluently.</summary>
        FieldCounter& counter() { return m_fieldCounters.emplace_back(); }

        /// <summary>Add an indexed field collector -- returns a reference to configure it fluently.</summary>
        IndexedFieldCollector& collector() { return m_indexedCollectors.emplace_back(); }

        /// <summary>Also walk base classes when matching fields. get_fields()
        /// returns declared fields only, so an inherited member (PeerBase's
        /// roundTripTime on PhotonPeer, for one) is invisible without this.</summary>
        ClassResolver& includeInherited(bool enable = true) {
            m_includeInherited = enable;
            return *this;
        }

        /// <summary>Set a deobfuscation name for the class itself.</summary>
        ClassResolver& deobfuscate(std::string name) {
            m_deobfName = std::move(name);
            return *this;
        }

        /// <summary>Reset all query match state for re-validation.</summary>
        void reset() {
            for (auto& q : m_fieldQueries) q.reset();
            for (auto& q : m_methodQueries) q.reset();
            for (auto& q : m_propertyQueries) q.reset();
            for (auto& c : m_fieldCounters) c.reset();
            for (auto& c : m_indexedCollectors) c.reset();
            m_misses.clear();
        }

        /// <summary>Validate all queries against the class. Returns true if all required queries match.</summary>
        /// <param name="collectMisses">Keep going after the first failed constraint and record
        /// every unsatisfied one in misses(). Off by default -- the class-scan loops
        /// validate() thousands of candidates and want the early-out.</param>
        [[nodiscard]] bool validate(bool collectMisses = false) {
            if (!m_klass) return false;

            reset();

            // Trace entry so the last printed class pinpoints whichever one
            // crashes during field/method iteration. raw_name() is a direct
            // struct read — safe even when the stable-name helper returns
            // garbage (which is the current suspected crash cause).
            const bool trace = g_resolverTrace != nullptr;
            if (trace) {
                char buf[256];
                snprintf(buf, sizeof(buf),
                    "validate klass=%p raw='%s' fQ=%zu mQ=%zu ctr=%zu coll=%zu",
                    m_klass.raw(), m_klass.raw_name(),
                    m_fieldQueries.size(), m_methodQueries.size(),
                    m_fieldCounters.size(), m_indexedCollectors.size());
                g_resolverTrace(buf);
            }

            // Declared fields first, then each base class in turn when asked --
            // that order keeps index-based collectors stable for the common case.
            bool needsStatic = false, needsFieldClass = false, needsName = false;
            for (const auto& q : m_fieldQueries) {
                needsStatic     |= q.m_static.has_value();
                needsFieldClass |= q.m_hasTypeFilter || q.m_selfType || !q.m_typeName.empty();
                needsName       |= !q.m_name.empty();
            }
            for (const auto& c : m_fieldCounters) {
                needsStatic     |= c.m_static.has_value();
                needsFieldClass |= c.m_hasTypeFilter || !c.m_typeName.empty();
            }
            for (const auto& c : m_indexedCollectors) {
                needsStatic     |= c.m_static.has_value();
                needsFieldClass |= c.m_hasTypeFilter || !c.m_typeName.empty();
            }

            int depth = 0;
            for (Class cur = m_klass; cur && depth < 64; cur = m_includeInherited ? cur.parent() : Class{}, ++depth) {
                auto fields = cur.get_fields();
                for (auto& field : fields) {
                    detail::FieldMatchCtx f;
                    f.field = field;
                    if (needsFieldClass) f.fieldClass = detail::ClassFromFieldType(field);
                    if (needsStatic)     f.isStatic = detail::is_field_static(field);
                    if (needsName)       f.name = field.name();

                    for (auto& counter : m_fieldCounters)
                        if (counter.matches(f))
                            counter.increment();

                    for (auto& coll : m_indexedCollectors)
                        if (coll.matches(f))
                            coll.capture(field);

                    for (auto& query : m_fieldQueries)
                        if (query.matches(m_klass, f))
                            query.capture(field);
                }
            }

            // Field enumeration comes up empty on a class whose fields the runtime has
            // not materialised yet; the by-name export answers anyway, so a named query
            // that found nothing gets one direct lookup before it is called a miss.
            for (auto& query : m_fieldQueries) {
                if (query.matched() || query.m_name.empty()) continue;
                Class cur = m_klass;
                for (int lookupDepth = 0; cur && lookupDepth < 64; ++lookupDepth) {
                    if (Field f = cur.get_field(query.m_name)) {
                        detail::FieldMatchCtx ctx;
                        ctx.field = f;
                        ctx.fieldClass = detail::ClassFromFieldType(f);
                        ctx.isStatic = detail::is_field_static(f);
                        ctx.name = query.m_name.c_str();
                        if (query.matches(m_klass, ctx)) { query.capture(f); break; }
                    }
                    cur = m_includeInherited ? cur.parent() : Class{};
                }
            }

            bool ok = true;
            auto check = [&](bool valid, auto&& describe) {
                if (valid) return true;
                ok = false;
                if (collectMisses) m_misses.push_back(describe());
                return false;
            };

            for (const auto& c : m_fieldCounters)
                if (!check(c.valid(), [&] { return c.describe(); }) && !collectMisses) return false;
            for (const auto& c : m_indexedCollectors)
                if (!check(c.valid(), [&] { return c.describe(); }) && !collectMisses) return false;
            for (const auto& q : m_fieldQueries)
                if (!check(q.valid(), [&] { return q.describe(); }) && !collectMisses) return false;

            if (!m_propertyQueries.empty()) {
                auto props = m_klass.get_properties();
                for (auto& prop : props) {
                    if (!prop) continue;
                    Class getterClass = detail::ClassFromMethodReturnType(prop.getter());
                    for (auto& query : m_propertyQueries)
                        if (query.matches(prop, getterClass))
                            query.capture(prop);
                }
                for (const auto& q : m_propertyQueries)
                    if (!check(q.valid(), [&] { return q.describe(); }) && !collectMisses) return false;
            }

            if (!m_methodQueries.empty()) {
                if (g_resolverTrace) {
                    char buf[160];
                    snprintf(buf, sizeof(buf), "  iter methods klass=%p raw='%s'",
                        m_klass.raw(), m_klass.raw_name());
                    g_resolverTrace(buf);
                }
                auto methods = m_klass.get_methods();
                for (auto& method : methods)
                    for (auto& query : m_methodQueries)
                        if (query.matches(method))
                            query.capture(method);

                for (const auto& q : m_methodQueries)
                    if (!check(q.valid(), [&] { return q.describe(); }) && !collectMisses) return false;
            }

            return ok;
        }

        /// <summary>Apply all captured results (write to destination variables, register deobf mappings).</summary>
        void apply() {
            for (const auto& q : m_fieldQueries) q.apply();
            for (const auto& q : m_methodQueries) q.apply();
            for (const auto& q : m_propertyQueries) q.apply();
            for (const auto& c : m_indexedCollectors) c.apply();

            if (!m_deobfName.empty()) {
                detail::AddNamedMapping({}, m_klass.raw_name(), m_deobfName);
            }
        }

        /// <summary>Validate + apply in one call. Returns true if all required queries matched.</summary>
        [[nodiscard]] bool resolve() {
            if (!validate()) return false;
            apply();
            return true;
        }

        /// <summary>For blocks that already know they have the right class: apply everything
        /// that matched even when some required query missed, and record the misses.
        /// A single renamed field then costs one offset instead of the whole block.</summary>
        bool resolvePartial() {
            bool ok = validate(true);
            apply();
            return ok;
        }

        /// <summary>Unsatisfied constraints from the last resolvePartial()/validate(true).</summary>
        [[nodiscard]] const std::vector<std::string>& misses() const { return m_misses; }

        /// <summary>misses() joined for logging.</summary>
        [[nodiscard]] std::string missReport() const {
            std::string out;
            for (const auto& m : m_misses) {
                if (!out.empty()) out += ", ";
                out += m;
            }
            return out;
        }

        /// <summary>Get the underlying class handle.</summary>
        [[nodiscard]] Class klass() const { return m_klass; }

        /// <summary>Get the raw il2cppClass* pointer.</summary>
        [[nodiscard]] void* raw() const { return m_klass.raw(); }
    };

} // namespace IL2CPP::Module
