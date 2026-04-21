#pragma once
#include "Reflection.hpp"
#include "Deobfuscation.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <vector>
#include <string>
#include <string_view>
#include <initializer_list>

namespace IL2CPP::Module {

    class ClassCache {
    public:
        /// <summary>Get the singleton instance</summary>
        static ClassCache& Get() {
            static ClassCache instance;
            return instance;
        }

        /// <summary>Load an assembly into the cache</summary>
        void Load(std::string_view assembly) {
            auto* e = GetExports();
            if (!e || !e->m_helperCacheLoad) return;
            std::string asm_str(assembly);
            reinterpret_cast<void(IL2CPP_CALLTYPE)(const char*)>(
                e->m_helperCacheLoad)(asm_str.c_str());
        }

        /// <summary>Load multiple assemblies at once</summary>
        void LoadMultiple(std::initializer_list<const char*> assemblies) {
            auto* e = GetExports();
            if (!e || !e->m_helperCacheLoadMultiple) return;
            std::vector<const char*> vec(assemblies);
            reinterpret_cast<void(IL2CPP_CALLTYPE)(const char**, size_t)>(
                e->m_helperCacheLoadMultiple)(vec.data(), vec.size());
        }

        /// <summary>Release all cached classes</summary>
        void ReleaseAll() {
            auto* e = GetExports();
            if (!e || !e->m_helperCacheRelease) return;
            reinterpret_cast<void(IL2CPP_CALLTYPE)()>(e->m_helperCacheRelease)();
        }

        /// <summary>Release a specific assembly's cache</summary>
        void Release(std::string_view assembly) {
            auto* e = GetExports();
            if (!e || !e->m_helperCacheReleaseOne) return;
            std::string asm_str(assembly);
            reinterpret_cast<void(IL2CPP_CALLTYPE)(const char*)>(
                e->m_helperCacheReleaseOne)(asm_str.c_str());
        }

        /// Get all classes from an assembly as wrapped Class handles.
        /// All names (class, fields, methods, properties) are automatically deobfuscated.
        [[nodiscard]] std::vector<Class> Classes(std::string_view assembly) {
            std::vector<Class> result;
            auto* e = GetExports();
            if (!e || !e->m_helperCacheGetClasses) return result;

            std::string asm_str(assembly);
            size_t count = 0;
            void** rawClasses = reinterpret_cast<void**(IL2CPP_CALLTYPE)(const char*, size_t*)>(
                e->m_helperCacheGetClasses)(asm_str.c_str(), &count);

            if (!rawClasses || count == 0) return result;

            result.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                if (rawClasses[i]) {
                    result.emplace_back(rawClasses[i]);
                }
            }
            return result;
        }

        /// Filter classes by signatures in a specific assembly.
        /// Returns a wrapped Class handle (or empty if not found).
        ///
        /// SIGNATURE SYNTAX:
        ///   "fieldName"      - match if class has field or method with this name
        ///   "~fieldName"     - match if class has field with this name
        ///   "-methodName"    - match if class has method with this name
        ///   "^className"     - match if direct parent class has this name
        ///   "^^className"    - match if any ancestor class has this name
        ///   "!name"          - negative: must NOT match (combinable with any prefix)
        ///
        /// EXAMPLES:
        ///   cache.Filter({ "~_vrcplayer", "~_USpeaker" }, "Assembly-CSharp");
        ///   // Finds class with both _vrcplayer and _USpeaker fields
        ///
        ///   cache.Filter({ "^MonoBehaviour", "~_field" }, "Assembly-CSharp");
        ///   // Finds class whose direct parent is MonoBehaviour and has _field
        ///
        ///   cache.Filter({ "^^VRCNetworkBehaviour" }, "Assembly-CSharp");
        ///   // Finds class that inherits from VRCNetworkBehaviour (any depth)
        [[nodiscard]] Class Filter(
            std::initializer_list<const char*> signatures,
            std::string_view assembly) {
            auto* e = GetExports();
            if (!e || !e->m_helperCacheFilter) return Class{};

            std::string asm_str(assembly);
            std::vector<const char*> sigs(signatures);
            void* result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char**, size_t, const char*)>(
                e->m_helperCacheFilter)(sigs.data(), sigs.size(), asm_str.c_str());

            return Class{ result };
        }

        /// Filter classes by signatures across all loaded assemblies.
        /// Returns a wrapped Class handle (or empty if not found).
        [[nodiscard]] Class FilterAll(std::initializer_list<const char*> signatures) {
            auto* e = GetExports();
            if (!e || !e->m_helperCacheFilterAll) return Class{};

            std::vector<const char*> sigs(signatures);
            void* result = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char**, size_t)>(
                e->m_helperCacheFilterAll)(sigs.data(), sigs.size());

            return Class{ result };
        }

    private:
        ClassCache() = default;
        ~ClassCache() = default;
        ClassCache(const ClassCache&) = delete;
        ClassCache& operator=(const ClassCache&) = delete;
    };

} // namespace IL2CPP::Module
