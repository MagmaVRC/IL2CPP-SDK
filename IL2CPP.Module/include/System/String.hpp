#pragma once
#include "../ManagedObject.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>
#include <windows.h>

namespace IL2CPP::Module::System {

    class String : public ManagedObject {
    public:
        using ManagedObject::ManagedObject;

        /// <summary>Get the string length (char count).</summary>
        [[nodiscard]] int length() const {
            if (!valid()) return 0;
            return read<int>(0x10);
        }

        /// <summary>Get a pointer to the internal wide character buffer.</summary>
        [[nodiscard]] const wchar_t* chars() const {
            if (!valid()) return nullptr;
            return ptr_at<const wchar_t>(0x14);
        }

        /// <summary>Convert to std::wstring.</summary>
        [[nodiscard]] std::wstring to_wstring() const {
            int len = length();
            if (len <= 0 || !chars()) return L"";
            return std::wstring(chars(), len);
        }

        /// <summary>Convert to std::string (UTF-8).</summary>
        [[nodiscard]] std::string to_string() const {
            auto ws = to_wstring();
            if (ws.empty()) return {};
            int utf8Len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), static_cast<int>(ws.size()), nullptr, 0, nullptr, nullptr);
            if (utf8Len <= 0) return {};
            std::string out(static_cast<size_t>(utf8Len), '\0');
            WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), static_cast<int>(ws.size()), out.data(), utf8Len, nullptr, nullptr);
            return out;
        }

        /// <summary>Implicit conversion to std::string.</summary>
        [[nodiscard]] operator std::string() const { return to_string(); }

        /// <summary>Clear the string content in-place.</summary>
        void clear() {
            if (!valid()) return;
            write<int>(0x10, 0);
        }

        /// <summary>Create a new managed String from a C string.</summary>
        [[nodiscard]] static String create(const char* str) {
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew || !str) return String{};
            return String{ reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(str) };
        }

        /// <summary>Create a new managed String from a std::string_view.</summary>
        [[nodiscard]] static String create(std::string_view str) {
            return create(std::string(str).c_str());
        }
    };

    [[nodiscard]] inline String operator""_il(const char* str, std::size_t) {
        return String::create(str);
    }

} // namespace IL2CPP::Module::System
