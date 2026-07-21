#pragma once
#include "Array.hpp"

namespace IL2CPP::Module::System {

    template<typename T>
    class List : public ManagedObject {
        static constexpr int kItemsOffset   = 0x10;  // il2cppArray<T>*
        static constexpr int kSizeOffset    = 0x18;  // int m_iSize
        static constexpr int kVersionOffset = 0x1C;  // int m_iVersion

        [[nodiscard]] T* items_data() const {
            void* arr = *reinterpret_cast<void**>(static_cast<char*>(m_native) + kItemsOffset);
            if (!IsValidPointer(arr)) return nullptr;
            return reinterpret_cast<T*>(static_cast<char*>(arr) + Array<T>::ValuesOffset());
        }

    public:
        using ManagedObject::ManagedObject;

        /// <summary>Get the internal array as an Array&lt;T&gt; handle.</summary>
        [[nodiscard]] Array<T> items() const {
            if (!valid()) return Array<T>{};
            return Array<T>{ read<void*>(kItemsOffset) };
        }

        /// <summary>Get the list count (logical size, not capacity).</summary>
        [[nodiscard]] int count() const {
            if (!valid()) return 0;
            return read<int>(kSizeOffset);
        }

        [[nodiscard]] bool empty() const { return count() == 0; }

        /// <summary>Access by index through the internal array.</summary>
        [[nodiscard]] T& operator[](uintptr_t i) { return items_data()[i]; }
        [[nodiscard]] const T& operator[](uintptr_t i) const { return items_data()[i]; }

        /// <summary>Safe access.</summary>
        [[nodiscard]] T* try_at(uintptr_t i) {
            if (!valid()) return nullptr;
            int cnt = read<int>(kSizeOffset);
            if (i >= static_cast<uintptr_t>(cnt)) return nullptr;
            T* d = items_data();
            return d ? &d[i] : nullptr;
        }

        /// <summary>Get as std::span over the logical count.</summary>
        [[nodiscard]] std::span<T> as_span() {
            if (!valid()) return {};
            T* d = items_data();
            return d ? std::span<T>{ d, static_cast<size_t>(read<int>(kSizeOffset)) } : std::span<T>{};
        }
        [[nodiscard]] std::span<const T> as_span() const {
            if (!valid()) return {};
            T* d = items_data();
            return d ? std::span<const T>{ d, static_cast<size_t>(read<int>(kSizeOffset)) } : std::span<const T>{};
        }


        [[nodiscard]] T* begin() {
            if (!valid()) return nullptr;
            return items_data();
        }
        [[nodiscard]] T* end() {
            if (!valid()) return nullptr;
            T* d = items_data();
            return d ? d + read<int>(kSizeOffset) : nullptr;
        }
        [[nodiscard]] const T* begin() const {
            if (!valid()) return nullptr;
            return items_data();
        }
        [[nodiscard]] const T* end() const {
            if (!valid()) return nullptr;
            T* d = items_data();
            return d ? d + read<int>(kSizeOffset) : nullptr;
        }

        /// <summary>Convert to std::vector.</summary>
        [[nodiscard]] std::vector<T> to_vector() const {
            if (!valid()) return {};
            T* d = items_data();
            if (!d) return {};
            int cnt = read<int>(kSizeOffset);
            return { d, d + cnt };
        }
    };

} // namespace IL2CPP::Module::System
