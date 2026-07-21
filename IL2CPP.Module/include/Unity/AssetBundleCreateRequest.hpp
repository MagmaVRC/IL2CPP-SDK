#pragma once
#include "AssetBundle.hpp"
#include "AsyncOperation.hpp"
#include <exception>
#include <memory>
#include <stdexcept>

namespace IL2CPP::Module::Unity {

    /// Wraps UnityEngine.AssetBundleCreateRequest : AsyncOperation
    /// Has get_assetBundle to retrieve the loaded bundle.
    ///
    /// Runtime-type-aware: on stripped-LoadFromMemory builds the capability layer returns a
    /// UnityWebRequestAsyncOperation dressed as an AssetBundleCreateRequest. GetAssetBundle
    /// detects that and walks UnityWebRequest.downloadHandler → DownloadHandlerAssetBundle.assetBundle
    /// so callers keep the same API.
    class AssetBundleCreateRequest : public AsyncOperation {
    public:
        using AsyncOperation::AsyncOperation;

        [[nodiscard]] AssetBundle GetAssetBundle() {
            if (!raw()) return AssetBundle{};
            if (IsWebRequestAsyncOp()) {
                static auto getRequest = MethodHandler::resolve(
                    IL2CPP_STR("UnityEngine.Networking.UnityWebRequestAsyncOperation"),
                    IL2CPP_STR("get_webRequest"), 0);
                static auto getHandler = MethodHandler::resolve(
                    IL2CPP_STR("UnityEngine.Networking.UnityWebRequest"),
                    IL2CPP_STR("get_downloadHandler"), 0);
                static auto getBundle = MethodHandler::resolve(
                    IL2CPP_STR("UnityEngine.Networking.DownloadHandlerAssetBundle"),
                    IL2CPP_STR("get_assetBundle"), 0);
                void* request = MethodHandler::invoke<void*>(getRequest, raw());
                if (!request) return AssetBundle{};
                void* handler = MethodHandler::invoke<void*>(getHandler, request);
                if (!handler) return AssetBundle{};
                return AssetBundle{ MethodHandler::invoke<void*>(getBundle, handler) };
            }
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundleCreateRequest"), IL2CPP_STR("get_assetBundle"), 0);
            return AssetBundle{ MethodHandler::invoke<void*>(m, raw()) };
        }

    private:
        [[nodiscard]] bool IsWebRequestAsyncOp() const {
            static Class webRequestOp = Class::find(
                IL2CPP_STR("UnityEngine.Networking.UnityWebRequestAsyncOperation"));
            if (!webRequestOp) return false;
            auto* e = GetExports();
            if (!e || !e->m_objectGetClass) return false;
            void* actualClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(
                e->m_objectGetClass)(raw());
            return actualClass == webRequestOp.raw();
        }

    public:

