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

    class ManagedObject {
    protected:
        void* m_native = nullptr;  // il2cppObject*

    public:
        ManagedObject() = default;
        explicit ManagedObject(void* raw) : m_native(raw) {}


        /// Get the raw il2cppObject* pointer.
        [[nodiscard]] void* raw() const noexcept { return m_native; }

        /// Implicit bool conversion - check if the object is valid.
        [[nodiscard]] operator bool() const noexcept { return valid(); }

        /// Check if the underlying pointer is a valid managed object.
        [[nodiscard]] bool valid() const noexcept;

        /// Get the class handle for this object.
        [[nodiscard]] Class get_class() const;


        template<typename T>
        [[nodiscard]] T get_field(int fieldOffset) const {
            if (!valid()) return T{};
            return *reinterpret_cast<T*>(static_cast<char*>(m_native) + fieldOffset);
        }

        /// Get a field value by name. Returns default T{} if not found.
        template<typename T>
        [[nodiscard]] T get_field(std::string_view name) const {
            if (!valid()) return T{};
            Class klass = get_class();
            if (!klass) return T{};
            Field f = klass.get_field(name);
            if (!f) return T{};
            int off = f.offset();
            if (off >= 0) {
                return *reinterpret_cast<T*>(static_cast<char*>(m_native) + off);
            }
            return T{};
        }

        /// Get a field value using a pre-resolved Field handle (skips name lookup).
        template<typename T>
        [[nodiscard]] T get_field(const Field& field) const {
            if (!valid() || !field) return T{};
            int off = field.offset();
            if (off >= 0) {
                return *reinterpret_cast<T*>(static_cast<char*>(m_native) + off);
            }
            return T{};
        }

        /// Set a field value by name.
        template<typename T>
        void set_field(std::string_view name, T value) {
            if (!valid()) return;
            Class klass = get_class();
            if (!klass) return;
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

        /// Set a field value using a pre-resolved Field handle (skips name lookup).
        template<typename T>
        void set_field(const Field& field, T value) {
            if (!valid() || !field) return;
            int off = field.offset();
            if (off >= 0) {
                *reinterpret_cast<T*>(static_cast<char*>(m_native) + off) = value;
            }
        }

        /// Get a property value by name (calls the getter).
        template<typename T>
        [[nodiscard]] T get_property(std::string_view name) const {
            if (!valid()) return T{};
            Class klass = get_class();
            if (!klass) return T{};
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

        /// Set a property value by name (calls the setter).
        template<typename T>
        void set_property(std::string_view name, T value) {
            if (!valid()) return;
            Class klass = get_class();
            if (!klass) return;
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


        /// Read a value at a raw offset from the object base.
        template<typename T>
        [[nodiscard]] T read(int offset) const {
            if (!m_native || offset < 0) return T{};
            return *reinterpret_cast<T*>(static_cast<char*>(m_native) + offset);
        }

        /// Write a value at a raw offset from the object base.
        template<typename T>
        void write(int offset, T value) {
            if (!m_native || offset < 0) return;
            *reinterpret_cast<T*>(static_cast<char*>(m_native) + offset) = value;
        }

        /// Get a typed pointer at a raw offset from the object base.
        template<typename T = void*>
        [[nodiscard]] T* ptr_at(int offset) const {
            if (!m_native || offset < 0) return nullptr;
            return reinterpret_cast<T*>(static_cast<char*>(m_native) + offset);
        }


        /// Call a method by name with typed return. Uses runtime invoke.
        template<typename TReturn>
        TReturn call_method(std::string_view name, void** params = nullptr, int argc = -1) const {
            if constexpr (std::is_void_v<TReturn>) {
                if (!valid()) return;
                Class klass = get_class();
                if (!klass) return;
                Method m = klass.get_method(name, argc);
                if (!m) return;
                m.invoke(m_native, params);
            }
            else {
                if (!valid()) return TReturn{};
                Class klass = get_class();
                if (!klass) return TReturn{};
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

        /// Get a direct function pointer for a method, for fast calling.
        [[nodiscard]] void* get_method_pointer(std::string_view name, int argc = -1) const;

        /// Get a field handle by name.
        [[nodiscard]] Field get_field_info(std::string_view name) const;

        /// Get a method handle by name.
        [[nodiscard]] Method get_method_info(std::string_view name, int argc = -1) const;


        /// Get a static field value by name.
        template<typename T>
        [[nodiscard]] T get_static_field(std::string_view name) const {
            if (!valid()) return T{};
            Class klass = get_class();
            if (!klass) return T{};
            Field f = klass.get_field(name);
            if (!f || !f.is_static()) return T{};
            T value{};
            f.get_static_value(&value);
            return value;
        }

        /// Set a static field value by name.
        template<typename T>
        void set_static_field(std::string_view name, T value) {
            if (!valid()) return;
            Class klass = get_class();
            if (!klass) return;
            Field f = klass.get_field(name);
            if (!f || !f.is_static()) return;
            f.set_static_value(&value);
        }


        /// Check if a field with the given name exists on this object's class.
        [[nodiscard]] bool has_field(std::string_view name) const;

        /// Get the byte offset for a field by name. Returns -1 if not found.
        [[nodiscard]] int get_field_offset(std::string_view name) const;

        /// Reverse lookup: get the Field metadata for a field at a specific offset.
        [[nodiscard]] Field get_field_by_offset(int offset) const;

        /// Get the Nth field in declaration order (0-indexed).
        [[nodiscard]] Field get_field_by_index(int index) const;

        /// Find all fields whose type name matches the given string.
        [[nodiscard]] std::vector<Field> find_fields_by_type(std::string_view typeName) const;

        /// Find all fields matching an access modifier ("public", "private", "protected", "internal").
        [[nodiscard]] std::vector<Field> find_fields_by_access(std::string_view access) const;

        /// Find all fields matching a predicate.
        [[nodiscard]] std::vector<Field> find_fields(const std::function<bool(const Field&)>& predicate) const;


        /// Get all fields for this object's class (convenience wrapper).
        [[nodiscard]] std::vector<Field> get_fields() const;

        /// Iterate all fields with a callback. Returns early if the callback returns false.
        void for_each_field(const std::function<bool(const Field&)>& callback) const;

        /// Get a list of {name, offset} pairs for all instance fields.
        [[nodiscard]] std::vector<std::pair<const char*, int>> get_field_offsets() const;


        /// Check if a method with the given name exists (optionally matching arg count).
        [[nodiscard]] bool has_method(std::string_view name, int argc = -1) const;

        /// Get all methods for this object's class (convenience wrapper).
        [[nodiscard]] std::vector<Method> get_methods() const;

        /// Iterate all methods with a callback. Returns early if the callback returns false.
        void for_each_method(const std::function<bool(const Method&)>& callback) const;

        /// Find all methods whose return type name matches the given string.
        [[nodiscard]] std::vector<Method> find_methods_by_return_type(std::string_view typeName) const;

        /// Find all methods with the given parameter count.
        [[nodiscard]] std::vector<Method> find_methods_by_param_count(int count) const;


        /// Get the instance size in bytes (from the class metadata).
        [[nodiscard]] uint32_t instance_size() const;

        /// Get the class name as a string (namespace.name).
        [[nodiscard]] std::string get_class_name() const;

        /// Call the managed ToString() method.
        [[nodiscard]] std::string to_string() const;

        /// Check if this object is an instance of the given class (or a derived class).
        [[nodiscard]] bool is_instance_of(const Class& klass) const;

        /// Safe downcast: returns a T if this object is an instance of T's class, else default T.
        template<typename T>
            requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] T try_cast() const {
            if (!valid()) return T{};
            T dummy{};
            return T{ m_native };
        }

        /// Safe downcast with type checking against a known class.
        template<typename T>
            requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] T try_cast(const Class& targetClass) const {
            if (!valid() || !targetClass) return T{};
            if (!is_instance_of(targetClass)) return T{};
            return T{ m_native };
        }


        /// Unbox this managed object to a value type T.
        template<typename T>
        [[nodiscard]] T unbox() const {
            if (!valid()) return T{};
            return *reinterpret_cast<T*>(IL2CPP::Unbox(m_native));
        }

        /// Get a pointer to the unboxed data.
        [[nodiscard]] void* unbox_ptr() const;

        /// Box a value type into a managed object. Requires the class of the value type.
        [[nodiscard]] static ManagedObject box_value(const Class& klass, void* data);

        /// Typed box helper.
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

    /// Check if a raw pointer looks like a valid IL2CPP object.
    [[nodiscard]] inline bool IsValidPointer(void* ptr) noexcept {
        if (!ptr) return false;
        constexpr uintptr_t minAddr = 0x10000;
        constexpr uintptr_t maxAddr = 0x7FFFFFFF0000;
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        return addr >= minAddr && addr <= maxAddr;
    }

} // namespace IL2CPP::Module

template<typename T>
    requires std::is_base_of_v<IL2CPP::Module::ManagedObject, T>
struct std::hash<T> {
    size_t operator()(const T& obj) const noexcept {
        return std::hash<void*>{}(obj.raw());
    }
};
