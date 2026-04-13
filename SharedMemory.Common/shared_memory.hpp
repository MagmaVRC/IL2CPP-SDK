#pragma once

#include <windows.h>
#include <Psapi.h>
#include <cstdint>
#include <cstring>

#pragma comment(lib, "Psapi.lib")

namespace SharedMemory {

    inline constexpr uint32_t kMagic    = 0x554E4978;
    inline constexpr uint32_t kVersion  = 1;
    inline constexpr uint32_t kCapacity = 32;

    struct RegistryEntry {
        char     name[64];
        void*    pointer;
        uint32_t size;
    };

    struct Registry {
        uint32_t       magic;
        uint32_t       version;
        uint32_t       count;
        uint32_t       capacity;
        SRWLOCK        lock;
        RegistryEntry  entries[kCapacity];
    };

    namespace detail {

        inline uint16_t crc16(uint8_t const* data, size_t len) {
            uint16_t crc = 0xFFFF;
            for (size_t i = 0; i < len; ++i) {
                crc ^= static_cast<uint16_t>(data[i]) << 8;
                for (int b = 0; b < 8; ++b) {
                    if (crc & 0x8000)
                        crc = (crc << 1) ^ 0x1021;
                    else
                        crc <<= 1;
                }
            }
            return crc;
        }

        inline bool get_game_assembly(uintptr_t& base, uint32_t& sizeOfImage) {
            HMODULE hGA = GetModuleHandleA("GameAssembly.dll");
            if (!hGA) return false;

            MODULEINFO mi{};
            if (!GetModuleInformation(GetCurrentProcess(), hGA, &mi, sizeof(mi)))
                return false;

            base = reinterpret_cast<uintptr_t>(hGA);
            sizeOfImage = static_cast<uint32_t>(mi.SizeOfImage);
            return true;
        }

        inline uint64_t derive_key(uintptr_t ga_base, uint32_t sizeOfImage) {
            uint64_t pid = static_cast<uint64_t>(GetCurrentProcessId());
            return ga_base ^ (static_cast<uint64_t>(sizeOfImage) << 13)
                           ^ (pid * 0x9E3779B97F4A7C15ULL);
        }

        inline uint64_t encode_pointer(void* ptr, uint64_t key) {
            return reinterpret_cast<uint64_t>(ptr) ^ key;
        }

        inline void* decode_pointer(uint64_t encoded, uint64_t key) {
            return reinterpret_cast<void*>(encoded ^ key);
        }

        inline bool write_dos_header(uintptr_t ga_base, void* registry, uint64_t key) {
            auto* dos = reinterpret_cast<uint8_t*>(ga_base);

            DWORD oldProtect = 0;
            if (!VirtualProtect(dos, 0x10, PAGE_READWRITE, &oldProtect))
                return false;

            auto* sentinel = reinterpret_cast<volatile LONG*>(dos + 0x0C);
            LONG original = *sentinel;

            if (static_cast<uint32_t>(original) == kMagic) {
                VirtualProtect(dos, 0x10, oldProtect, &oldProtect);
                return false;
            }

            uint64_t encoded = encode_pointer(registry, key);
            uint16_t crc     = crc16(reinterpret_cast<uint8_t const*>(&registry), 8);
            memcpy(dos + 0x02, &encoded, 8);
            memcpy(dos + 0x0A, &crc, 2);

            LONG prev = InterlockedCompareExchange(sentinel, static_cast<LONG>(kMagic), original);

            VirtualProtect(dos, 0x10, oldProtect, &oldProtect);

            if (prev != original) {
                return false;
            }
            return true;
        }

        inline Registry* read_dos_header(uintptr_t ga_base, uint64_t key) {
            auto* dos = reinterpret_cast<uint8_t const*>(ga_base);

            uint32_t sentinel = 0;
            memcpy(&sentinel, dos + 0x0C, 4);
            if (sentinel != kMagic) return nullptr;

            uint64_t encoded = 0;
            memcpy(&encoded, dos + 0x02, 8);

            void* decoded = decode_pointer(encoded, key);

            uint16_t stored_crc = 0;
            memcpy(&stored_crc, dos + 0x0A, 2);
            uint16_t computed_crc = crc16(reinterpret_cast<uint8_t const*>(&decoded), 8);

            if (stored_crc != computed_crc) return nullptr;

            auto* reg = static_cast<Registry*>(decoded);
            if (reg->magic != kMagic) return nullptr;

            return reg;
        }

    } // namespace detail

