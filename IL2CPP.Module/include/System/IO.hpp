#pragma once
#include "../MethodHandler.hpp"
#include "String.hpp"
#include "Array.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::System {

    class File {
    public:
        File() = delete;

        [[nodiscard]] static bool Exists(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.File"), IL2CPP_STR("Exists"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return false;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] static std::string ReadAllText(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.File"), IL2CPP_STR("ReadAllText"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return "";
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            void* result = MethodHandler::invoke<void*>(m, nullptr, params);
            if (!result) return "";
            return String{ result }.to_string();
        }

        static void WriteAllText(std::string_view path, std::string_view contents) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.File"), IL2CPP_STR("WriteAllText"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* pathStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* contStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(contents).c_str());
            void* params[] = { pathStr, contStr };
            MethodHandler::invoke(m, nullptr, params);
        }

        /// <summary>Read all bytes — returns raw IL2CPP byte array. Use Array&lt;uint8_t&gt; to access.</summary>
        [[nodiscard]] static void* ReadAllBytes(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.File"), IL2CPP_STR("ReadAllBytes"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return nullptr;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<void*>(m, nullptr, params);
        }

        /// <summary>Read all bytes into a native vector.</summary>
        [[nodiscard]] static std::vector<uint8_t> ReadAllBytesNative(std::string_view path) {
            void* arr = ReadAllBytes(path);
            if (!arr) return {};
            Array<uint8_t> a{ arr };
            return a.to_vector();
        }

        static void WriteAllBytes(std::string_view path, const void* data, size_t size) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.File"), IL2CPP_STR("WriteAllBytes"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew || !data || size == 0) return;
            void* pathStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* byteClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)(IL2CPP_STR("System.Byte"));
            if (!byteClass) return;
            void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(e->m_arrayNew)(byteClass, size);
            if (!arr) return;
            std::memcpy(static_cast<char*>(arr) + 0x20, data, size);
            void* params[] = { pathStr, arr };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void Delete(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.File"), IL2CPP_STR("Delete"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void Copy(std::string_view sourceFileName, std::string_view destFileName, bool overwrite = false) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.File"), IL2CPP_STR("Copy"), 3);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* srcStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(sourceFileName).c_str());
            void* dstStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(destFileName).c_str());
            void* params[] = { srcStr, dstStr, &overwrite };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void Move(std::string_view sourceFileName, std::string_view destFileName) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.File"), IL2CPP_STR("Move"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* srcStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(sourceFileName).c_str());
            void* dstStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(destFileName).c_str());
            void* params[] = { srcStr, dstStr };
            MethodHandler::invoke(m, nullptr, params);
        }
    };

    class Path {
    public:
        Path() = delete;

        [[nodiscard]] static std::string Combine(std::string_view path1, std::string_view path2) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.Path"), IL2CPP_STR("Combine"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return "";
            void* s1 = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path1).c_str());
            void* s2 = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path2).c_str());
            void* params[] = { s1, s2 };
            void* result = MethodHandler::invoke<void*>(m, nullptr, params);
            if (!result) return "";
            return String{ result }.to_string();
        }

        [[nodiscard]] static std::string GetFileName(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.Path"), IL2CPP_STR("GetFileName"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return "";
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            void* result = MethodHandler::invoke<void*>(m, nullptr, params);
            if (!result) return "";
            return String{ result }.to_string();
        }

        [[nodiscard]] static std::string GetDirectoryName(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.Path"), IL2CPP_STR("GetDirectoryName"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return "";
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            void* result = MethodHandler::invoke<void*>(m, nullptr, params);
            if (!result) return "";
            return String{ result }.to_string();
        }

        [[nodiscard]] static std::string GetExtension(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.Path"), IL2CPP_STR("GetExtension"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return "";
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            void* result = MethodHandler::invoke<void*>(m, nullptr, params);
            if (!result) return "";
            return String{ result }.to_string();
        }

        [[nodiscard]] static std::string GetFileNameWithoutExtension(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.Path"), IL2CPP_STR("GetFileNameWithoutExtension"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return "";
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            void* result = MethodHandler::invoke<void*>(m, nullptr, params);
            if (!result) return "";
            return String{ result }.to_string();
        }

        [[nodiscard]] static std::string GetTempPath() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.Path"), IL2CPP_STR("GetTempPath"), 0);
            void* result = MethodHandler::invoke<void*>(m, nullptr);
            if (!result) return "";
            return String{ result }.to_string();
        }
    };

    class Directory {
    public:
        Directory() = delete;

        [[nodiscard]] static bool Exists(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.Directory"), IL2CPP_STR("Exists"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return false;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        static void CreateDirectory(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.Directory"), IL2CPP_STR("CreateDirectory"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void Delete(std::string_view path, bool recursive = false) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.IO.Directory"), IL2CPP_STR("Delete"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str, &recursive };
            MethodHandler::invoke(m, nullptr, params);
        }
    };

} // namespace IL2CPP::Module::System
