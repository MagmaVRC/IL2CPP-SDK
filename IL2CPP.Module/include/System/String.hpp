#pragma once
#include "../ManagedObject.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>
#include <windows.h>

namespace IL2CPP::Module::System {

    class String : public ManagedObject {
        static constexpr int kLengthOffset = 0x10;
        static constexpr int kCharsOffset  = 0x14;
        static constexpr int kMaxLength    = 0x800000;

    public:
        using ManagedObject::ManagedObject;

        /// <summary>Get the string length (char count), or 0 if the object is not a plausible String.</summary>
        [[nodiscard]] int length() const {
            if (!valid()) return 0;
            const int len = read<int>(kLengthOffset);
            return (len < 0 || len > kMaxLength) ? 0 : len;
        }

        /// <summary>Get a pointer to the internal wide character buffer.</summary>
        [[nodiscard]] const wchar_t* chars() const {
            if (!valid()) return nullptr;
            return ptr_at<const wchar_t>(kCharsOffset);
        }

        /// <summary>Convert to std::wstring.</summary>
        [[nodiscard]] std::wstring to_wstring() const {
            const int len = length();
            if (len <= 0) return L"";
            return std::wstring(ptr_at<const wchar_t>(kCharsOffset), len);
        }

        /// <summary>Convert to std::string (UTF-8).</summary>
        [[nodiscard]] std::string to_string() const {
            const int len = length();
            if (len <= 0) return {};
            const wchar_t* wc = ptr_at<const wchar_t>(kCharsOffset);
            int ascii = 0;
            while (ascii < len && wc[ascii] < 0x80) ++ascii;
            if (ascii == len) {
                std::string out;
                out.resize_and_overwrite(static_cast<size_t>(len), [wc](char* p, size_t n) {
                    for (size_t i = 0; i < n; ++i) p[i] = static_cast<char>(wc[i]);
                    return n;
                });
                return out;
            }
            const int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wc, len, nullptr, 0, nullptr, nullptr);
            if (utf8Len <= 0) return {};
            std::string out;
            out.resize_and_overwrite(static_cast<size_t>(utf8Len), [&](char* p, size_t n) {
                return static_cast<size_t>(WideCharToMultiByte(CP_UTF8, 0, wc, len, p, static_cast<int>(n), nullptr, nullptr));
            });
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

        /// <summary>Create a new managed String from UTF-16 units.</summary>
        /// <param name="len">Unit count; the text need not be null-terminated.</param>
        /// <returns>An invalid String when the runtime does not export the UTF-16 constructor.</returns>
        [[nodiscard]] static String create_utf16(const char16_t* text, int32_t len) {
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNewUtf16 || !text || len < 0) return String{};
            return String{ reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char16_t*, int32_t)>(
                exports->m_stringNewUtf16)(text, len) };
        }
    };

    [[nodiscard]] inline String operator""_il(const char* str, std::size_t) {
        return String::create(str);
    }

} // namespace IL2CPP::Module::System
