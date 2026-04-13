#pragma once
#include "AssetBundle.hpp"

namespace IL2CPP::Module::Unity {

    /// Wraps UnityEngine.AssetBundleCreateRequest : AsyncOperation
    /// Has get_assetBundle to retrieve the loaded bundle.
    class AssetBundleCreateRequest : public Object {
    public:
        using Object::Object;

        [[nodiscard]] bool GetIsDone() {
            static auto m = MethodHandler::resolve("UnityEngine.AsyncOperation", "get_isDone", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] float GetProgress() {
            static auto m = MethodHandler::resolve("UnityEngine.AsyncOperation", "get_progress", 0);
            return MethodHandler::invoke<float>(m, raw());
        }

        [[nodiscard]] AssetBundle GetAssetBundle() {
            static auto m = MethodHandler::resolve("UnityEngine.AssetBundleCreateRequest", "get_assetBundle", 0);
            return AssetBundle{ MethodHandler::invoke<void*>(m, raw()) };
        }
    };

    /// Wraps UnityEngine.AssetBundleRequest : ResourceRequest
    /// Returned by LoadAllAssetsAsync, LoadAssetWithSubAssetsAsync.
    class AssetBundleRequest : public Object {
    public:
        using Object::Object;

        [[nodiscard]] bool GetIsDone() {
            static auto m = MethodHandler::resolve("UnityEngine.AsyncOperation", "get_isDone", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] float GetProgress() {
            static auto m = MethodHandler::resolve("UnityEngine.AsyncOperation", "get_progress", 0);
            return MethodHandler::invoke<float>(m, raw());
        }

        [[nodiscard]] Object GetAsset() {
            static auto m = MethodHandler::resolve("UnityEngine.AssetBundleRequest", "get_asset", 0);
            return Object{ MethodHandler::invoke<void*>(m, raw()) };
        }

        [[nodiscard]] std::vector<Object> GetAllAssets() {
            static auto m = MethodHandler::resolve("UnityEngine.AssetBundleRequest", "get_allAssets", 0);
            return Object::FromArray<Object>(MethodHandler::invoke<void*>(m, raw()));
        }

        template<typename T>
        [[nodiscard]] T GetAssetAs() {
            Object obj = GetAsset();
            if (!obj) return T{};
            return T{ obj.raw() };
        }

        template<typename T>
        [[nodiscard]] std::vector<T> GetAllAssetsAs() {
            static auto m = MethodHandler::resolve("UnityEngine.AssetBundleRequest", "get_allAssets", 0);
            return Object::FromArray<T>(MethodHandler::invoke<void*>(m, raw()));
        }
    };

    /// Wraps UnityEngine.AssetBundleUnloadOperation : AsyncOperation
    /// Returned by AssetBundle.UnloadAsync.
    class AssetBundleUnloadOperation : public Object {
    public:
        using Object::Object;

        [[nodiscard]] bool GetIsDone() {
            static auto m = MethodHandler::resolve("UnityEngine.AsyncOperation", "get_isDone", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] float GetProgress() {
            static auto m = MethodHandler::resolve("UnityEngine.AsyncOperation", "get_progress", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
    };

    inline AssetBundleRequest AssetBundle::LoadAllAssetsAsync() {
        static auto m = MethodHandler::resolve("UnityEngine.AssetBundle", "LoadAllAssetsAsync", 0);
        return AssetBundleRequest{ MethodHandler::invoke<void*>(m, raw()) };
    }

    inline AssetBundleRequest AssetBundle::LoadAllAssetsAsync(std::string_view typeName) {
        static auto m = MethodHandler::resolve("UnityEngine.AssetBundle", "LoadAllAssetsAsync", 1);
        Class klass = Class::find(typeName);
        if (!klass) return AssetBundleRequest{};
        Type t = klass.get_type();
        if (!t) return AssetBundleRequest{};
        void* sysType = t.get_system_type_object();
        void* params[] = { sysType };
        return AssetBundleRequest{ MethodHandler::invoke<void*>(m, raw(), params) };
    }

    inline AssetBundleRequest AssetBundle::LoadAssetWithSubAssetsAsync(std::string_view name, bool performTypeChecks) {
        static auto m = MethodHandler::resolve("UnityEngine.AssetBundle", "LoadAssetWithSubAssetsAsync", 2);
        auto* e = GetExports();
        if (!e || !e->m_stringNew) return AssetBundleRequest{};
        void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
        void* params[] = { str, &performTypeChecks };
        return AssetBundleRequest{ MethodHandler::invoke<void*>(m, raw(), params) };
    }

    inline AssetBundleRequest AssetBundle::LoadAssetWithSubAssetsAsync(std::string_view name, std::string_view typeName, bool performTypeChecks) {
        static auto m = MethodHandler::resolve("UnityEngine.AssetBundle", "LoadAssetWithSubAssetsAsync", 3);
        auto* e = GetExports();
        if (!e || !e->m_stringNew) return AssetBundleRequest{};
        void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
        Class klass = Class::find(typeName);
        if (!klass) return AssetBundleRequest{};
        Type t = klass.get_type();
        if (!t) return AssetBundleRequest{};
        void* sysType = t.get_system_type_object();
        void* params[] = { str, sysType, &performTypeChecks };
        return AssetBundleRequest{ MethodHandler::invoke<void*>(m, raw(), params) };
    }

    inline AssetBundleUnloadOperation AssetBundle::UnloadAsync(bool unloadAllLoadedObjects) {
        static auto m = MethodHandler::resolve("UnityEngine.AssetBundle", "UnloadAsync", 1);
        void* params[] = { &unloadAllLoadedObjects };
        return AssetBundleUnloadOperation{ MethodHandler::invoke<void*>(m, raw(), params) };
    }

} // namespace IL2CPP::Module::Unity
