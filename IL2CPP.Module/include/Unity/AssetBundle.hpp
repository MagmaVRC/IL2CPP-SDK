#pragma once
#include "../MethodHandler.hpp"
#include "../Reflection.hpp"
#include "../System/Array.hpp"
#include "../System/String.hpp"
#include "../il2cpp_module.hpp"
#include "Object.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <Logging.Module/include/logger_module.hpp>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <windows.h>

namespace IL2CPP::Module::Unity {

    class AssetBundleRequest;
    class AssetBundleUnloadOperation;
    class AssetBundleCreateRequest;

    class AssetBundle : public Object {
        static std::vector<std::string> StringArrayToVector(void* rawArray) {
            if (!rawArray) return {};
            System::Array<void*> arr{ rawArray };
            std::vector<std::string> result;
            result.reserve(arr.size());
            for (uintptr_t i = 0; i < arr.size(); i++) {
                if (arr[i]) result.push_back(System::String{ arr[i] }.to_string());
            }
            return result;
        }

    public:
        using Object::Object;

        // Both loaders resolve a *logical* capability; IL2CPP.Core picks the real
        // strategy (managed internal method, or a native binding when the target
        // stripped it) transparently — see the Core capability registry.
        [[nodiscard]] static AssetBundle LoadFromFile(std::string_view path, uint32_t crc = 0, uint64_t offset = 0) {
            System::String str = System::String::create(std::string(path).c_str());
            if (!str) return AssetBundle{};
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadFromFile"));
            if (!m) return AssetBundle{};
            uint64_t offsetVal = offset;
            void* params[] = { str.raw(), &crc, &offsetVal };
            return AssetBundle{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        [[nodiscard]] static AssetBundle LoadFromMemory(const void* data, size_t size, uint32_t crc = 0) {
            if (!data || size == 0) return AssetBundle{};
            auto bytes = System::Array<uint8_t>::FromBytes(data, size);  // managed byte[]
            if (!bytes) return AssetBundle{};
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadFromMemory"));
            if (!m) return AssetBundle{};
            void* arr = bytes.raw();
            void* params[] = { arr, &crc };
            return AssetBundle{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        // Async variants — return an AssetBundleCreateRequest to poll (GetIsDone / GetAssetBundle).
        // Defined in AssetBundleCreateRequest.hpp (needs the complete request type).
        [[nodiscard]] static AssetBundleCreateRequest LoadFromFileAsync(std::string_view path, uint32_t crc = 0, uint64_t offset = 0);
        [[nodiscard]] static AssetBundleCreateRequest LoadFromMemoryAsync(const void* data, size_t size, uint32_t crc = 0);

        [[nodiscard]] static std::vector<AssetBundle> GetAllLoaded() {
            return Object::FindObjectsOfTypeAs<AssetBundle>(IL2CPP_STR("UnityEngine.AssetBundle"));
        }


        [[nodiscard]] Object LoadAsset(std::string_view name) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadAsset"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return Object{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return Object{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        [[nodiscard]] Object LoadAsset(std::string_view name, std::string_view typeName) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadAsset"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return Object{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            Class klass = Class::find(typeName);
            if (!klass) return Object{};
            Type t = klass.get_type();
            if (!t) return Object{};
            void* sysType = t.get_system_type_object();
            void* params[] = { str, sysType };
            return Object{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        template<typename T>
        [[nodiscard]] T LoadAssetAs(std::string_view name) {
            Object obj = LoadAsset(name);
            if (!obj) return T{};
            return T{ obj.raw() };
        }

        [[nodiscard]] AssetBundleRequest LoadAllAssetsAsync();
        [[nodiscard]] AssetBundleRequest LoadAllAssetsAsync(std::string_view typeName);
        [[nodiscard]] AssetBundleRequest LoadAssetWithSubAssetsAsync(std::string_view name, bool performTypeChecks = false);
        [[nodiscard]] AssetBundleRequest LoadAssetWithSubAssetsAsync(std::string_view name, std::string_view typeName, bool performTypeChecks = false);


        [[nodiscard]] std::vector<std::string> GetAllAssetNames() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("GetAllAssetNames"), 0);
            return StringArrayToVector(MethodHandler::invoke<void*>(m, raw()));
        }

        [[nodiscard]] std::vector<std::string> GetAllScenePaths() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("GetAllScenePaths"), 0);
            return StringArrayToVector(MethodHandler::invoke<void*>(m, raw()));
        }

        [[nodiscard]] bool GetIsStreamedSceneAssetBundle() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("get_isStreamedSceneAssetBundle"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }


        void Unload(bool unloadAllLoadedObjects) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("Unload"), 1);
            void* params[] = { &unloadAllLoadedObjects };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] AssetBundleUnloadOperation UnloadAsync(bool unloadAllLoadedObjects);
    };

} // namespace IL2CPP::Module::Unity
