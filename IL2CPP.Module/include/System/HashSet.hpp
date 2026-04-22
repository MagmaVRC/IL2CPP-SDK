#pragma once
#include "Array.hpp"
#include <functional>
#include <vector>

namespace IL2CPP::Module::System {

    template<typename T>
    class HashSet : public ManagedObject {
    public:
        struct Slot {
            int     hashCode;
            int     next;
            T       value;
        };

    private:
        static constexpr int kBucketsOffset   = 0x10;
        static constexpr int kSlotsOffset     = 0x18;
        static constexpr int kCountOffset     = 0x20;
        static constexpr int kLastIndexOffset = 0x24;
        static constexpr int kFreeListOffset  = 0x28;

        struct SlotsView {
            Slot* data;
            int   limit;
        };

        [[nodiscard]] SlotsView get_slots_view() const {
            void* sArr = read<void*>(kSlotsOffset);
            if (!IsValidPointer(sArr)) return { nullptr, 0 };
            int limit = read<int>(kLastIndexOffset);
            int cap = static_cast<int>(*reinterpret_cast<uintptr_t*>(
                static_cast<char*>(sArr) + Array<Slot>::kMaxLengthOffset));
            if (limit > cap) limit = cap;
            return {
                reinterpret_cast<Slot*>(static_cast<char*>(sArr) + Array<Slot>::kValuesOffset),
                limit
            };
        }

    public:
        using ManagedObject::ManagedObject;

        /// <summary>Get the internal slots array as an Array handle.</summary>
        [[nodiscard]] Array<Slot> slots() const {
            if (!valid()) return Array<Slot>{};
            return Array<Slot>{ read<void*>(kSlotsOffset) };
        }

        /// <summary>Get the number of active elements.</summary>
        [[nodiscard]] int count() const {
            if (!valid()) return 0;
            return read<int>(kCountOffset);
        }

        [[nodiscard]] int last_index() const {
            if (!valid()) return 0;
            return read<int>(kLastIndexOffset);
        }

        [[nodiscard]] bool empty() const { return count() == 0; }

        /// <summary>Check if the set contains a value (linear scan).</summary>
        [[nodiscard]] bool contains(const T& value) const {
            if (!valid()) return false;
            auto [data, limit] = get_slots_view();
            if (!data) return false;
            for (int i = 0; i < limit; ++i) {
                if (data[i].hashCode >= 0 && data[i].value == value)
                    return true;
            }
            return false;
        }

        /// <summary>Iterate all active elements.</summary>
        template<typename Func>
        void for_each(Func f) {
            if (!valid()) return;
            auto [data, limit] = get_slots_view();
            if (!data) return;
            for (int i = 0; i < limit; ++i) {
                if (data[i].hashCode >= 0)
                    f(data[i].value);
            }
        }

        /// <summary>Iterate all active elements (const).</summary>
        template<typename Func>
        void for_each(Func f) const {
            if (!valid()) return;
            auto [data, limit] = get_slots_view();
            if (!data) return;
            for (int i = 0; i < limit; ++i) {
                if (data[i].hashCode >= 0)
                    f(data[i].value);
            }
        }

        /// <summary>Convert to std::vector.</summary>
        [[nodiscard]] std::vector<T> to_vector() const {
            if (!valid()) return {};
            std::vector<T> result;
            result.reserve(read<int>(kCountOffset));
            auto [data, limit] = get_slots_view();
            if (!data) return result;
            for (int i = 0; i < limit; ++i) {
                if (data[i].hashCode >= 0)
                    result.push_back(data[i].value);
            }
            return result;
        }
    };

} // namespace IL2CPP::Module::System
