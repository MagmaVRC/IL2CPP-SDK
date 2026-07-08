#pragma once
#include "Reflection.hpp"
#include "Deobfuscation.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <vector>
#include <string>
#include <optional>
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
    class FieldCounter;
    class IndexedFieldCollector;
    class ClassResolver;


    namespace detail {
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
        // Every full_name()/raw_full_name() heap-allocates, so building this
        // set once per field and reusing it across all expected-name
        // comparisons avoids the churn of rebuilding it per comparison.
        inline std::vector<std::string> CollectFieldTypeNames(Field field, Class fieldClass) {
            std::vector<std::string> names;
            names.reserve(10);
            if (fieldClass) {
                names.emplace_back(fieldClass.name());
                names.emplace_back(fieldClass.raw_name());
                names.push_back(fieldClass.full_name());
                names.push_back(fieldClass.raw_full_name());
            }
            names.emplace_back(field.type_name());
            names.emplace_back(field.raw_type_name());
            if (Type fieldType = field.type()) {
                names.emplace_back(fieldType.name());
                names.emplace_back(fieldType.raw_name());
                names.push_back(fieldType.full_name());
                names.push_back(fieldType.raw_full_name());
            }
            return names;
        }

        inline bool FieldTypeNamesContain(const std::vector<std::string>& names,
                                          std::string_view expected) {
            if (expected.empty()) return true;
            for (const std::string& n : names)
                if (TypeNameEquals(n, expected)) return true;
            return false;
        }

        inline bool FieldTypeNameMatches(Field field, Class fieldClass, std::string_view expected) {
            if (expected.empty()) return true;
            return FieldTypeNamesContain(CollectFieldTypeNames(field, fieldClass), expected);
        }

        inline bool FieldMatchesTargetClass(Field field, Class fieldClass, Class targetClass) {
            if (!targetClass) return false;
            if (fieldClass && fieldClass.raw() == targetClass.raw()) return true;

            std::vector<std::string> names = CollectFieldTypeNames(field, fieldClass);

            if (FieldTypeNamesContain(names, targetClass.name())) return true;
            if (FieldTypeNamesContain(names, targetClass.raw_name())) return true;

            std::string fullName = targetClass.full_name();
            if (!fullName.empty() && FieldTypeNamesContain(names, fullName)) return true;

            std::string rawFullName = targetClass.raw_full_name();
            if (!rawFullName.empty() && FieldTypeNamesContain(names, rawFullName)) return true;

            return false;
        }

        inline bool FieldMatchesSelfType(Field field, Class fieldClass, Class klass) {
            if (!klass) return false;
            if (fieldClass && fieldClass.raw() == klass.raw()) return true;

            std::vector<std::string> names = CollectFieldTypeNames(field, fieldClass);

            if (FieldTypeNamesContain(names, klass.name())) return true;
            if (FieldTypeNamesContain(names, klass.raw_name())) return true;

            std::string fullName = klass.full_name();
            if (!fullName.empty() && FieldTypeNamesContain(names, fullName)) return true;

            std::string rawFullName = klass.raw_full_name();
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
            auto* e = GetExports();
            if (!e || !e->m_helperClassFromType) return Class{};
            void* klass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(
                e->m_helperClassFromType)(t.raw());
            return Class{ klass };
        }

        inline Class ClassFromMethodParam(Method method, int index) {
            if (!method) return Class{};
            auto* e = GetExports();
            if (!e || !e->m_helperGetMethodParamType || !e->m_helperClassFromType) return Class{};
            void* paramType = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uint32_t)>(
                e->m_helperGetMethodParamType)(method.raw(), static_cast<uint32_t>(index));
            if (!paramType) return Class{};
            void* klass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(
                e->m_helperClassFromType)(paramType);
            return Class{ klass };
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

        Field       m_matched;

    public:
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

        [[nodiscard]] bool matches(Class klass, Field field, Class fieldClass, bool isStatic) const {
            if (m_matched) return false;
            if (m_hasTypeFilter && !detail::FieldMatchesTargetClass(field, fieldClass, m_targetType)) return false;
            if (m_selfType && !detail::FieldMatchesSelfType(field, fieldClass, klass)) return false;
            if (!m_typeName.empty() && !detail::FieldTypeNameMatches(field, fieldClass, m_typeName)) return false;
            if (!m_name.empty() && !detail::str_eq(field.name(), m_name.c_str())) return false;
            if (m_static.has_value() && m_static.value() != isStatic) return false;
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
                Deobfuscation::AddMapping(
                    m_deobfPrefix + m_matched.raw_name(), m_deobfName);
            }
        }

        void reset() { m_matched = Field{}; }
    };


    class MethodQuery {
        friend class ClassResolver;

        std::string m_name;
        int         m_paramCount = -1;
        Class       m_paramType;
        bool        m_hasParamTypeFilter = false;
        int         m_paramIndex = 0;
        Class       m_returnType;
        bool        m_hasReturnFilter = false;
        bool        m_collectAll = false;
        bool        m_required = false;
        std::string m_deobfPrefix;
        std::string m_deobfName;

        void**      m_ptrDest = nullptr;
        void**      m_methodRawDest = nullptr;
        std::vector<void*>* m_ptrListDest = nullptr;

        std::vector<Method> m_matched;

    public:
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

        /// <summary>Match methods where parameter at `index` has the given type class.</summary>
        MethodQuery& paramType(int index, Class type) {
            m_paramIndex = index;
            m_paramType = type;
            m_hasParamTypeFilter = true;
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
            if (m_hasParamTypeFilter) {
                Class paramClass = detail::ClassFromMethodParam(method, m_paramIndex);
                if (!paramClass || paramClass.raw() != m_paramType.raw()) return false;
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
                    Deobfuscation::AddMapping(
                        m_deobfPrefix + m_matched[0].raw_name(), m_deobfName);
                }
            }
            if (m_ptrListDest) {
                for (auto& m : m_matched)
                    m_ptrListDest->push_back(m.pointer());
            }
        }

        void reset() { m_matched.clear(); }
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

        [[nodiscard]] bool matches(Field field, Class fieldClass, bool isStatic) const {
            if (m_hasTypeFilter && !detail::FieldMatchesTargetClass(field, fieldClass, m_targetType)) return false;
            if (!m_typeName.empty() && !detail::FieldTypeNameMatches(field, fieldClass, m_typeName)) return false;
            if (m_static.has_value() && m_static.value() != isStatic) return false;
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

        [[nodiscard]] bool matches(Field field, Class fieldClass, bool isStatic) const {
            if (m_hasTypeFilter && !detail::FieldMatchesTargetClass(field, fieldClass, m_targetType)) return false;
            if (!m_typeName.empty() && !detail::FieldTypeNameMatches(field, fieldClass, m_typeName)) return false;
            if (m_static.has_value() && m_static.value() != isStatic) return false;
            return true;
        }

        void capture(Field field) { m_captured.push_back(field); }

        [[nodiscard]] bool valid() const {
            return m_requiredCount <= 0 || static_cast<int>(m_captured.size()) >= m_requiredCount;
        }

        [[nodiscard]] int collected() const { return static_cast<int>(m_captured.size()); }

        void apply() const {
            for (const auto& b : m_bindings) {
                if (b.index >= static_cast<int>(m_captured.size())) continue;
                auto& field = m_captured[b.index];
                if (b.offsetDest)   *b.offsetDest = field.offset();
                if (b.ptrDest)      *b.ptrDest = field.raw();
                if (b.fieldRawDest) *b.fieldRawDest = field.raw();
                if (b.nameDest)     *b.nameDest = field.name();  // Returns deobfuscated
                if (!b.deobfName.empty()) {
                    Deobfuscation::AddMapping(
                        b.deobfPrefix + field.raw_name(), b.deobfName);
                }
            }
        }

        void reset() { m_captured.clear(); }
    };


    class ClassResolver {
        Class                               m_klass;
        std::vector<FieldQuery>             m_fieldQueries;
        std::vector<MethodQuery>            m_methodQueries;
        std::vector<FieldCounter>           m_fieldCounters;
        std::vector<IndexedFieldCollector>  m_indexedCollectors;
        std::string                         m_deobfName;

    public:
        /// <summary>Construct a resolver for the given class handle.</summary>
        explicit ClassResolver(Class klass) : m_klass(klass) {}

        /// <summary>Construct from a raw il2cppClass* pointer.</summary>
        explicit ClassResolver(void* rawClass) : m_klass(Class{ rawClass }) {}

        /// <summary>Add a field query -- returns a reference to configure it fluently.</summary>
        FieldQuery& field() { return m_fieldQueries.emplace_back(); }

        /// <summary>Add a method query -- returns a reference to configure it fluently.</summary>
        MethodQuery& method() { return m_methodQueries.emplace_back(); }

        /// <summary>Add a field counter -- returns a reference to configure it fluently.</summary>
        FieldCounter& counter() { return m_fieldCounters.emplace_back(); }

        /// <summary>Add an indexed field collector -- returns a reference to configure it fluently.</summary>
        IndexedFieldCollector& collector() { return m_indexedCollectors.emplace_back(); }

        /// <summary>Set a deobfuscation name for the class itself.</summary>
        ClassResolver& deobfuscate(std::string name) {
            m_deobfName = std::move(name);
            return *this;
        }

        /// <summary>Reset all query match state for re-validation.</summary>
        void reset() {
            for (auto& q : m_fieldQueries) q.reset();
            for (auto& q : m_methodQueries) q.reset();
            for (auto& c : m_fieldCounters) c.reset();
            for (auto& c : m_indexedCollectors) c.reset();
        }

        /// <summary>Validate all queries against the class. Returns true if all required queries match.</summary>
        [[nodiscard]] bool validate() {
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

            auto fields = m_klass.get_fields();
            for (auto& field : fields) {
                Class fieldClass = detail::ClassFromFieldType(field);
                bool isStatic = detail::is_field_static(field);

                for (auto& counter : m_fieldCounters)
                    if (counter.matches(field, fieldClass, isStatic))
                        counter.increment();

                for (auto& coll : m_indexedCollectors)
                    if (coll.matches(field, fieldClass, isStatic))
                        coll.capture(field);

                for (auto& query : m_fieldQueries)
                    if (query.matches(m_klass, field, fieldClass, isStatic))
                        query.capture(field);
            }

            for (const auto& c : m_fieldCounters)
                if (!c.valid()) return false;
            for (const auto& c : m_indexedCollectors)
                if (!c.valid()) return false;
            for (const auto& q : m_fieldQueries)
                if (!q.valid()) return false;


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
                    if (!q.valid()) return false;
            }

            return true;
        }

        /// <summary>Apply all captured results (write to destination variables, register deobf mappings).</summary>
        void apply() {
            for (const auto& q : m_fieldQueries) q.apply();
            for (const auto& q : m_methodQueries) q.apply();
            for (const auto& c : m_indexedCollectors) c.apply();

            if (!m_deobfName.empty()) {
                Deobfuscation::AddMapping(m_klass.raw_name(), m_deobfName);
            }
        }

        /// <summary>Validate + apply in one call. Returns true if all required queries matched.</summary>
        [[nodiscard]] bool resolve() {
            if (!validate()) return false;
            apply();
            return true;
        }

        /// <summary>Get the underlying class handle.</summary>
        [[nodiscard]] Class klass() const { return m_klass; }

        /// <summary>Get the raw il2cppClass* pointer.</summary>
        [[nodiscard]] void* raw() const { return m_klass.raw(); }
    };

} // namespace IL2CPP::Module
