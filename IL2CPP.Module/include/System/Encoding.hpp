#pragma once
#include "../ManagedObject.hpp"
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

    class Encoding : public ManagedObject {
    public:
        using ManagedObject::ManagedObject;


        [[nodiscard]] static Encoding GetUTF8() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.Text.Encoding"), IL2CPP_STR("get_UTF8"), 0);
            return Encoding{ MethodHandler::invoke<void*>(m, nullptr) };
        }

        [[nodiscard]] static Encoding GetASCII() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.Text.Encoding"), IL2CPP_STR("get_ASCII"), 0);
            return Encoding{ MethodHandler::invoke<void*>(m, nullptr) };
        }

        [[nodiscard]] static Encoding GetUnicode() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.Text.Encoding"), IL2CPP_STR("get_Unicode"), 0);
            return Encoding{ MethodHandler::invoke<void*>(m, nullptr) };
        }

        [[nodiscard]] static Encoding GetDefault() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.Text.Encoding"), IL2CPP_STR("get_Default"), 0);
            return Encoding{ MethodHandler::invoke<void*>(m, nullptr) };
        }


        [[nodiscard]] void* GetBytes(std::string_view str) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.Text.Encoding"), IL2CPP_STR("GetBytes"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return nullptr;
            void* s = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(str).c_str());
            void* params[] = { s };
            return MethodHandler::invoke<void*>(m, raw(), params);
        }

        /// <summary>GetBytes as a native vector.</summary>
        [[nodiscard]] std::vector<uint8_t> GetBytesNative(std::string_view str) const {
            void* arr = GetBytes(str);
            if (!arr) return {};
            Array<uint8_t> a{ arr };
            return a.to_vector();
        }


        [[nodiscard]] std::string GetString(void* byteArray) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("System.Text.Encoding"), IL2CPP_STR("GetString"), 1);
            if (!byteArray) return "";
            void* params[] = { byteArray };
            void* result = MethodHandler::invoke<void*>(m, raw(), params);
            if (!result) return "";
            return String{ result }.to_string();
        }

        /// <summary>GetString from raw native data.</summary>
        [[nodiscard]] std::string GetStringFromNative(const void* data, size_t size) const {
            auto* e = GetExports();
            if (!e || !data || size == 0) return "";
            void* byteClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)(IL2CPP_STR("System.Byte"));
            if (!byteClass) return "";
            void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(e->m_arrayNew)(byteClass, size);
            if (!arr) return "";
            std::memcpy(static_cast<char*>(arr) + 0x20, data, size);
            return GetString(arr);
        }
    };

} // namespace IL2CPP::Module::System
