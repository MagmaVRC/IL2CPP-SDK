#pragma once
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module {

    [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept;
    class Deobfuscation {
    public:
        /// Get the stable (deobfuscated) name for an obfuscated identifier.
        /// Returns the input unchanged if no mapping exists.
        [[nodiscard]] static const char* GetStableName(const char* obfuscatedName) {
            auto* e = GetExports();
            if (!e || !e->m_helperGetStableName || !obfuscatedName) return obfuscatedName;
            return reinterpret_cast<const char*(IL2CPP_CALLTYPE)(const char*)>(
                e->m_helperGetStableName)(obfuscatedName);
        }

        /// Get the original (obfuscated) name for a stable identifier.
        /// Returns the input unchanged if no mapping exists.
        [[nodiscard]] static const char* GetOriginalName(const char* stableName) {
            auto* e = GetExports();
            if (!e || !e->m_helperGetOriginalName || !stableName) return stableName;
            return reinterpret_cast<const char*(IL2CPP_CALLTYPE)(const char*)>(
                e->m_helperGetOriginalName)(stableName);
        }

        /// Register a new obfuscated <-> deobfuscated mapping with Core.
        /// Used by ClassResolver when resolving obfuscated classes from modules.
        static void AddMapping(std::string_view obfuscated, std::string_view deobfuscated) {
            auto* e = GetExports();
            if (!e || !e->m_helperAddMapping) return;
            std::string obf(obfuscated);
            std::string deobf(deobfuscated);
            reinterpret_cast<void(IL2CPP_CALLTYPE)(const char*, const char*)>(
                e->m_helperAddMapping)(obf.c_str(), deobf.c_str());
        }

        /// Get the stable (deobfuscated) full name for a bare short obfuscated name.
        /// Returns nullptr if no mapping exists.
        [[nodiscard]] static const char* GetStableNameByShort(const char* shortName) {
            auto* e = GetExports();
            if (!e || !e->m_helperGetStableNameByShort || !shortName) return nullptr;
            return reinterpret_cast<const char*(IL2CPP_CALLTYPE)(const char*)>(
                e->m_helperGetStableNameByShort)(shortName);
        }

        /// <summary>Convenience: get the stable name by short name as a std::string (empty if not found).</summary>
        [[nodiscard]] static std::string GetStableNameByShortStr(std::string_view shortName) {
            std::string s(shortName);
            const char* result = GetStableNameByShort(s.c_str());
            return result ? result : std::string{};
        }

        /// <summary>Convenience: get the stable name as a std::string.</summary>
        [[nodiscard]] static std::string GetStableNameStr(std::string_view obfuscated) {
            std::string obf(obfuscated);
            const char* result = GetStableName(obf.c_str());
            return result ? result : std::string(obfuscated);
        }

        /// <summary>Convenience: get the original name as a std::string.</summary>
        [[nodiscard]] static std::string GetOriginalNameStr(std::string_view stableName) {
            std::string stable(stableName);
            const char* result = GetOriginalName(stable.c_str());
            return result ? result : std::string(stableName);
        }

        /// <summary>Check if a name appears to be obfuscated (contains non-ASCII / non-printable chars).</summary>
        [[nodiscard]] static bool IsObfuscated(const char* name) noexcept {
            if (!name || !*name) return false;
            for (const unsigned char* p = reinterpret_cast<const unsigned char*>(name); *p; ++p) {
                if (*p > 0x7E || *p < 0x20) return true;
            }
            return false;
        }

    private:
        Deobfuscation() = delete;
    };

}
