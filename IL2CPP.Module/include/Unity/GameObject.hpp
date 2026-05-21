#pragma once
#include "Object.hpp"
#include "Component.hpp"
#include "../Reflection.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <vector>

namespace IL2CPP::Module::Unity {

    class Transform;

    class GameObject : public Object {
    public:
        using Object::Object;

        /// <summary>Static: Find a GameObject by name.</summary>
        [[nodiscard]] static GameObject Find(std::string_view name) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("Find"), 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return GameObject{};
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(name).c_str());
            void* params[] = { il2cppStr };
            return GameObject{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        /// <summary>Static: Find a GameObject by tag.</summary>
        [[nodiscard]] static GameObject FindWithTag(std::string_view tag) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("FindWithTag"), 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return GameObject{};
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(tag).c_str());
            void* params[] = { il2cppStr };
            return GameObject{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        /// <summary>Static: Find all GameObjects with the specified tag.</summary>
        [[nodiscard]] static std::vector<GameObject> FindGameObjectsWithTag(std::string_view tag) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("FindGameObjectsWithTag"), 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return {};
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(tag).c_str());
            void* params[] = { il2cppStr };
            void* array = MethodHandler::invoke<void*>(m, nullptr, params);
            return Object::FromArray<GameObject>(array);
        }

        /// <summary>Static: Create a primitive GameObject (Sphere, Cube, etc.).</summary>
        [[nodiscard]] static GameObject CreatePrimitive(PrimitiveType type) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("CreatePrimitive"), 1);
            int val = static_cast<int>(type);
            void* params[] = { &val };
            return GameObject{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        /// <summary>Get the Transform component.</summary>
        [[nodiscard]] Transform GetTransform() const;


        /// <summary>Get a component by System.Type.</summary>
        [[nodiscard]] Component GetComponent(Il2CppSystemType* systemType) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("GetComponent"), 1);
            if (!systemType) return Component{};
            void* params[] = { systemType };
            return Component{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        /// <summary>Get a component by Class.</summary>
        [[nodiscard]] Component GetComponent(Class klass) const {
            if (!klass) return Component{};
            Type t = klass.get_type();
            if (!t) return Component{};
            return GetComponent(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()));
        }

        /// <summary>Get a component by name.</summary>
        [[nodiscard]] Component GetComponentByName(std::string_view name) const {
            Class klass = Class::find(name);
            if (!klass) return Component{};
            return GetComponent(klass);
        }

        /// <summary>Get a component in children.</summary>
        [[nodiscard]] Component GetComponentInChildren(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("GetComponentInChildren"), 2);
            if (!systemType) return Component{};
            void* params[] = { systemType, &includeInactive };
            return Component{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        /// <summary>Get a component in children by Class.</summary>
        [[nodiscard]] Component GetComponentInChildren(Class klass, bool includeInactive = false) const {
            if (!klass) return Component{};
            Type t = klass.get_type();
            if (!t) return Component{};
            return GetComponentInChildren(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()), includeInactive);
        }

        /// <summary>Get a component in parent.</summary>
        [[nodiscard]] Component GetComponentInParent(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("GetComponentInParent"), 2);
            if (!systemType) return Component{};
            void* params[] = { systemType, &includeInactive };
            return Component{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        /// <summary>Get a component in parent by Class.</summary>
        [[nodiscard]] Component GetComponentInParent(Class klass, bool includeInactive = false) const {
            if (!klass) return Component{};
            Type t = klass.get_type();
            if (!t) return Component{};
            return GetComponentInParent(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()), includeInactive);
        }

        /// <summary>Add a component.</summary>
        [[nodiscard]] Component AddComponent(Il2CppSystemType* systemType) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("AddComponent"), 1);
            if (!systemType) return Component{};
            void* params[] = { systemType };
            return Component{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        /// <summary>Add a component by Class.</summary>
        [[nodiscard]] Component AddComponent(Class klass) {
            if (!klass) return Component{};
            Type t = klass.get_type();
            if (!t) return Component{};
            return AddComponent(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()));
        }


        /// <summary>Get all components of the specified type.</summary>
        [[nodiscard]] std::vector<Component> GetComponents(Il2CppSystemType* systemType) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("GetComponents"), 1);
            if (!systemType) return {};
            void* params[] = { systemType };
            void* array = MethodHandler::invoke<void*>(m, raw(), params);
            return Object::FromArray<Component>(array);
        }

        /// <summary>Get all components of the specified class.</summary>
        [[nodiscard]] std::vector<Component> GetComponents(Class klass) const {
            if (!klass) return {};
            Type t = klass.get_type();
            if (!t) return {};
            return GetComponents(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()));
        }

        /// <summary>Get all components of the specified class by name.</summary>
        [[nodiscard]] std::vector<Component> GetComponents(std::string_view className) const {
            Class klass = Class::find(className);
            if (!klass) return {};
            return GetComponents(klass);
        }

        /// <summary>Get all components and return as std::vector&lt;T&gt;.</summary>
        template<typename T> requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] std::vector<T> GetComponentsAs(Il2CppSystemType* systemType) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("GetComponents"), 1);
            if (!systemType) return {};
            void* params[] = { systemType };
            void* array = MethodHandler::invoke<void*>(m, raw(), params);
            return Object::FromArray<T>(array);
        }

        template<typename T> requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] std::vector<T> GetComponentsAs(Class klass) const {
            if (!klass) return {};
            Type t = klass.get_type();
            if (!t) return {};
            return GetComponentsAs<T>(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()));
        }

        /// <summary>Get all components of the specified type in children.</summary>
        [[nodiscard]] std::vector<Component> GetComponentsInChildren(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("GetComponentsInChildren"), 2);
            if (!systemType) return {};
            void* params[] = { systemType, &includeInactive };
            void* array = MethodHandler::invoke<void*>(m, raw(), params);
            return Object::FromArray<Component>(array);
        }

        /// <summary>Get all components of the specified class in children.</summary>
        [[nodiscard]] std::vector<Component> GetComponentsInChildren(Class klass, bool includeInactive = false) const {
            if (!klass) return {};
            Type t = klass.get_type();
            if (!t) return {};
            return GetComponentsInChildren(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()), includeInactive);
        }

        /// <summary>Get all components in children and return as std::vector&lt;T&gt;.</summary>
        template<typename T> requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] std::vector<T> GetComponentsInChildrenAs(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("GetComponentsInChildren"), 2);
            if (!systemType) return {};
            void* params[] = { systemType, &includeInactive };
            void* array = MethodHandler::invoke<void*>(m, raw(), params);
            return Object::FromArray<T>(array);
        }

        /// <summary>Get all components of the specified type in parent.</summary>
        [[nodiscard]] std::vector<Component> GetComponentsInParent(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("GetComponentsInParent"), 2);
            if (!systemType) return {};
            void* params[] = { systemType, &includeInactive };
            void* array = MethodHandler::invoke<void*>(m, raw(), params);
            return Object::FromArray<Component>(array);
        }

        /// <summary>Get all components of the specified class in parent.</summary>
        [[nodiscard]] std::vector<Component> GetComponentsInParent(Class klass, bool includeInactive = false) const {
            if (!klass) return {};
            Type t = klass.get_type();
            if (!t) return {};
            return GetComponentsInParent(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()), includeInactive);
        }

        /// <summary>Get all components in parent and return as std::vector&lt;T&gt;.</summary>
        template<typename T> requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] std::vector<T> GetComponentsInParentAs(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("GetComponentsInParent"), 2);
            if (!systemType) return {};
            void* params[] = { systemType, &includeInactive };
            void* array = MethodHandler::invoke<void*>(m, raw(), params);
            return Object::FromArray<T>(array);
        }

        /// <summary>Try to get a component of the specified type.</summary>
        [[nodiscard]] bool TryGetComponent(Il2CppSystemType* systemType, Component& out) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("TryGetComponent"), 2);
            if (!systemType) return false;
            void* outComp = nullptr;
            void* params[] = { systemType, &outComp };
            bool success = MethodHandler::invoke<bool>(m, raw(), params);
            if (success && outComp) {
                out = Component{ outComp };
                return true;
            }
            return false;
        }

        /// <summary>Try to get a component of the specified class.</summary>
        [[nodiscard]] bool TryGetComponent(Class klass, Component& out) const {
            if (!klass) return false;
            Type t = klass.get_type();
            if (!t) return false;
            return TryGetComponent(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()), out);
        }


        [[nodiscard]] bool GetActive() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("get_active"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] bool GetActiveSelf() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("get_activeSelf"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] bool GetActiveInHierarchy() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("get_activeInHierarchy"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        void SetActive(bool active) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("SetActive"), 1);
            void* params[] = { &active };
            MethodHandler::invoke(m, raw(), params);
        }


        [[nodiscard]] unsigned int GetLayer() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("get_layer"), 0);
            return static_cast<unsigned int>(MethodHandler::invoke<int>(m, raw()));
        }

        void SetLayer(unsigned int layer) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("set_layer"), 1);
            int val = static_cast<int>(layer);
            void* params[] = { &val };
            MethodHandler::invoke(m, raw(), params);
        }


        [[nodiscard]] Scene GetScene() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("get_scene"), 0);
            return MethodHandler::invoke<Scene>(m, raw());
        }


        [[nodiscard]] std::string GetTag() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("get_tag"), 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        void SetTag(std::string_view tag) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("set_tag"), 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(tag).c_str());
            void* params[] = { il2cppStr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool CompareTag(std::string_view tag) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("CompareTag"), 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return false;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(tag).c_str());
            void* params[] = { il2cppStr };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }


        void SendMessage(std::string_view methodName, SendMessageOptions options = SendMessageOptions::RequireReceiver) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("SendMessage"), 2);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(methodName).c_str());
            int opt = static_cast<int>(options);
            void* params[] = { il2cppStr, &opt };
            MethodHandler::invoke(m, raw(), params);
        }

        void SendMessageUpwards(std::string_view methodName, SendMessageOptions options = SendMessageOptions::RequireReceiver) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("SendMessageUpwards"), 2);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(methodName).c_str());
            int opt = static_cast<int>(options);
            void* params[] = { il2cppStr, &opt };
            MethodHandler::invoke(m, raw(), params);
        }

        void BroadcastMessage(std::string_view methodName, SendMessageOptions options = SendMessageOptions::RequireReceiver) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.GameObject"), IL2CPP_STR("BroadcastMessage"), 2);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(methodName).c_str());
            int opt = static_cast<int>(options);
            void* params[] = { il2cppStr, &opt };
            MethodHandler::invoke(m, raw(), params);
        }

    };

    using GameObjectArray = std::vector<GameObject>;

} // namespace IL2CPP::Module::Unity
