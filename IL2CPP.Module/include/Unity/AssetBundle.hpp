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

        static std::string WriteTempFile(const void* data, size_t size) {
            char tempDir[MAX_PATH];
            char tempPath[MAX_PATH];
            if (!GetTempPathA(MAX_PATH, tempDir)) return "";
            if (!GetTempFileNameA(tempDir, "ab_", 0, tempPath)) return "";

            FILE* f = nullptr;
            if (fopen_s(&f, tempPath, "wb") != 0 || !f) return "";
            size_t written = fwrite(data, 1, size, f);
            fclose(f);

            if (written != size) {
                DeleteFileA(tempPath);
                return "";
            }
            return tempPath;
        }

    public:
        using Object::Object;

        [[nodiscard]] static AssetBundle LoadFromFile(std::string_view path, uint32_t crc = 0, uint64_t offset = 0) {
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return AssetBundle{};
            System::String str = System::String::create(std::string(path).c_str());

            // Use runtime_invoke on the managed LoadFromFile_Internal — works on
            // both 2022.3 and Unity 6 regardless of internal marshaling changes.
            static auto m = MethodHandler::resolve(
                IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadFromFile_Internal"), 3);
            if (m) {
                uint64_t offsetVal = offset;
                void* params[] = { str.raw(), &crc, &offsetVal };
                return AssetBundle{ MethodHandler::invoke<void*>(m, nullptr, params) };
            }

            // Direct fallback
            static auto fn = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uint32_t, uint64_t)>(
                Module::ResolveCall("UnityEngine.AssetBundle::LoadFromFile_Internal", true));
            if (!fn) return AssetBundle{};
            return AssetBundle{ fn(str.raw(), crc, offset) };
        }

        [[nodiscard]] static AssetBundle LoadFromMemory(const void* data, size_t size) {
            if (!data || size == 0) return AssetBundle{};
            std::string tempPath = WriteTempFile(data, size);
            if (tempPath.empty()) return AssetBundle{};
            AssetBundle bundle = LoadFromFile(tempPath);
            DeleteFileA(tempPath.c_str());
            return bundle;
        }

        [[nodiscard]] static std::vector<AssetBundle> GetAllLoaded() {
            return Object::FindObjectsOfTypeAs<AssetBundle>("UnityEngine.AssetBundle");
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
