#pragma once
#include "Reflection.hpp"
#include <IL2CPP.Common/il2cpp_structs.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace IL2CPP::Module {
    [[nodiscard]] __forceinline bool IsValidPointer(void* ptr) noexcept {
        constexpr uintptr_t lo = 0x10000;
        constexpr uintptr_t range = 0x7FFFFFFF0000 - lo;
        return reinterpret_cast<uintptr_t>(ptr) - lo <= range;
    }

    class ManagedObject {
    protected:
        void* m_native = nullptr;  // il2cppObject*

        [[nodiscard]] bool valid() const noexcept {
            if (!IsValidPointer(m_native)) return false;
            return IsValidPointer(*reinterpret_cast<void**>(m_native));
        }

        [[nodiscard]] Class get_class_internal() const noexcept {
            return Class{ *reinterpret_cast<void**>(m_native) };
        }

    public:
        ManagedObject() = default;
        explicit ManagedObject(void* raw) : m_native(raw) {}


        /// <summary>Get the raw il2cppObject* pointer.</summary>
        [[nodiscard]] void* raw() const noexcept { return m_native; }

        /// <summary>Implicit bool conversion - check if the object is valid.</summary>
        [[nodiscard]] operator bool() const noexcept { return valid(); }

        /// <summary>Get the class handle for this object.</summary>
        [[nodiscard]] Class get_class() const noexcept {
            if (!valid()) return Class{};
            return get_class_internal();
        }


        template<typename T>
        [[nodiscard]] T get_field(int fieldOffset) const {
            if (!valid()) return T{};
            return *reinterpret_cast<T*>(static_cast<char*>(m_native) + fieldOffset);
        }

        /// <summary>Get a field value by name. Returns default T{} if not found.</summary>
        template<typename T>
        [[nodiscard]] T get_field(std::string_view name) const {
            if (!valid()) return T{};
            Class klass = get_class_internal();
            Field f = klass.get_field(name);
            if (!f) return T{};
            int off = f.offset();
            if (off >= 0) {
                return *reinterpret_cast<T*>(static_cast<char*>(m_native) + off);
            }
            return T{};
        }

        /// <summary>Get a field value using a pre-resolved Field handle (skips name lookup).</summary>
        template<typename T>
        [[nodiscard]] T get_field(const Field& field) const {
            if (!valid() || !field) return T{};
            int off = field.offset();
            if (off >= 0) {
                return *reinterpret_cast<T*>(static_cast<char*>(m_native) + off);
            }
            return T{};
        }

        /// <summary>Set a field value by name.</summary>
        template<typename T>
        void set_field(std::string_view name, T value) {
            if (!valid()) return;
            Class klass = get_class_internal();
            Field f = klass.get_field(name);
            if (!f) return;
            int off = f.offset();
            if (off >= 0) {
                *reinterpret_cast<T*>(static_cast<char*>(m_native) + off) = value;
            }
        }

        template<typename T>
        void set_field(int fieldOffset, T value) {
            if (!valid()) return;
            *reinterpret_cast<T*>(static_cast<char*>(m_native) + fieldOffset) = value;
        }

        /// <summary>Set a field value using a pre-resolved Field handle (skips name lookup).</summary>
        template<typename T>
        void set_field(const Field& field, T value) {
            if (!valid() || !field) return;
            int off = field.offset();
            if (off >= 0) {
                *reinterpret_cast<T*>(static_cast<char*>(m_native) + off) = value;
            }
        }

        /// <summary>Get a property value by name (calls the getter).</summary>
        template<typename T>
        [[nodiscard]] T get_property(std::string_view name) const {
            if (!valid()) return T{};
            Class klass = get_class_internal();
            Property prop = klass.get_property(name);
            if (!prop) return T{};
            Method getter = prop.getter();
            if (!getter) return T{};
            void* result = getter.invoke(m_native, nullptr);
            if (!result) return T{};
            if constexpr (std::is_pointer_v<T>) {
                return reinterpret_cast<T>(result);
            } else {
                return *reinterpret_cast<T*>(IL2CPP::Unbox(result));
            }
        }

        /// <summary>Set a property value by name (calls the setter).</summary>
        template<typename T>
        void set_property(std::string_view name, T value) {
            if (!valid()) return;
            Class klass = get_class_internal();
            Property prop = klass.get_property(name);
            if (!prop) return;
            Method setter = prop.setter();
            if (!setter) return;
            void* params[1];
            if constexpr (std::is_pointer_v<T>) {
                params[0] = reinterpret_cast<void*>(value);
            } else {
                params[0] = &value;
            }
            setter.invoke(m_native, params);
        }


        /// <summary>Read a value at a raw offset from the object base.</summary>
        template<typename T>
        [[nodiscard]] T read(int offset) const {
            if (!m_native || offset < 0) return T{};
            return *reinterpret_cast<T*>(static_cast<char*>(m_native) + offset);
        }

        /// <summary>Write a value at a raw offset from the object base.</summary>
        template<typename T>
        void write(int offset, T value) {
            if (!m_native || offset < 0) return;
            *reinterpret_cast<T*>(static_cast<char*>(m_native) + offset) = value;
        }

        /// <summary>Get a typed pointer at a raw offset from the object base.</summary>
        template<typename T = void*>
        [[nodiscard]] T* ptr_at(int offset) const {
            if (!m_native || offset < 0) return nullptr;
            return reinterpret_cast<T*>(static_cast<char*>(m_native) + offset);
        }


        /// <summary>Call a method by name with typed return. Uses runtime invoke.</summary>
        template<typename TReturn>
        TReturn call_method(std::string_view name, void** params = nullptr, int argc = -1) const {
            if constexpr (std::is_void_v<TReturn>) {
                if (!valid()) return;
                Class klass = get_class_internal();
                Method m = klass.get_method(name, argc);
                if (!m) return;
                m.invoke(m_native, params);
            }
            else {
                if (!valid()) return TReturn{};
                Class klass = get_class_internal();
                Method m = klass.get_method(name, argc);
                if (!m) return TReturn{};
                void* result = m.invoke(m_native, params);
                if (!result) return TReturn{};
                if constexpr (std::is_pointer_v<TReturn>) {
                    return reinterpret_cast<TReturn>(result);
                }
                else {
                    return *reinterpret_cast<TReturn*>(IL2CPP::Unbox(result));
                }
            }
        }

        /// <summary>Get a direct function pointer for a method, for fast calling.</summary>
        [[nodiscard]] void* get_method_pointer(std::string_view name, int argc = -1) const;

        /// <summary>Get a field handle by name.</summary>
        [[nodiscard]] Field get_field_info(std::string_view name) const;

        /// <summary>Get a method handle by name.</summary>
        [[nodiscard]] Method get_method_info(std::string_view name, int argc = -1) const;


        /// <summary>Get a static field value by name.</summary>
        template<typename T>
        [[nodiscard]] T get_static_field(std::string_view name) const {
            if (!valid()) return T{};
            Class klass = get_class_internal();
            Field f = klass.get_field(name);
            if (!f || !f.is_static()) return T{};
            T value{};
            f.get_static_value(&value);
            return value;
        }

        /// <summary>Set a static field value by name.</summary>
        template<typename T>
        void set_static_field(std::string_view name, T value) {
            if (!valid()) return;
            Class klass = get_class_internal();
            Field f = klass.get_field(name);
            if (!f || !f.is_static()) return;
            f.set_static_value(&value);
        }


        /// <summary>Check if a field with the given name exists on this object's class.</summary>
        [[nodiscard]] bool has_field(std::string_view name) const;

        /// <summary>Get the byte offset for a field by name. Returns -1 if not found.</summary>
        [[nodiscard]] int get_field_offset(std::string_view name) const;

        /// <summary>Reverse lookup: get the Field metadata for a field at a specific offset.</summary>
        [[nodiscard]] Field get_field_by_offset(int offset) const;

        /// <summary>Get the Nth field in declaration order (0-indexed).</summary>
        [[nodiscard]] Field get_field_by_index(int index) const;

        /// <summary>Find all fields whose type name matches the given string.</summary>
        [[nodiscard]] std::vector<Field> find_fields_by_type(std::string_view typeName) const;

        /// <summary>Find all fields matching an access modifier ("public", "private", "protected", "internal").</summary>
        [[nodiscard]] std::vector<Field> find_fields_by_access(std::string_view access) const;

        /// <summary>Find all fields matching a predicate.</summary>
        [[nodiscard]] std::vector<Field> find_fields(const std::function<bool(const Field&)>& predicate) const;


        /// <summary>Get all fields for this object's class (convenience wrapper).</summary>
        [[nodiscard]] std::vector<Field> get_fields() const;

        /// <summary>Iterate all fields with a callback. Returns early if the callback returns false.</summary>
        void for_each_field(const std::function<bool(const Field&)>& callback) const;

        /// <summary>Get a list of {name, offset} pairs for all instance fields.</summary>
        [[nodiscard]] std::vector<std::pair<const char*, int>> get_field_offsets() const;


        /// <summary>Check if a method with the given name exists (optionally matching arg count).</summary>
        [[nodiscard]] bool has_method(std::string_view name, int argc = -1) const;

        /// <summary>Get all methods for this object's class (convenience wrapper).</summary>
        [[nodiscard]] std::vector<Method> get_methods() const;

        /// <summary>Iterate all methods with a callback. Returns early if the callback returns false.</summary>
        void for_each_method(const std::function<bool(const Method&)>& callback) const;

        /// <summary>Find all methods whose return type name matches the given string.</summary>
        [[nodiscard]] std::vector<Method> find_methods_by_return_type(std::string_view typeName) const;

        /// <summary>Find all methods with the given parameter count.</summary>
        [[nodiscard]] std::vector<Method> find_methods_by_param_count(int count) const;


        /// <summary>Get the instance size in bytes (from the class metadata).</summary>
        [[nodiscard]] uint32_t instance_size() const;

        /// <summary>Get the class name as a string (namespace.name).</summary>
        [[nodiscard]] std::string get_class_name() const;

        /// <summary>Call the managed ToString() method.</summary>
        [[nodiscard]] std::string to_string() const;

        /// <summary>Check if this object is an instance of the given class (or a derived class).</summary>
        [[nodiscard]] bool is_instance_of(const Class& klass) const;

        /// <summary>Safe downcast: returns a T if this object is an instance of T's class, else default T.</summary>
        template<typename T>
            requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] T try_cast() const {
            if (!valid()) return T{};
            T dummy{};
            return T{ m_native };
        }

        /// <summary>Safe downcast with type checking against a known class.</summary>
        template<typename T>
            requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] T try_cast(const Class& targetClass) const {
            if (!valid() || !targetClass) return T{};
            if (!is_instance_of(targetClass)) return T{};
            return T{ m_native };
        }


        /// <summary>Unbox this managed object to a value type T.</summary>
        template<typename T>
        [[nodiscard]] T unbox() const {
            if (!valid()) return T{};
            return *reinterpret_cast<T*>(IL2CPP::Unbox(m_native));
        }

        /// <summary>Get a pointer to the unboxed data.</summary>
        [[nodiscard]] void* unbox_ptr() const;

        /// <summary>Box a value type into a managed object. Requires the class of the value type.</summary>
        [[nodiscard]] static ManagedObject box_value(const Class& klass, void* data);

        /// <summary>Typed box helper.</summary>
        template<typename T>
        [[nodiscard]] static ManagedObject box(const Class& klass, T value) {
            return box_value(klass, &value);
        }


        [[nodiscard]] std::string get_string_field(std::string_view name) const;
        [[nodiscard]] std::string get_string_property(std::string_view name) const;
        [[nodiscard]] std::string call_string_method(std::string_view name, void** params = nullptr, int argc = -1) const;
        void set_string_field(std::string_view name, std::string_view value);
        void set_string_property(std::string_view name, std::string_view value);


        [[nodiscard]] bool operator==(const ManagedObject& other) const noexcept { return m_native == other.m_native; }
        [[nodiscard]] bool operator!=(const ManagedObject& other) const noexcept { return m_native != other.m_native; }
    };

} // namespace IL2CPP::Module

template<typename T>
    requires std::is_base_of_v<IL2CPP::Module::ManagedObject, T>
struct std::hash<T> {
    size_t operator()(const T& obj) const noexcept {
        return std::hash<void*>{}(obj.raw());
    }
};
