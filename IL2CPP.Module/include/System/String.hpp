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

        /// Get the string length (char count).
        [[nodiscard]] int length() const {
            if (!valid()) return 0;
            return read<int>(0x10);
        }

        /// Get a pointer to the internal wide character buffer.
        [[nodiscard]] const wchar_t* chars() const {
            if (!valid()) return nullptr;
            return ptr_at<const wchar_t>(0x14);
        }

        /// Convert to std::wstring.
        [[nodiscard]] std::wstring to_wstring() const {
            int len = length();
            if (len <= 0 || !chars()) return L"";
            return std::wstring(chars(), len);
        }

        /// Convert to std::string (UTF-8).
        [[nodiscard]] std::string to_string() const {
            auto ws = to_wstring();
            if (ws.empty()) return {};
            int utf8Len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), static_cast<int>(ws.size()), nullptr, 0, nullptr, nullptr);
            if (utf8Len <= 0) return {};
            std::string out(static_cast<size_t>(utf8Len), '\0');
            WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), static_cast<int>(ws.size()), out.data(), utf8Len, nullptr, nullptr);
            return out;
        }

        /// Implicit conversion to std::string.
        [[nodiscard]] operator std::string() const { return to_string(); }

        /// Clear the string content in-place.
        void clear() {
            if (!valid()) return;
            write<int>(0x10, 0);
        }

        /// Create a new managed String from a C string.
        [[nodiscard]] static String create(const char* str) {
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew || !str) return String{};
            return String{ reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(str) };
        }

        /// Create a new managed String from a std::string_view.
        [[nodiscard]] static String create(std::string_view str) {
            return create(std::string(str).c_str());
        }
    };

    [[nodiscard]] inline String operator""_il(const char* str, std::size_t) {
        return String::create(str);
    }

} // namespace IL2CPP::Module::System