    inline Registry* GetOrCreateRegistry() {
        uintptr_t ga_base = 0;
        uint32_t  sizeOfImage = 0;
        if (!detail::get_game_assembly(ga_base, sizeOfImage))
            return nullptr;

        uint64_t key = detail::derive_key(ga_base, sizeOfImage);

        Registry* existing = detail::read_dos_header(ga_base, key);
        if (existing) return existing;

        auto* reg = static_cast<Registry*>(
            VirtualAlloc(nullptr, sizeof(Registry),
                         MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        if (!reg) return nullptr;

        memset(reg, 0, sizeof(Registry));
        reg->magic    = kMagic;
        reg->version  = kVersion;
        reg->count    = 0;
        reg->capacity = kCapacity;
        InitializeSRWLock(&reg->lock);

        if (!detail::write_dos_header(ga_base, reg, key)) {
            VirtualFree(reg, 0, MEM_RELEASE);
            return detail::read_dos_header(ga_base, key);
        }

        return reg;
    }

    inline Registry* FindRegistry() {
        uintptr_t ga_base = 0;
        uint32_t  sizeOfImage = 0;
        if (!detail::get_game_assembly(ga_base, sizeOfImage))
            return nullptr;

        uint64_t key = detail::derive_key(ga_base, sizeOfImage);
        return detail::read_dos_header(ga_base, key);
    }

    template<typename T>
    T* Register(char const* name) {
        Registry* reg = GetOrCreateRegistry();
        if (!reg) return nullptr;

        AcquireSRWLockExclusive(&reg->lock);

        for (uint32_t i = 0; i < reg->count; ++i) {
            if (strncmp(reg->entries[i].name, name, 63) == 0) {
                auto* ptr = static_cast<T*>(reg->entries[i].pointer);
                ReleaseSRWLockExclusive(&reg->lock);
                return ptr;
            }
        }

        if (reg->count >= reg->capacity) {
            ReleaseSRWLockExclusive(&reg->lock);
            return nullptr;
        }

        auto* mem = static_cast<T*>(
            VirtualAlloc(nullptr, sizeof(T),
                         MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        if (!mem) {
            ReleaseSRWLockExclusive(&reg->lock);
            return nullptr;
        }
        memset(mem, 0, sizeof(T));

        auto& entry = reg->entries[reg->count++];
        strncpy_s(entry.name, name, 63);
        entry.pointer = mem;
        entry.size    = static_cast<uint32_t>(sizeof(T));

        ReleaseSRWLockExclusive(&reg->lock);
        return mem;
    }

    template<typename T>
    T* Register(char const* name, T const& src) {
        T* ptr = Register<T>(name);
        if (ptr) memcpy(ptr, &src, sizeof(T));
        return ptr;
    }

    template<typename T>
    T const* Resolve(char const* name) {
        Registry* reg = FindRegistry();
        if (!reg) return nullptr;

        AcquireSRWLockShared(&reg->lock);

        for (uint32_t i = 0; i < reg->count; ++i) {
            if (strncmp(reg->entries[i].name, name, 63) == 0) {
                auto* ptr = static_cast<T const*>(reg->entries[i].pointer);
                ReleaseSRWLockShared(&reg->lock);
                return ptr;
            }
        }

        ReleaseSRWLockShared(&reg->lock);
        return nullptr;
    }

    inline void Unregister(char const* name) {
        Registry* reg = FindRegistry();
        if (!reg) return;

        AcquireSRWLockExclusive(&reg->lock);

        for (uint32_t i = 0; i < reg->count; ++i) {
            if (strncmp(reg->entries[i].name, name, 63) == 0) {
                if (reg->entries[i].pointer) {
                    VirtualFree(reg->entries[i].pointer, 0, MEM_RELEASE);
                }
                for (uint32_t j = i; j + 1 < reg->count; ++j) {
                    reg->entries[j] = reg->entries[j + 1];
                }
                --reg->count;
                memset(&reg->entries[reg->count], 0, sizeof(RegistryEntry));
                break;
            }
        }

        ReleaseSRWLockExclusive(&reg->lock);
    }

} // namespace SharedMemory
