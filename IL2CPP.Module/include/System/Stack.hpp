#pragma once
#include "Array.hpp"

namespace IL2CPP::Module::System {

    template<typename T>
    class Stack : public ManagedObject {
        static constexpr int kArrayOffset   = 0x10;
        static constexpr int kSizeOffset    = 0x18;

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

        /// <summary>Peek at the top element without popping.</summary>
        [[nodiscard]] T* peek() {
            if (empty()) return nullptr;
            auto arr = internal_array();
            if (!arr) return nullptr;
            return arr.try_at(count() - 1);
        }

        /// <summary>Convert to std::vector (top of stack is last element).</summary>
        [[nodiscard]] std::vector<T> to_vector() const {
            if (empty()) return {};
            auto arr = internal_array();
            if (!arr) return {};
            int sz = count();
            std::vector<T> result;
            result.reserve(sz);
            const T* data = arr.data();
            if (!data) return {};
            for (int i = 0; i < sz; ++i) {
                result.push_back(data[i]);
            }
            return result;
        }

        /// <summary>Iterate all elements from bottom to top.</summary>
        template<typename Func>
        void for_each(Func f) const {
            if (empty()) return;
            auto arr = internal_array();
            if (!arr) return;
            int sz = count();
            const T* data = arr.data();
            if (!data) return;
            for (int i = 0; i < sz; ++i) {
                f(data[i]);
            }
        }
    };

} // namespace IL2CPP::Module::System
