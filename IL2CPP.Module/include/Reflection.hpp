#pragma once
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace IL2CPP::Module {

    class Field;
    class Method;
    class Property;
    class Event;
    class Type;
    class ManagedObject;
    class Assembly;
    class Image;

    class Class;

    class Type {
        void* m_native = nullptr;
    public:
        explicit Type(void* raw = nullptr) : m_native(raw) {}

        [[nodiscard]] void* raw() const noexcept { return m_native; }
        [[nodiscard]] operator bool() const noexcept { return m_native != nullptr; }

        [[nodiscard]] const char* name() const;
        [[nodiscard]] const char* raw_name() const;
        [[nodiscard]] const char* ns() const;
        [[nodiscard]] const char* raw_ns() const;
        [[nodiscard]] std::string full_name() const;
        [[nodiscard]] std::string raw_full_name() const;
        [[nodiscard]] Class get_class() const;
        [[nodiscard]] int type_enum() const;
        [[nodiscard]] bool is_byref() const;
        [[nodiscard]] const char* assembly_qualified_name() const;
        [[nodiscard]] void* get_system_type_object() const;
        [[nodiscard]] uint32_t attributes() const;
        [[nodiscard]] bool equals(const Type& other) const;
        [[nodiscard]] Class get_class_or_element_class() const;
    };

    class Field {
        void* m_native = nullptr;
    public:
        explicit Field(void* raw = nullptr) : m_native(raw) {}

        [[nodiscard]] void* raw() const noexcept { return m_native; }
        [[nodiscard]] operator bool() const noexcept { return m_native != nullptr; }

        [[nodiscard]] const char* name() const;
        [[nodiscard]] const char* raw_name() const;
        [[nodiscard]] const char* type_name() const;
        [[nodiscard]] const char* raw_type_name() const;
        [[nodiscard]] Type type() const;
        [[nodiscard]] int offset() const;
        [[nodiscard]] uint32_t attributes() const;
        [[nodiscard]] uint32_t raw_flags() const;
        [[nodiscard]] uint32_t token() const;

        [[nodiscard]] bool is_static() const;
        [[nodiscard]] bool is_public() const;
        [[nodiscard]] bool is_private() const;
        [[nodiscard]] bool is_protected() const;
        [[nodiscard]] bool is_internal() const;
        [[nodiscard]] bool is_readonly() const;
        [[nodiscard]] bool is_const() const;
        [[nodiscard]] const char* access_modifier() const;

        void get_value(void* obj, void* out_value) const;
        /// <summary>Write an instance field through il2cpp_field_set_value, which applies the GC write barrier.</summary>
        /// <param name="obj">Object that owns the field.</param>
        /// <param name="value">For a value-type field, a pointer to the value. For a reference-type field
        /// (string, class, array), the object pointer itself, not its address.</param>
        void set_value(void* obj, void* value) const;
        void get_static_value(void* out_value) const;
        void set_static_value(void* value) const;
    };

    class Method {
        void* m_native = nullptr;
    public:
        explicit Method(void* raw = nullptr) : m_native(raw) {}

        [[nodiscard]] void* raw() const noexcept { return m_native; }
        [[nodiscard]] operator bool() const noexcept { return m_native != nullptr; }

        [[nodiscard]] const char* name() const;
        [[nodiscard]] const char* raw_name() const;
        [[nodiscard]] const char* return_type_name() const;
        [[nodiscard]] const char* raw_return_type_name() const;
        [[nodiscard]] void* pointer() const;
        [[nodiscard]] uint8_t param_count() const;
        [[nodiscard]] uint32_t flags(uint32_t* impl_flags = nullptr) const;
        [[nodiscard]] uint32_t token() const;
        [[nodiscard]] uint16_t slot() const;
        [[nodiscard]] void* reflection_object(void* reflected_class = nullptr) const;
        [[nodiscard]] Method virtual_for(void* object) const;
        [[nodiscard]] static Method from_reflection(void* reflection_method);

        [[nodiscard]] bool is_static() const;
        [[nodiscard]] bool is_public() const;
        [[nodiscard]] bool is_private() const;
        [[nodiscard]] bool is_protected() const;
        [[nodiscard]] bool is_internal() const;
        [[nodiscard]] bool is_virtual() const;
        [[nodiscard]] bool is_abstract() const;
        [[nodiscard]] bool is_sealed() const;
        [[nodiscard]] const char* access_modifier() const;

        [[nodiscard]] Type get_param_type(uint32_t index) const;
        [[nodiscard]] const char* get_param_name(uint32_t index) const;
        [[nodiscard]] Type return_type() const;

        void* invoke(void* obj, void** params, void** exc = nullptr) const;
    };

    class Property {
        void* m_native = nullptr;
    public:
        explicit Property(void* raw = nullptr) : m_native(raw) {}

        [[nodiscard]] void* raw() const noexcept { return m_native; }
        [[nodiscard]] operator bool() const noexcept { return m_native != nullptr; }

        [[nodiscard]] const char* name() const;
        [[nodiscard]] const char* raw_name() const;
        [[nodiscard]] const char* type_name() const;
        [[nodiscard]] const char* raw_type_name() const;
        [[nodiscard]] Method getter() const;
        [[nodiscard]] Method setter() const;
        [[nodiscard]] void* parent_class_raw() const;
        [[nodiscard]] uint32_t raw_flags() const;

        [[nodiscard]] bool is_public() const;
        [[nodiscard]] bool is_private() const;
        [[nodiscard]] bool is_readonly() const;
        [[nodiscard]] bool is_writeonly() const;
        [[nodiscard]] const char* access_modifier() const;
    };

    class Event {
        void* m_native = nullptr;
    public:
        explicit Event(void* raw = nullptr) : m_native(raw) {}

        [[nodiscard]] void* raw() const noexcept { return m_native; }
        [[nodiscard]] operator bool() const noexcept { return m_native != nullptr; }

        [[nodiscard]] const char* name() const;
        [[nodiscard]] Method add_method() const;
        [[nodiscard]] Method remove_method() const;
        [[nodiscard]] Method raise_method() const;
        [[nodiscard]] Class parent() const;
        [[nodiscard]] uint32_t token() const;
    };

    class Class {
        void* m_native = nullptr;
    public:
        explicit Class(void* raw = nullptr) : m_native(raw) {}

        [[nodiscard]] void* raw() const noexcept { return m_native; }
        [[nodiscard]] operator bool() const noexcept { return m_native != nullptr; }

        [[nodiscard]] const char* name() const;
        [[nodiscard]] const char* raw_name() const;
        [[nodiscard]] const char* ns() const;
        [[nodiscard]] const char* raw_ns() const;
        [[nodiscard]] std::string full_name() const;
        [[nodiscard]] std::string raw_full_name() const;
        [[nodiscard]] Class parent() const;
        [[nodiscard]] Type get_type() const;
        [[nodiscard]] bool is_subclass_of(const Class& parent) const;
        [[nodiscard]] bool is_assignable_from(const Class& candidate) const;
        [[nodiscard]] bool is_enum() const;
        [[nodiscard]] bool is_generic() const;
        [[nodiscard]] uint32_t instance_size() const;
        [[nodiscard]] int32_t value_size(uint32_t* alignment = nullptr) const;
        [[nodiscard]] bool has_references() const;
        [[nodiscard]] void* static_field_data() const;
        [[nodiscard]] std::vector<Class> get_interfaces() const;

        [[nodiscard]] Field get_field(std::string_view name) const;
        [[nodiscard]] std::vector<Field> get_fields() const;

        [[nodiscard]] Method get_method(std::string_view name, int argc = -1) const;
        [[nodiscard]] void* get_method_pointer(std::string_view name, int argc = -1) const;
        [[nodiscard]] std::vector<Method> get_methods() const;

        /// <summary>Fields declared here and on every base class, nearest first.</summary>
        /// <param name="maxDepth">Cap on how far up the chain to walk.</param>
        /// <returns>Each field paired with the full name of the class declaring it.</returns>
        [[nodiscard]] std::vector<std::pair<Field, std::string>> get_fields_deep(int maxDepth = 16) const;

        /// <summary>Methods declared here and on every base class, nearest first.</summary>
        /// <param name="maxDepth">Cap on how far up the chain to walk.</param>
        /// <returns>Each method paired with the full name of the class declaring it.</returns>
        [[nodiscard]] std::vector<std::pair<Method, std::string>> get_methods_deep(int maxDepth = 16) const;

        /// <summary>Find a field by name, walking base classes.</summary>
        [[nodiscard]] Field get_field_deep(std::string_view name, int maxDepth = 16) const;

        /// <summary>Every overload of a name, optionally filtered by arity.</summary>
        /// <param name="name">Method name.</param>
        /// <param name="argc">Parameter count, or -1 for any.</param>
        /// <param name="searchBases">Also collect overloads declared on base classes.</param>
        [[nodiscard]] std::vector<Method> get_overloads(std::string_view name, int argc = -1,
                                                        bool searchBases = true) const;

        /// <summary>Resolve one overload by its parameter type names.</summary>
        /// <param name="name">Method name.</param>
        /// <param name="paramTypes">One type name per parameter; matched against the
        /// declared type's short or full name.</param>
        /// <param name="searchBases">Also consider methods declared on base classes.</param>
        /// <returns>The single matching method, or an empty handle when none or several match.</returns>
        [[nodiscard]] Method get_method_by_signature(std::string_view name,
                                                     const std::vector<std::string>& paramTypes,
                                                     bool searchBases = true) const;

        /// <summary>The deobfuscated name, which full_name() stops one mapping hop short of.</summary>
        [[nodiscard]] std::string display_name() const;

        [[nodiscard]] Property get_property(std::string_view name) const;
        [[nodiscard]] std::vector<Property> get_properties() const;

        [[nodiscard]] std::vector<Event> get_events() const;

        [[nodiscard]] static Class find(std::string_view full_name);
        [[nodiscard]] static Class from_system_type(void* system_type);
        [[nodiscard]] ManagedObject new_object() const;

        /// <summary>Allocate an instance and invoke a matching .ctor. The "new" primitive.</summary>
        [[nodiscard]] ManagedObject new_instance(void** ctor_params = nullptr, int argc = 0) const;
    };

    class Image {
        void* m_native = nullptr;
    public:
        explicit Image(void* raw = nullptr) : m_native(raw) {}

        [[nodiscard]] void* raw() const noexcept { return m_native; }
        [[nodiscard]] operator bool() const noexcept { return m_native != nullptr; }

        [[nodiscard]] const char* name() const;
        [[nodiscard]] uint32_t class_count() const;
        [[nodiscard]] Class get_class(uint32_t index) const;
        [[nodiscard]] std::vector<Class> get_classes() const;
    };

    class Assembly {
        void* m_native = nullptr;
    public:
        explicit Assembly(void* raw = nullptr) : m_native(raw) {}

        [[nodiscard]] void* raw() const noexcept { return m_native; }
        [[nodiscard]] operator bool() const noexcept { return m_native != nullptr; }

        [[nodiscard]] Image get_image() const;
        [[nodiscard]] static std::vector<Assembly> get_all();
    };

} // namespace IL2CPP::Module
