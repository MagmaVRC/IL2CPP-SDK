#pragma once

//  UniTask.hpp - Calling VRChat's async methods and finding out how they ended.
//
//  A UniTask-returning method cannot go through runtime_invoke: the return is an
//  inflated generic struct, and boxing it faults. These call the method's own code
//  with the hidden return-buffer argument, then keep the task so its outcome can be
//  read -- polled rather than continued, because a native delegate does not survive
//  the way UniTask invokes one.

#include "../ManagedObject.hpp"
#include "../MethodHandler.hpp"
#include "../Reflection.hpp"

#include <cstdint>
#include <functional>
#include <initializer_list>

namespace IL2CPP::Module::Cysharp {

    enum class UniTaskStatus : int32_t {
        Pending = 0,
        Succeeded = 1,
        Faulted = 2,
        Canceled = 3,
    };

    /// One in-flight task. Holds a GC root on its source, so it must be released.
    class UniTaskHandle {
    public:
        UniTaskHandle() = default;

        [[nodiscard]] bool Valid() const noexcept { return m_source != nullptr; }

        /// <summary>Where the task has got to.</summary>
        /// <returns>Succeeded for a task that completed synchronously and carries no
        /// source of its own.</returns>
        [[nodiscard]] UniTaskStatus Status() const;

        /// <summary>The value a UniTask&lt;T&gt; produced, for a reference-typed T.</summary>
        [[nodiscard]] void* Result() const noexcept { return m_result; }

        /// <summary>Drop the GC root. Safe to call more than once.</summary>
        void Release();

    private:
        friend class UniTask;
        void* m_source = nullptr;
        void* m_result = nullptr;
        // GCHandle by value: naming the alias here would pull il2cpp_module.hpp into
        // every consumer of this header.
        uintptr_t m_root = 0;
        int16_t m_token = 0;
    };

    class UniTask {
    public:
        /// <summary>Call a method that returns UniTask or UniTask&lt;T&gt; and keep the
        /// task.</summary>
        /// <param name="obj">Instance, or nullptr for a static method.</param>
        /// <param name="args">Native argument shapes, as MethodHandler::invoke_discard
        /// documents: the value itself for a value type, not its address.</param>
        /// <returns>An invalid handle when the method could not be called at all.</returns>
        [[nodiscard]] static UniTaskHandle Start(const Method& method, void* obj,
                                                 std::initializer_list<void*> args = {});

        /// <summary>Run `done` once the task settles, on the thread that pumps. The handle
        /// is released for you afterwards.</summary>
        static void Then(const UniTaskHandle& handle, std::function<void(UniTaskStatus)> done);

        /// <summary>Advance every task registered with Then. Call once per frame from the
        /// Unity thread; a task's status is only meaningful there.</summary>
        static void Pump();

        /// <summary>How many tasks are still waiting.</summary>
        [[nodiscard]] static size_t Pending();
    };

} // namespace IL2CPP::Module::Cysharp
