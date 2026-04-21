#include <ManagedObject.hpp>
#include <il2cpp_module.hpp>
#include <System/String.hpp>

namespace IL2CPP::Module {

    void* ManagedObject::get_method_pointer(std::string_view name, int argc) const {
        if (!valid()) return nullptr;
        return get_class_internal().get_method_pointer(name, argc);
    }

    Field ManagedObject::get_field_info(std::string_view name) const {
        if (!valid()) return Field{};
        return get_class_internal().get_field(name);
    }

    Method ManagedObject::get_method_info(std::string_view name, int argc) const {
        if (!valid()) return Method{};
        return get_class_internal().get_method(name, argc);
    }


    bool ManagedObject::has_field(std::string_view name) const {
        if (!valid()) return false;
        return static_cast<bool>(get_class_internal().get_field(name));
    }

    int ManagedObject::get_field_offset(std::string_view name) const {
        if (!valid()) return -1;
        Field f = get_class_internal().get_field(name);
        return f ? f.offset() : -1;
    }

    Field ManagedObject::get_field_by_offset(int offset) const {
        if (!valid() || offset < 0) return Field{};
        auto fields = get_class_internal().get_fields();
        for (auto& f : fields) {
            if (f.offset() == offset) return f;
        }
        return Field{};
    }

    Field ManagedObject::get_field_by_index(int index) const {
        if (!valid() || index < 0) return Field{};
        auto fields = get_class_internal().get_fields();
        if (index >= static_cast<int>(fields.size())) return Field{};
        return fields[index];
    }

    std::vector<Field> ManagedObject::find_fields_by_type(std::string_view typeName) const {
        std::vector<Field> result;
        if (!valid()) return result;
        auto fields = get_class_internal().get_fields();
        for (auto& f : fields) {
            const char* tname = f.type_name();
            if (tname && typeName == tname) {
                result.push_back(f);
            }
        }
        return result;
    }

    std::vector<Field> ManagedObject::find_fields_by_access(std::string_view access) const {
        std::vector<Field> result;
        if (!valid()) return result;
        auto fields = get_class_internal().get_fields();
        for (auto& f : fields) {
            const char* mod = f.access_modifier();
            if (mod && access == mod) {
                result.push_back(f);
            }
        }
        return result;
    }

    std::vector<Field> ManagedObject::find_fields(const std::function<bool(const Field&)>& predicate) const {
        std::vector<Field> result;
        if (!valid() || !predicate) return result;
        auto fields = get_class_internal().get_fields();
        for (auto& f : fields) {
            if (predicate(f)) {
                result.push_back(f);
            }
        }
        return result;
    }


    std::vector<Field> ManagedObject::get_fields() const {
        if (!valid()) return {};
        return get_class_internal().get_fields();
    }

    void ManagedObject::for_each_field(const std::function<bool(const Field&)>& callback) const {
        if (!valid() || !callback) return;
        auto fields = get_class_internal().get_fields();
        for (const auto& f : fields) {
            if (!callback(f)) break;
        }
    }

    std::vector<std::pair<const char*, int>> ManagedObject::get_field_offsets() const {
        std::vector<std::pair<const char*, int>> result;
        if (!valid()) return result;
        auto fields = get_class_internal().get_fields();
        for (auto& f : fields) {
            if (!f.is_static()) {
                result.emplace_back(f.name(), f.offset());
            }
        }
        return result;
    }


    uint32_t ManagedObject::instance_size() const {
        if (!valid()) return 0;
        return get_class_internal().instance_size();
    }

    std::string ManagedObject::get_class_name() const {
        if (!valid()) return "";
        return get_class_internal().full_name();
    }

    std::string ManagedObject::to_string() const {
        return call_string_method("ToString", nullptr, 0);
    }

    bool ManagedObject::is_instance_of(const Class& klass) const {
        if (!valid() || !klass) return false;
        Class myClass = get_class_internal();
        if (myClass.raw() == klass.raw()) return true;
        return myClass.is_subclass_of(klass);
    }


    bool ManagedObject::has_method(std::string_view name, int argc) const {
        if (!valid()) return false;
        return static_cast<bool>(get_class_internal().get_method(name, argc));
    }

    std::vector<Method> ManagedObject::get_methods() const {
        if (!valid()) return {};
        return get_class_internal().get_methods();
    }

    void ManagedObject::for_each_method(const std::function<bool(const Method&)>& callback) const {
        if (!valid() || !callback) return;
        auto methods = get_class_internal().get_methods();
        for (const auto& m : methods) {
            if (!callback(m)) break;
        }
    }

    std::vector<Method> ManagedObject::find_methods_by_return_type(std::string_view typeName) const {
        std::vector<Method> result;
        if (!valid()) return result;
        auto methods = get_class_internal().get_methods();
        for (auto& m : methods) {
            const char* retName = m.return_type_name();
            if (retName && typeName == retName) {
                result.push_back(m);
            }
        }
        return result;
    }

    std::vector<Method> ManagedObject::find_methods_by_param_count(int count) const {
        std::vector<Method> result;
        if (!valid() || count < 0) return result;
        auto methods = get_class_internal().get_methods();
        for (auto& m : methods) {
            if (m.param_count() == static_cast<uint8_t>(count)) {
                result.push_back(m);
            }
        }
        return result;
    }


    void* ManagedObject::unbox_ptr() const {
        if (!valid()) return nullptr;
        return IL2CPP::Unbox(m_native);
    }

    ManagedObject ManagedObject::box_value(const Class& klass, void* data) {
        if (!klass || !data) return ManagedObject{};
        auto* e = GetExports();
        if (!e || !e->m_valueBox) return ManagedObject{};
        void* boxed = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, void*)>(e->m_valueBox)(klass.raw(), data);
        return ManagedObject{ boxed };
    }


    std::string ManagedObject::get_string_field(std::string_view name) const {
        if (!valid()) return "";
        auto* strPtr = get_field<void*>(name);
        if (!strPtr) return "";
        return System::String(strPtr).to_string();
    }

    std::string ManagedObject::get_string_property(std::string_view name) const {
        if (!valid()) return "";
        auto* strPtr = get_property<void*>(name);
        if (!strPtr) return "";
        return System::String(strPtr).to_string();
    }

    std::string ManagedObject::call_string_method(std::string_view name, void** params, int argc) const {
        if (!valid()) return "";
        auto* strPtr = call_method<void*>(name, params, argc);
        if (!strPtr) return "";
        return System::String(strPtr).to_string();
    }

    void ManagedObject::set_string_field(std::string_view name, std::string_view value) {
        if (!valid()) return;
        auto str = System::String::create(value);
        set_field(name, str.raw());
    }

    void ManagedObject::set_string_property(std::string_view name, std::string_view value) {
        if (!valid()) return;
        auto str = System::String::create(value);
        set_property(name, str.raw());
    }

} // namespace IL2CPP::Module