        [[nodiscard]] Task<AssetBundle> Await() const {
            TaskSource<AssetBundle> source;
            auto resultRoot = std::make_shared<ManagedRoot>();
            source.KeepAlive(resultRoot);
            Task<AssetBundle> task = source.GetTask();
            if (!raw()) {
                (void)source.SetValue(AssetBundle{});
            } else if (!OnCompleted([source, resultRoot](AsyncOperation completed) {
                try {
                    if (!completed) throw std::runtime_error("AssetBundleCreateRequest completion failed");
                    AssetBundle result = AssetBundleCreateRequest{ completed.raw() }.GetAssetBundle();
                    if (result && !resultRoot->Reset(result.raw())) {
                        throw std::runtime_error("failed to root completed AssetBundle");
                    }
                    (void)source.SetValue(result);
                } catch (...) {
                    (void)source.SetException(std::current_exception());
                }
            })) {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("failed to subscribe to AssetBundleCreateRequest completion")));
            }
            return task;
        }
    };

    /// Wraps UnityEngine.AssetBundleRequest : ResourceRequest
    /// Returned by LoadAllAssetsAsync, LoadAssetWithSubAssetsAsync.
    class AssetBundleRequest : public AsyncOperation {
    public:
        using AsyncOperation::AsyncOperation;

        [[nodiscard]] Object GetAsset() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundleRequest"), IL2CPP_STR("get_asset"), 0);
            return Object{ MethodHandler::invoke<void*>(m, raw()) };
        }

        [[nodiscard]] std::vector<Object> GetAllAssets() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundleRequest"), IL2CPP_STR("get_allAssets"), 0);
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
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundleRequest"), IL2CPP_STR("get_allAssets"), 0);
            return Object::FromArray<T>(MethodHandler::invoke<void*>(m, raw()));
        }

        [[nodiscard]] Task<Object> Await() const {
            TaskSource<Object> source;
            auto resultRoot = std::make_shared<ManagedRoot>();
            source.KeepAlive(resultRoot);
            Task<Object> task = source.GetTask();
            if (!raw()) {
                (void)source.SetValue(Object{});
            } else if (!OnCompleted([source, resultRoot](AsyncOperation completed) {
                try {
                    if (!completed) throw std::runtime_error("AssetBundleRequest completion failed");
                    Object result = AssetBundleRequest{ completed.raw() }.GetAsset();
                    if (result && !resultRoot->Reset(result.raw())) {
                        throw std::runtime_error("failed to root completed asset");
                    }
                    (void)source.SetValue(result);
                } catch (...) {
                    (void)source.SetException(std::current_exception());
                }
            })) {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("failed to subscribe to AssetBundleRequest completion")));
            }
            return task;
        }

        template<typename T>
        [[nodiscard]] Task<T> AwaitAs() const {
            TaskSource<T> source;
            auto resultRoot = std::make_shared<ManagedRoot>();
            source.KeepAlive(resultRoot);
            Task<T> task = source.GetTask();
            if (!raw()) {
                (void)source.SetValue(T{});
            } else if (!OnCompleted([source, resultRoot](AsyncOperation completed) {
                try {
                    if (!completed) throw std::runtime_error("AssetBundleRequest completion failed");
                    T result = AssetBundleRequest{ completed.raw() }.template GetAssetAs<T>();
                    if (result && !resultRoot->Reset(result.raw())) {
                        throw std::runtime_error("failed to root completed asset");
                    }
                    (void)source.SetValue(result);
                } catch (...) {
                    (void)source.SetException(std::current_exception());
                }
            })) {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("failed to subscribe to AssetBundleRequest completion")));
            }
            return task;
        }
    };

    /// Wraps UnityEngine.AssetBundleUnloadOperation : AsyncOperation
    /// Returned by AssetBundle.UnloadAsync.
    class AssetBundleUnloadOperation : public AsyncOperation {
    public:
        using AsyncOperation::AsyncOperation;

        [[nodiscard]] Task<void> Await() const {
            TaskSource<void> source;
            Task<void> task = source.GetTask();
            if (!raw()) {
                (void)source.SetValue();
            } else if (!OnCompleted([source](AsyncOperation completed) {
                try {
                    if (!completed) throw std::runtime_error("AssetBundleUnloadOperation completion failed");
                    (void)source.SetValue();
                } catch (...) {
                    (void)source.SetException(std::current_exception());
                }
            })) {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("failed to subscribe to AssetBundleUnloadOperation completion")));
            }
            return task;
        }
    };

    // Logical capabilities — Core owns the async load strategy (see capability registry).
    inline AssetBundleCreateRequest AssetBundle::LoadFromFileAsync(std::string_view path, uint32_t crc, uint64_t offset) {
        System::String str = System::String::create(std::string(path).c_str());
        if (!str) return AssetBundleCreateRequest{};
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadFromFileAsync"));
        if (!m) return AssetBundleCreateRequest{};
        uint64_t offsetVal = offset;
        void* params[] = { str.raw(), &crc, &offsetVal };
        return AssetBundleCreateRequest{ MethodHandler::invoke<void*>(m, nullptr, params) };
    }

    inline AssetBundleCreateRequest AssetBundle::LoadFromMemoryAsync(const void* data, size_t size, uint32_t crc) {
        if (!data || size == 0) return AssetBundleCreateRequest{};
        auto bytes = System::Array<uint8_t>::FromBytes(data, size);  // managed byte[]
        if (!bytes) return AssetBundleCreateRequest{};
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadFromMemoryAsync"));
        if (!m) return AssetBundleCreateRequest{};
        void* arr = bytes.raw();
        void* params[] = { arr, &crc };
        return AssetBundleCreateRequest{ MethodHandler::invoke<void*>(m, nullptr, params) };
    }

    inline AssetBundleRequest AssetBundle::LoadAllAssetsAsync() {
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadAllAssetsAsync"), 0);
        return AssetBundleRequest{ MethodHandler::invoke<void*>(m, raw()) };
    }

    inline AssetBundleRequest AssetBundle::LoadAllAssetsAsync(std::string_view typeName) {
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadAllAssetsAsync"), 1);
        Class klass = Class::find(typeName);
        if (!klass) return AssetBundleRequest{};
        Type t = klass.get_type();
        if (!t) return AssetBundleRequest{};
        void* sysType = t.get_system_type_object();
        void* params[] = { sysType };
        return AssetBundleRequest{ MethodHandler::invoke<void*>(m, raw(), params) };
    }

    inline AssetBundleRequest AssetBundle::LoadAssetWithSubAssetsAsync(std::string_view name, bool performTypeChecks) {
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadAssetWithSubAssetsAsync"), 2);
        auto* e = GetExports();
        if (!e || !e->m_stringNew) return AssetBundleRequest{};
        void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
        void* params[] = { str, &performTypeChecks };
        return AssetBundleRequest{ MethodHandler::invoke<void*>(m, raw(), params) };
    }

    inline AssetBundleRequest AssetBundle::LoadAssetWithSubAssetsAsync(std::string_view name, std::string_view typeName, bool performTypeChecks) {
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("LoadAssetWithSubAssetsAsync"), 3);
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
        static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.AssetBundle"), IL2CPP_STR("UnloadAsync"), 1);
        void* params[] = { &unloadAllLoadedObjects };
        return AssetBundleUnloadOperation{ MethodHandler::invoke<void*>(m, raw(), params) };
    }

} // namespace IL2CPP::Module::Unity
