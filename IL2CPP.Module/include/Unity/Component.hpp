#pragma once
#include "Object.hpp"
#include "../Reflection.hpp"
#include "../MethodHandler.hpp"
#include <atomic>
#include <cstdint>
#include <vector>

namespace IL2CPP::Module::Unity {

    namespace typecache {
        /// <summary>System.Type object for a class, memoised per Il2CppClass*. Type objects are rooted by the runtime for the process lifetime.</summary>
        inline void* CachedSystemType(const Class& klass, const Type& type) {
            struct Node { void* klass; void* object; };
            constexpr size_t kSlots = 256;
            static std::atomic<const Node*> slots[kSlots]{};

            void* key = klass.raw();
            const size_t start = (reinterpret_cast<uintptr_t>(key) >> 3) % kSlots;
            for (size_t i = 0; i < 8; ++i) {
                auto& slot = slots[(start + i) % kSlots];
                const Node* node = slot.load(std::memory_order_acquire);
                if (node) {
                    if (node->klass == key) return node->object;
                    continue;
                }
                void* object = type.get_system_type_object();
                if (!object || !key) return object;
                const Node* fresh = new Node{ key, object };
                const Node* expected = nullptr;
                if (!slot.compare_exchange_strong(expected, fresh, std::memory_order_acq_rel)) delete fresh;
                return object;
            }
            return type.get_system_type_object();
        }
    }

    class GameObject;

    class Component : public Object {
    public:
        using Object::Object;

        /// <summary>Get the Transform attached to this component.</summary>
        [[nodiscard]] Transform GetTransform() const;

        /// <summary>Get the GameObject this component is attached to.</summary>
        [[nodiscard]] GameObject GetGameObject() const;


        /// <summary>Get a component by System.Type.</summary>
        [[nodiscard]] Component GetComponent(Il2CppSystemType* systemType) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("GetComponent"), 1);
            if (!systemType) return Component{};
            void* params[] = { systemType };
            return Component{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        /// <summary>Get a component by Class.</summary>
        [[nodiscard]] Component GetComponent(Class klass) const {
            if (!klass) return Component{};
            Type t = klass.get_type();
            if (!t) return Component{};
            return GetComponent(reinterpret_cast<Il2CppSystemType*>(typecache::CachedSystemType(klass, t)));
        }

        /// <summary>Get a component by name.</summary>
        [[nodiscard]] Component GetComponentByName(std::string_view name) const {
            Class klass = Class::find(name);
            if (!klass) return Component{};
            return GetComponent(klass);
        }

        /// <summary>Get a component in children by System.Type.</summary>
        [[nodiscard]] Component GetComponentInChildren(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("GetComponentInChildren"), 2);
            if (!systemType) return Component{};
            void* params[] = { systemType, &includeInactive };
            return Component{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        /// <summary>Get a component in children by Class.</summary>
        [[nodiscard]] Component GetComponentInChildren(Class klass, bool includeInactive = false) const {
            if (!klass) return Component{};
            Type t = klass.get_type();
            if (!t) return Component{};
            return GetComponentInChildren(reinterpret_cast<Il2CppSystemType*>(typecache::CachedSystemType(klass, t)), includeInactive);
        }

        /// <summary>Get a component in parent by System.Type.</summary>
        [[nodiscard]] Component GetComponentInParent(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("GetComponentInParent"), 2);
            if (!systemType) return Component{};
            void* params[] = { systemType, &includeInactive };
            return Component{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        /// <summary>Get a component in parent by Class.</summary>
        [[nodiscard]] Component GetComponentInParent(Class klass, bool includeInactive = false) const {
            if (!klass) return Component{};
            Type t = klass.get_type();
            if (!t) return Component{};
            return GetComponentInParent(reinterpret_cast<Il2CppSystemType*>(typecache::CachedSystemType(klass, t)), includeInactive);
        }


        /// Get all components of the specified type.
        /// @param systemType The System.Type object pointer (Il2CppSystemType*)
        /// @return A vector of Component containing all found components.
        [[nodiscard]] std::vector<Component> GetComponents(Il2CppSystemType* systemType) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("GetComponents"), 1);
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
            return GetComponents(reinterpret_cast<Il2CppSystemType*>(typecache::CachedSystemType(klass, t)));
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
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("GetComponents"), 1);
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
            return GetComponentsAs<T>(reinterpret_cast<Il2CppSystemType*>(typecache::CachedSystemType(klass, t)));
        }

        /// <summary>Get all components of the specified type in children.</summary>
        [[nodiscard]] std::vector<Component> GetComponentsInChildren(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("GetComponentsInChildren"), 2);
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
            return GetComponentsInChildren(reinterpret_cast<Il2CppSystemType*>(typecache::CachedSystemType(klass, t)), includeInactive);
        }

        /// <summary>Get all components in children and return as std::vector&lt;T&gt;.</summary>
        template<typename T> requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] std::vector<T> GetComponentsInChildrenAs(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("GetComponentsInChildren"), 2);
            if (!systemType) return {};
            void* params[] = { systemType, &includeInactive };
            void* array = MethodHandler::invoke<void*>(m, raw(), params);
            return Object::FromArray<T>(array);
        }

        /// <summary>Get all components of the specified type in parent.</summary>
        [[nodiscard]] std::vector<Component> GetComponentsInParent(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("GetComponentsInParent"), 2);
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
            return GetComponentsInParent(reinterpret_cast<Il2CppSystemType*>(typecache::CachedSystemType(klass, t)), includeInactive);
        }

        /// <summary>Get all components in parent and return as std::vector&lt;T&gt;.</summary>
        template<typename T> requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] std::vector<T> GetComponentsInParentAs(Il2CppSystemType* systemType, bool includeInactive = false) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("GetComponentsInParent"), 2);
            if (!systemType) return {};
            void* params[] = { systemType, &includeInactive };
            void* array = MethodHandler::invoke<void*>(m, raw(), params);
            return Object::FromArray<T>(array);
        }


        /// Try to get a component of the specified type.
        /// @param systemType The System.Type object pointer
        /// @param out Output parameter for the found component
        /// @return true if the component was found, false otherwise
        [[nodiscard]] bool TryGetComponent(Il2CppSystemType* systemType, Component& out) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Component"), IL2CPP_STR("TryGetComponent"), 2);
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
            return TryGetComponent(reinterpret_cast<Il2CppSystemType*>(typecache::CachedSystemType(klass, t)), out);
        }
    };

    using ComponentArray = std::vector<Component>;

} // namespace IL2CPP::Module::Unity
