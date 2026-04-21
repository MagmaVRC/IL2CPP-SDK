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

        [[nodiscard]] Array<Slot> slots() const {
            if (!valid()) return Array<Slot>{};
            return Array<Slot>{ read<void*>(kSlotsOffset) };
        }

    public:
        using ManagedObject::ManagedObject;

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
            auto s = slots();
            if (!s) return false;
            int limit = last_index();
            int cap = static_cast<int>(s.size());
            if (limit > cap) limit = cap;
            const Slot* data = s.data();
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
            auto s = slots();
            if (!s) return;
            int limit = last_index();
            int cap = static_cast<int>(s.size());
            if (limit > cap) limit = cap;
            Slot* data = s.data();
            if (!data) return;
            for (int i = 0; i < limit; ++i) {
                if (data[i].hashCode >= 0)
                    f(data[i].value);
            }
        }

        /// <summary>Iterate all active elements (const).</summary>
        template<typename Func>
        void for_each(Func f) const {
            auto s = slots();
            if (!s) return;
            int limit = last_index();
            int cap = static_cast<int>(s.size());
            if (limit > cap) limit = cap;
            const Slot* data = s.data();
            if (!data) return;
            for (int i = 0; i < limit; ++i) {
                if (data[i].hashCode >= 0)
                    f(data[i].value);
            }
        }

        /// <summary>Convert to std::vector.</summary>
        [[nodiscard]] std::vector<T> to_vector() const {
            std::vector<T> result;
            result.reserve(count());
            for_each([&](const T& v) { result.push_back(v); });
            return result;
        }
    };

} // namespace IL2CPP::Module::System
