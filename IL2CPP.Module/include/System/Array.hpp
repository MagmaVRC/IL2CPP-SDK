#pragma once
#include "../ManagedObject.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <vector>
#include <span>
#include <algorithm>
#include <ranges>
#include <concepts>
#include <cstring>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::System {

    template<typename T>
    class Array : public ManagedObject {
        static constexpr int kBoundsOffset    = 0x10;  // il2cppArrayBounds*
        static constexpr int kMaxLengthOffset = 0x18;  // uintptr_t
        static constexpr int kValuesOffset    = 0x20;  // T m_pValues (start of data inline)

        template<typename TKey, typename TValue> friend class Dictionary;
        template<typename U> friend class List;
        template<typename U> friend class HashSet;

    private:
        [[nodiscard]] T* data_internal() const {
            return reinterpret_cast<T*>(static_cast<char*>(m_native) + kValuesOffset);
        }

        [[nodiscard]] uintptr_t size_internal() const {
            return *reinterpret_cast<uintptr_t*>(static_cast<char*>(m_native) + kMaxLengthOffset);
        }

    public:
        using ManagedObject::ManagedObject;

        [[nodiscard]] uintptr_t size() const {
            if (!valid()) return 0;
            return size_internal();
        }

        [[nodiscard]] bool empty() const { return size() == 0; }

        /// <summary>Get a pointer to the start of the data.</summary>
        [[nodiscard]] T* data() {
            if (!valid()) return nullptr;
            return data_internal();
        }

        [[nodiscard]] const T* data() const {
            if (!valid()) return nullptr;
            return const_cast<Array*>(this)->data_internal();
        }

        /// <summary>Access by index (unchecked).</summary>
        [[nodiscard]] T& operator[](uintptr_t i) { return data_internal()[i]; }
        [[nodiscard]] const T& operator[](uintptr_t i) const { return const_cast<Array*>(this)->data_internal()[i]; }

        /// <summary>Safe access by index. Returns nullptr if out of bounds.</summary>
        [[nodiscard]] T* try_at(uintptr_t i) {
            if (!valid()) return nullptr;
            return i < size_internal() ? &data_internal()[i] : nullptr;
        }

        /// <summary>Get as std::span.</summary>
        [[nodiscard]] std::span<T> as_span() {
            if (!valid()) return {};
            return { data_internal(), size_internal() };
        }
        [[nodiscard]] std::span<const T> as_span() const {
            if (!valid()) return {};
            return { const_cast<Array*>(this)->data_internal(), const_cast<Array*>(this)->size_internal() };
        }


        [[nodiscard]] T* begin() { return data(); }
        [[nodiscard]] T* end() {
            if (!valid()) return nullptr;
            return data_internal() + size_internal();
        }
        [[nodiscard]] const T* begin() const { return data(); }
        [[nodiscard]] const T* end() const {
            if (!valid()) return nullptr;
            return const_cast<Array*>(this)->data_internal() + const_cast<Array*>(this)->size_internal();
        }

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

        static Array<T> Create(std::string_view elementTypeName, uintptr_t length) {
            auto* e = GetExports();
            if (!e) return {};
            void* klass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(
                e->m_helperFindClass)(std::string(elementTypeName).c_str());
            if (!klass) return {};
            void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(
                e->m_arrayNew)(klass, length);
            return Array<T>{arr};
        }

        static Array<uint8_t> FromBytes(const void* src, size_t len) requires std::same_as<T, uint8_t> {
            if (!src || len == 0) return {};
            auto arr = Create("System.Byte", len);
            if (arr) std::memcpy(arr.data(), src, len);
            return arr;
        }

        static Array<uint8_t> FromBytes(const std::vector<uint8_t>& v) requires std::same_as<T, uint8_t> {
            return FromBytes(v.data(), v.size());
        }
    };

} // namespace IL2CPP::Module::System
