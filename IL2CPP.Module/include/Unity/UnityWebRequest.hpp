#pragma once
#include "Object.hpp"
#include "../MethodHandler.hpp"
#include "../System/String.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module::Unity {

    class UnityWebRequestAsyncOperation;  // Forward declaration

    class DownloadHandler : public Object {
    public:
        using Object::Object;

        [[nodiscard]] std::string GetText() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.DownloadHandler", "get_text", 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] void* GetData() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.DownloadHandler", "get_data", 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
    };

    class UploadHandler : public Object {
    public:
        using Object::Object;

        [[nodiscard]] std::string GetContentType() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UploadHandler", "get_contentType", 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            return System::String{ str }.to_string();
        }
        void SetContentType(std::string_view type) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UploadHandler", "set_contentType", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(type).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, raw(), params);
        }
    };

    class UnityWebRequest : public Object {
    public:
        using Object::Object;

        [[nodiscard]] static UnityWebRequest Get(std::string_view url) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "Get", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return UnityWebRequest{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(url).c_str());
            void* params[] = { str };
            return UnityWebRequest{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        [[nodiscard]] static UnityWebRequest Post(std::string_view url, std::string_view postData) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "Post", 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return UnityWebRequest{};
            void* urlStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(url).c_str());
            void* dataStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(postData).c_str());
            void* params[] = { urlStr, dataStr };
            return UnityWebRequest{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        [[nodiscard]] static UnityWebRequest Put(std::string_view url, std::string_view bodyData) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "Put", 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return UnityWebRequest{};
            void* urlStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(url).c_str());
            void* dataStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(bodyData).c_str());
            void* params[] = { urlStr, dataStr };
            return UnityWebRequest{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        [[nodiscard]] static UnityWebRequest Delete(std::string_view url) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "Delete", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return UnityWebRequest{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(url).c_str());
            void* params[] = { str };
            return UnityWebRequest{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        [[nodiscard]] static UnityWebRequest Head(std::string_view url) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "Head", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return UnityWebRequest{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(url).c_str());
            void* params[] = { str };
            return UnityWebRequest{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        [[nodiscard]] UnityWebRequestAsyncOperation SendWebRequest();

        [[nodiscard]] std::string GetUrl() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_url", 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] std::string GetMethod() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_method", 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] std::string GetError() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_error", 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] long long GetResponseCode() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_responseCode", 0);
            return MethodHandler::invoke<long long>(m, raw());
        }

        [[nodiscard]] bool GetIsDone() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_isDone", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] bool GetIsNetworkError() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_isNetworkError", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        [[nodiscard]] bool GetIsHttpError() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_isHttpError", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] float GetDownloadProgress() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_downloadProgress", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        [[nodiscard]] float GetUploadProgress() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_uploadProgress", 0);
            return MethodHandler::invoke<float>(m, raw());
        }

        [[nodiscard]] DownloadHandler GetDownloadHandler() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_downloadHandler", 0);
            return DownloadHandler{ MethodHandler::invoke<void*>(m, raw()) };
        }
        void SetDownloadHandler(DownloadHandler handler) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "set_downloadHandler", 1);
            void* r = handler.raw();
            void* params[] = { r };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] UploadHandler GetUploadHandler() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_uploadHandler", 0);
            return UploadHandler{ MethodHandler::invoke<void*>(m, raw()) };
        }
        void SetUploadHandler(UploadHandler handler) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "set_uploadHandler", 1);
            void* r = handler.raw();
            void* params[] = { r };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetRequestHeader(std::string_view name, std::string_view value) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "SetRequestHeader", 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* nameStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* valStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(value).c_str());
            void* params[] = { nameStr, valStr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::string GetResponseHeader(std::string_view name) const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "GetResponseHeader", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return "";
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            void* result = MethodHandler::invoke<void*>(m, raw(), params);
            if (!result) return "";
            return System::String{ result }.to_string();
        }

        void Abort() {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "Abort", 0);
            MethodHandler::invoke(m, raw());
        }

        void Dispose() {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "Dispose", 0);
            MethodHandler::invoke(m, raw());
        }

        [[nodiscard]] int GetTimeout() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "get_timeout", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetTimeout(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "set_timeout", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
    };

    class UnityWebRequestAsyncOperation : public Object {
    public:
        using Object::Object;

        [[nodiscard]] bool GetIsDone() const {
            static auto m = MethodHandler::resolve("UnityEngine.AsyncOperation", "get_isDone", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] float GetProgress() const {
            static auto m = MethodHandler::resolve("UnityEngine.AsyncOperation", "get_progress", 0);
            return MethodHandler::invoke<float>(m, raw());
        }

        [[nodiscard]] UnityWebRequest GetWebRequest() const {
            static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequestAsyncOperation", "get_webRequest", 0);
            return UnityWebRequest{ MethodHandler::invoke<void*>(m, raw()) };
        }
    };

    inline UnityWebRequestAsyncOperation UnityWebRequest::SendWebRequest() {
        static auto m = MethodHandler::resolve("UnityEngine.Networking.UnityWebRequest", "SendWebRequest", 0);
        return UnityWebRequestAsyncOperation{ MethodHandler::invoke<void*>(m, raw()) };
    }

} // namespace IL2CPP::Module::Unity
