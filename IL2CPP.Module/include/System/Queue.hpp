#pragma once
#include "Array.hpp"

namespace IL2CPP::Module::System {

    template<typename T>
    class Queue : public ManagedObject {
        static constexpr int kArrayOffset   = 0x10;
        static constexpr int kHeadOffset    = 0x18;
        static constexpr int kTailOffset    = 0x1C;
        static constexpr int kSizeOffset    = 0x20;

    public:
        using ManagedObject::ManagedObject;

        [[nodiscard]] int count() const {
            if (!valid()) return 0;
            return read<int>(kSizeOffset);
        }

        [[nodiscard]] bool empty() const { return count() == 0; }

        [[nodiscard]] Array<T> internal_array() const {
            if (!valid()) return Array<T>{};
            return Array<T>{ read<void*>(kArrayOffset) };
        }

        [[nodiscard]] int head() const {
            if (!valid()) return 0;
            return read<int>(kHeadOffset);
        }

        [[nodiscard]] int tail() const {
            if (!valid()) return 0;
            return read<int>(kTailOffset);
        }

        /// <summary>Peek at the front element without dequeuing.</summary>
        [[nodiscard]] T* peek() {
            if (empty()) return nullptr;
            auto arr = internal_array();
            if (!arr) return nullptr;
            return arr.try_at(head());
        }

        /// <summary>Convert to std::vector (preserves queue order).</summary>
        [[nodiscard]] std::vector<T> to_vector() const {
            if (empty()) return {};
            auto arr = internal_array();
            if (!arr) return {};
            int sz = count();
            int h = head();
            int cap = static_cast<int>(arr.size());
            std::vector<T> result;
            result.reserve(sz);
            T* data = arr.data();
            if (!data) return {};
            for (int i = 0; i < sz; ++i) {
                result.push_back(data[(h + i) % cap]);
            }
            return result;
        }

        /// <summary>Iterate all elements in queue order.</summary>
        template<typename Func>
        void for_each(Func f) const {
            if (empty()) return;
            auto arr = internal_array();
            if (!arr) return;
            int sz = count();
            int h = head();
            int cap = static_cast<int>(arr.size());
            const T* data = arr.data();
            if (!data) return;
            for (int i = 0; i < sz; ++i) {
                f(data[(h + i) % cap]);
            }
        }
    };

} // namespace IL2CPP::Module::System
