#pragma once
#include "Reflection.hpp"
#include "Deobfuscation.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <vector>
#include <string>
#include <optional>
#include <cstring>

namespace IL2CPP::Module {

    class FieldQuery;
    class MethodQuery;
    class FieldCounter;
    class IndexedFieldCollector;
    class ClassResolver;


    namespace detail {
        inline Class ClassFromFieldType(Field field) {
            if (!field) return Class{};
            Type t = field.type();
            if (!t) return Class{};
            auto* e = GetExports();
            if (!e || !e->m_helperClassFromType) return Class{};
            void* klass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(
                e->m_helperClassFromType)(t.raw());
            return Class{ klass };
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

        inline bool is_field_static(Field field) {
            if (!field) return false;
            Type t = field.type();
            if (!t || !t.raw()) return false;
            uint16_t attrs = *reinterpret_cast<const uint16_t*>(
                static_cast<const char*>(t.raw()) + 0x8);
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
        /// Match fields whose type class matches exactly.
        FieldQuery& byType(Class t) {
            m_targetType = t;
            m_hasTypeFilter = true;
            return *this;
        }

        /// Match fields whose type class name matches.
        FieldQuery& byTypeName(std::string_view n) {
            m_typeName = n;
            return *this;
        }

        /// Match fields by name.
        FieldQuery& byName(std::string_view n) {
            m_name = n;
            return *this;
        }

        /// Only match static fields.
        FieldQuery& isStatic() {
            m_static = true;
            return *this;
        }

        /// Only match non-static (instance) fields.
        FieldQuery& notStatic() {
            m_static = false;
            return *this;
        }

        /// Only match fields whose type equals the enclosing class.
        FieldQuery& isSelf() {
            m_selfType = true;
            return *this;
        }

        /// Mark this query as required -- resolve() fails if not matched.
        FieldQuery& required() {
            m_required = true;
            return *this;
        }

        /// Capture the matched field's offset.
        FieldQuery& toOffset(int& dest) {
            m_offsetDest = &dest;
            return *this;
        }

        /// Capture the raw il2cppFieldInfo* pointer.
        FieldQuery& toPtr(void*& dest) {
            m_ptrDest = &dest;
            return *this;
        }

        /// Capture the raw il2cppFieldInfo* pointer (alias).
        FieldQuery& toFieldRaw(void*& dest) {
            m_fieldRawDest = &dest;
            return *this;
        }

        /// Capture the matched field's name.
        FieldQuery& toName(std::string& dest) {
            m_nameDest = &dest;
            return *this;
        }

        /// Register a deobfuscation mapping when this field matches.
        FieldQuery& deobfuscate(std::string prefix, std::string name) {
            m_deobfPrefix = std::move(prefix);
            m_deobfName = std::move(name);
            return *this;
        }

        [[nodiscard]] bool matches(Class klass, Field field, Class fieldClass, bool isStatic) const {
            if (m_matched) return false;
            if (m_hasTypeFilter && fieldClass.raw() != m_targetType.raw()) return false;
            if (m_selfType && fieldClass.raw() != klass.raw()) return false;
            if (!m_typeName.empty() && !detail::str_eq(fieldClass.name(), m_typeName.c_str())) return false;
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
        /// Match methods by name.
        MethodQuery& byName(std::string_view n) {
            m_name = n;
            return *this;
        }

        /// Match methods with exactly N parameters.
        MethodQuery& withParams(int count) {
            m_paramCount = count;
            return *this;
        }

        /// Match methods where parameter at `index` has the given type class.
        MethodQuery& paramType(int index, Class type) {
            m_paramIndex = index;
            m_paramType = type;
            m_hasParamTypeFilter = true;
            return *this;
        }

        /// Match methods whose return type equals the given class.
        MethodQuery& returnType(Class type) {
            m_returnType = type;
            m_hasReturnFilter = true;
            return *this;
        }

        /// Collect all matching methods (instead of just the first).
        MethodQuery& collectAll() {
            m_collectAll = true;
            return *this;
        }

        /// Mark this query as required -- resolve() fails if not matched.
        MethodQuery& required() {
            m_required = true;
            return *this;
        }

        /// Capture the first matched method's function pointer.
        MethodQuery& toPtr(void*& dest) {
            m_ptrDest = &dest;
            return *this;
        }

        /// Capture the raw il2cppMethodInfo* pointer of the first match.
        MethodQuery& toMethodRaw(void*& dest) {
            m_methodRawDest = &dest;
            return *this;
        }

        /// Collect all matched method pointers into a vector.
        MethodQuery& toPtrList(std::vector<void*>& dest) {
            m_ptrListDest = &dest;
            m_collectAll = true;
            return *this;
        }

        /// Register a deobfuscation mapping for the first matched method.
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

        [[nodiscard]] bool matches(Field, Class fieldClass, bool isStatic) const {
            if (m_hasTypeFilter && fieldClass.raw() != m_targetType.raw()) return false;
            if (!m_typeName.empty() && !detail::str_eq(fieldClass.name(), m_typeName.c_str())) return false;
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

        [[nodiscard]] bool matches(Field, Class fieldClass, bool isStatic) const {
            if (m_hasTypeFilter && fieldClass.raw() != m_targetType.raw()) return false;
            if (!m_typeName.empty() && !detail::str_eq(fieldClass.name(), m_typeName.c_str())) return false;
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
        /// Construct a resolver for the given class handle.
        explicit ClassResolver(Class klass) : m_klass(klass) {}

        /// Construct from a raw il2cppClass* pointer.
        explicit ClassResolver(void* rawClass) : m_klass(Class{ rawClass }) {}

        /// Add a field query -- returns a reference to configure it fluently.
        FieldQuery& field() { return m_fieldQueries.emplace_back(); }

        /// Add a method query -- returns a reference to configure it fluently.
        MethodQuery& method() { return m_methodQueries.emplace_back(); }

        /// Add a field counter -- returns a reference to configure it fluently.
        FieldCounter& counter() { return m_fieldCounters.emplace_back(); }

        /// Add an indexed field collector -- returns a reference to configure it fluently.
        IndexedFieldCollector& collector() { return m_indexedCollectors.emplace_back(); }

        /// Set a deobfuscation name for the class itself.
        ClassResolver& deobfuscate(std::string name) {
            m_deobfName = std::move(name);
            return *this;
        }

        /// Reset all query match state for re-validation.
        void reset() {
            for (auto& q : m_fieldQueries) q.reset();
            for (auto& q : m_methodQueries) q.reset();
            for (auto& c : m_fieldCounters) c.reset();
            for (auto& c : m_indexedCollectors) c.reset();
        }

        /// Validate all queries against the class. Returns true if all required queries match.
        [[nodiscard]] bool validate() {
            if (!m_klass) return false;

            reset();


            auto fields = m_klass.get_fields();
            for (auto& field : fields) {
                Class fieldClass = detail::ClassFromFieldType(field);
                if (!fieldClass) continue;

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

        /// Apply all captured results (write to destination variables, register deobf mappings).
        void apply() {
            for (const auto& q : m_fieldQueries) q.apply();
            for (const auto& q : m_methodQueries) q.apply();
            for (const auto& c : m_indexedCollectors) c.apply();

            if (!m_deobfName.empty()) {
                Deobfuscation::AddMapping(m_klass.raw_name(), m_deobfName);
            }
        }

        /// Validate + apply in one call. Returns true if all required queries matched.
        [[nodiscard]] bool resolve() {
            if (!validate()) return false;
            apply();
            return true;
        }

        /// Get the underlying class handle.
        [[nodiscard]] Class klass() const { return m_klass; }

        /// Get the raw il2cppClass* pointer.
        [[nodiscard]] void* raw() const { return m_klass.raw(); }
    };

} // namespace IL2CPP::Module
