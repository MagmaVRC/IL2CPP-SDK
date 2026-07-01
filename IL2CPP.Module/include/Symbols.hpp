#pragma once
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <cstdint>

namespace IL2CPP::Module {

    // Provided by the Module layer (also declared in Deobfuscation.hpp).
    [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept;

    // Module-side facade over the Core Symbol Store. Publishes/looks up
    // overlay-resolved classes/fields/methods by human name through the
    // export-helper table. All validation happens Core-side at publish time.
    class Symbols {
        using PubFn = void(IL2CPP_CALLTYPE)(uint32_t, const char*, const char*, void*, int64_t);
        using LkpFn = void*(IL2CPP_CALLTYPE)(uint32_t, const char*, const char*);
    public:
        static void Publish(const char* human, void* rawClass, uint32_t instanceSize) {
            auto* e = GetExports(); if (!e || !e->m_helperSymbolPublish) return;
            reinterpret_cast<PubFn>(e->m_helperSymbolPublish)(0, human, nullptr, rawClass, static_cast<int64_t>(instanceSize));
        }
        static void PublishField(const char* human, const char* member, int32_t offset, void* raw) {
            auto* e = GetExports(); if (!e || !e->m_helperSymbolPublish) return;
            reinterpret_cast<PubFn>(e->m_helperSymbolPublish)(1, human, member, raw, static_cast<int64_t>(offset));
        }
        static void PublishMethod(const char* human, const char* member, void* ptr, void* raw) {
            auto* e = GetExports(); if (!e || !e->m_helperSymbolPublish) return;
            reinterpret_cast<PubFn>(e->m_helperSymbolPublish)(2, human, member, ptr, reinterpret_cast<int64_t>(raw));
        }
        [[nodiscard]] static void* Class(const char* human) {
            auto* e = GetExports(); if (!e || !e->m_helperSymbolLookup) return nullptr;
            return reinterpret_cast<LkpFn>(e->m_helperSymbolLookup)(0, human, nullptr);
        }
        [[nodiscard]] static int32_t Field(const char* human, const char* member) {
            auto* e = GetExports(); if (!e || !e->m_helperSymbolLookup) return -1;
            void* r = reinterpret_cast<LkpFn>(e->m_helperSymbolLookup)(1, human, member);
            return r ? static_cast<int32_t>(reinterpret_cast<intptr_t>(r) - 1) : -1;
        }
        [[nodiscard]] static void* Method(const char* human, const char* member) {
            auto* e = GetExports(); if (!e || !e->m_helperSymbolLookup) return nullptr;
            return reinterpret_cast<LkpFn>(e->m_helperSymbolLookup)(2, human, member);
        }
        [[nodiscard]] static bool Has(const char* human) { return Class(human) != nullptr; }
    };

}
