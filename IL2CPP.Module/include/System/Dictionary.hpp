#pragma once
#include "Array.hpp"
#include <functional>

namespace IL2CPP::Module::System {

    template<typename TKey, typename TValue>
    class Dictionary : public ManagedObject {
    public:
        struct Entry {
            int     hashCode;
            int     next;
            TKey    key;
            TValue  value;
        };

    private:
        static constexpr int kBucketsOffset  = 0x10;
        static constexpr int kEntriesOffset  = 0x18;
        static constexpr int kCountOffset    = 0x20;
        static constexpr int kFreeListOff    = 0x24;
        static constexpr int kFreeCountOff   = 0x28;

        [[nodiscard]] Array<int> buckets() const {
            if (!valid()) return Array<int>{};
            return Array<int>{ read<void*>(kBucketsOffset) };
        }

        [[nodiscard]] Array<Entry> entries() const {
            if (!valid()) return Array<Entry>{};
            return Array<Entry>{ read<void*>(kEntriesOffset) };
        }

        [[nodiscard]] int raw_count() const {
            if (!valid()) return 0;
            return read<int>(kCountOffset);
        }

        [[nodiscard]] int free_count() const {
            if (!valid()) return 0;
            return read<int>(kFreeCountOff);
        }

    public:
        using ManagedObject::ManagedObject;

        /// <summary>Get the number of active entries.</summary>
        [[nodiscard]] int count() const { return raw_count() - free_count(); }

        [[nodiscard]] bool empty() const { return count() == 0; }

        /// <summary>Try to get a value by key. Returns nullptr if not found.</summary>
        [[nodiscard]] TValue* try_get_value(TKey key) {
            auto b = buckets();
            auto e = entries();
            if (!b || !e || b.empty()) return nullptr;

            int bucketIdx = (std::hash<TKey>{}(key) & 0x7FFFFFFF) % static_cast<int>(b.size());
            Entry* ent = e.data();
            int* buck = b.data();
            if (!ent || !buck) return nullptr;

            for (int i = buck[bucketIdx] - 1; i >= 0; i = ent[i].next) {
                if (ent[i].hashCode >= 0 && ent[i].key == key)
                    return &ent[i].value;
            }
            return nullptr;
        }

        /// <summary>Get a value by key, returns default if not found.</summary>
        [[nodiscard]] TValue get_value(TKey key) {
            auto* v = try_get_value(key);
            return v ? *v : TValue{};
        }

        /// <summary>Check if a key exists.</summary>
        [[nodiscard]] bool contains_key(TKey key) {
            return try_get_value(key) != nullptr;
        }

        /// <summary>Iterate all active entries.</summary>
        template<typename Func>
        void for_each(Func f) {
            auto e = entries();
            if (!e) return;
            int cnt = raw_count();
            int cap = static_cast<int>(e.size());
            int limit = (cnt < cap) ? cnt : cap;
            Entry* ent = e.data();
            if (!ent) return;
            for (int i = 0; i < limit; ++i) {
                if (ent[i].hashCode >= 0)
                    f(ent[i].key, ent[i].value);
            }
        }

        /// <summary>Iterate all active entries (const).</summary>
        template<typename Func>
        void for_each(Func f) const {
            auto e = entries();
            if (!e) return;
            int cnt = raw_count();
            int cap = static_cast<int>(e.size());
            int limit = (cnt < cap) ? cnt : cap;
            const Entry* ent = e.data();
            if (!ent) return;
            for (int i = 0; i < limit; ++i) {
                if (ent[i].hashCode >= 0)
                    f(ent[i].key, ent[i].value);
            }
        }
    };

} // namespace IL2CPP::Module::System
