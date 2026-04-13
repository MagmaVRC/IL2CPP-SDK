#pragma once
#include "../ManagedObject.hpp"
#include "../Reflection.hpp"
#include "../MethodHandler.hpp"
#include "../System/Array.hpp"
#include "../System/String.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace IL2CPP::Module::Unity {

    class Transform;

    class Object : public ManagedObject {
    public:
        using ManagedObject::ManagedObject;

        /// Check if the Unity native pointer is still valid.
        [[nodiscard]] bool IsValid() const noexcept {
            if (!valid()) return false;
            void* cachedPtr = read<void*>(0x10);
            return IsValidPointer(cachedPtr);
        }

        /// Destroy this object.
        void Destroy(float delay = 0.f) {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "Destroy", 2);
            void* self = raw();
            void* params[] = { self, &delay };
            MethodHandler::invoke(m, nullptr, params);
        }

        /// Destroy this object immediately (use with caution).
        void DestroyImmediate() {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "DestroyImmediate", 1);
            void* self = raw();
            void* params[] = { self };
            MethodHandler::invoke(m, nullptr, params);
        }

        /// Get the object name.
        [[nodiscard]] std::string GetName() const {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "get_name", 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        /// Set the object name.
        void SetName(std::string_view name) {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "set_name", 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(name).c_str());
            void* params[] = { il2cppStr };
            MethodHandler::invoke(m, raw(), params);
        }

        /// Instantiate (clone) this object.
        [[nodiscard]] Object Instantiate() const {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "Internal_CloneSingle", 1);
            void* self = raw();
            void* params[] = { self };
            return Object{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        /// Mark this object as DontDestroyOnLoad.
        void DontDestroyOnLoad() {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "DontDestroyOnLoad", 1);
            void* self = raw();
            void* params[] = { self };
            MethodHandler::invoke(m, nullptr, params);
        }

        /// Static: Destroy an object by handle.
        static void Destroy(Object obj, float delay = 0.f) {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "Destroy", 2);
            void* o = obj.raw();
            void* params[] = { o, &delay };
            MethodHandler::invoke(m, nullptr, params);
        }


        /// Find an object of the specified type (first match).
        /// @param systemType The System.Type object pointer (Il2CppSystemType*)
        /// @return The found object, or invalid Object if none found.
        [[nodiscard]] static Object FindObjectOfType(Il2CppSystemType* systemType) {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "FindObjectOfType", 2);
            if (!systemType) return Object{};
            bool inactive = false;
            void* params[] = { systemType, &inactive };
            return Object{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        /// Find an object of the specified class type (first match).
        /// @param klass The Class handle to search for.
        /// @return The found object, or invalid Object if none found.
        [[nodiscard]] static Object FindObjectOfType(Class klass) {
            if (!klass) return Object{};
            Type t = klass.get_type();
            if (!t) return Object{};
            return FindObjectOfType(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()));
        }

        /// Find an object of the specified class by name (first match).
        /// @param className Full class name (e.g., "UnityEngine.Camera")
        /// @return The found object, or invalid Object if none found.
        [[nodiscard]] static Object FindObjectOfType(std::string_view className) {
            Class klass = Class::find(className);
            if (!klass) return Object{};
            return FindObjectOfType(klass);
        }


        /// Helper: convert raw IL2CPP array to std::vector of wrapped objects.
        template<typename T>
        static std::vector<T> FromArray(void* rawArray) {
            if (!rawArray) return {};
            System::Array<void*> arr{ rawArray };

            uintptr_t count = arr.size();
            if (count == 0) return {};

            std::vector<T> result;
            result.reserve(count);

            for (uintptr_t i = 0; i < count; i++) {
                void* element = arr[i];
                if (element) result.push_back(T{ element });
            }
            return result;
        }

        /// Find all objects of the specified type.
        /// @param systemType The System.Type object pointer (Il2CppSystemType*)
        /// @return A vector of Object containing all found objects.
        [[nodiscard]] static std::vector<Object> FindObjectsOfType(Il2CppSystemType* systemType) {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "FindObjectsOfType", 2);
            if (!systemType) return {};
            bool inactive = false;
            void* params[] = { systemType, &inactive };
            void* result = MethodHandler::invoke<void*>(m, nullptr, params);
            return FromArray<Object>(result);
        }

        /// Find all objects of the specified class type.
        /// @param klass The Class handle to search for.
        /// @return A vector of Object containing all found objects.
        [[nodiscard]] static std::vector<Object> FindObjectsOfType(Class klass) {
            if (!klass) return {};
            Type t = klass.get_type();
            if (!t) return {};
            return FindObjectsOfType(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()));
        }

        /// Find all objects of the specified class by name.
        /// @param className Full class name (e.g., "UnityEngine.Camera")
        /// @return A vector of Object containing all found objects.
        [[nodiscard]] static std::vector<Object> FindObjectsOfType(std::string_view className) {
            Class klass = Class::find(className);
            if (!klass) return {};
            return FindObjectsOfType(klass);
        }


        /// Find all objects and return as std::vector<T>.
        /// @tparam T The wrapper type (must derive from ManagedObject)
        /// @param systemType The System.Type object pointer
        /// @return A vector of T containing all found objects.
        template<typename T> requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] static std::vector<T> FindObjectsOfTypeAs(Il2CppSystemType* systemType) {
            static auto m = MethodHandler::resolve("UnityEngine.Object", "FindObjectsOfType", 2);
            if (!systemType) return {};
            bool inactive = false;
            void* params[] = { systemType, &inactive };
            void* result = MethodHandler::invoke<void*>(m, nullptr, params);
            return FromArray<T>(result);
        }

        template<typename T> requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] static std::vector<T> FindObjectsOfTypeAs(Class klass) {
            if (!klass) return {};
            Type t = klass.get_type();
            if (!t) return {};
            return FindObjectsOfTypeAs<T>(reinterpret_cast<Il2CppSystemType*>(t.get_system_type_object()));
        }

        template<typename T> requires std::is_base_of_v<ManagedObject, T>
        [[nodiscard]] static std::vector<T> FindObjectsOfTypeAs(std::string_view className) {
            Class klass = Class::find(className);
            if (!klass) return {};
            return FindObjectsOfTypeAs<T>(klass);
        }


        /// Instantiate (clone) this object with a parent transform.
        /// @param parent The parent Transform for the cloned object.
        /// @return The cloned object.
        [[nodiscard]] Object InstantiateWithParent(Transform parent, bool worldPositionStays = false) const;
    };

    using ObjectArray = std::vector<Object>;

} // namespace IL2CPP::Module::Unity
