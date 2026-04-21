#pragma once
#include "../ManagedObject.hpp"
#include <vector>
#include <span>
#include <algorithm>
#include <ranges>
#include <concepts>

namespace IL2CPP::Module::System {

    template<typename T>
    class Array : public ManagedObject {
        static constexpr int kBoundsOffset    = 0x10;  // il2cppArrayBounds*
        static constexpr int kMaxLengthOffset = 0x18;  // uintptr_t
        static constexpr int kValuesOffset    = 0x20;  // T m_pValues (start of data inline)

    public:
        using ManagedObject::ManagedObject;

        [[nodiscard]] uintptr_t size() const {
            if (!valid()) return 0;
            return read<uintptr_t>(kMaxLengthOffset);
        }

        [[nodiscard]] bool empty() const { return size() == 0; }

        /// <summary>Get a pointer to the start of the data.</summary>
        [[nodiscard]] T* data() {
            if (!valid()) return nullptr;
            return reinterpret_cast<T*>(static_cast<char*>(raw()) + kValuesOffset);
        }

        [[nodiscard]] const T* data() const {
            if (!valid()) return nullptr;
            return reinterpret_cast<const T*>(static_cast<const char*>(raw()) + kValuesOffset);
        }

        /// <summary>Access by index (unchecked).</summary>
        [[nodiscard]] T& operator[](uintptr_t i) { return data()[i]; }
        [[nodiscard]] const T& operator[](uintptr_t i) const { return data()[i]; }

        /// <summary>Safe access by index. Returns nullptr if out of bounds.</summary>
        [[nodiscard]] T* try_at(uintptr_t i) {
            return i < size() ? &data()[i] : nullptr;
        }

        /// <summary>Get as std::span.</summary>
        [[nodiscard]] std::span<T> as_span() { return { data(), size() }; }
        [[nodiscard]] std::span<const T> as_span() const { return { data(), size() }; }


        [[nodiscard]] T* begin() { return data(); }
        [[nodiscard]] T* end() { return data() + size(); }
        [[nodiscard]] const T* begin() const { return data(); }
        [[nodiscard]] const T* end() const { return data() + size(); }

        /// <summary>Convert to std::vector.</summary>
        [[nodiscard]] std::vector<T> to_vector() const {
            if (!valid()) return {};
            return { begin(), end() };
        }

        /// <summary>Filter elements by predicate.</summary>
        template<typename Pred>
        [[nodiscard]] std::vector<T> filter(Pred pred) const {
            std::vector<T> result;
            for (auto& val : *this)
                if (pred(val)) result.push_back(val);
            return result;
        }

        /// <summary>Map elements via a function.</summary>
        template<typename Func>
        [[nodiscard]] auto map(Func f) const -> std::vector<std::invoke_result_t<Func, const T&>> {
            using U = std::invoke_result_t<Func, const T&>;
            std::vector<U> result;
            result.reserve(size());
            for (auto& val : *this) result.push_back(f(val));
            return result;
        }

        /// <summary>ForEach.</summary>
        template<typename Func>
        void for_each(Func f) { std::ranges::for_each(*this, f); }

        /// <summary>Check if any element matches.</summary>
        template<typename Pred>
        [[nodiscard]] bool any(Pred pred) const { return std::ranges::any_of(*this, pred); }

        /// <summary>Check if all elements match.</summary>
        template<typename Pred>
        [[nodiscard]] bool all(Pred pred) const { return std::ranges::all_of(*this, pred); }

        /// <summary>Check if contains a value.</summary>
        [[nodiscard]] bool contains(const T& value) const { return std::ranges::find(*this, value) != end(); }
    };

} // namespace IL2CPP::Module::System
