#pragma once
#include "ManagedObject.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace IL2CPP::Module {

    class Dispatcher {
        template<class F, class C>
        struct CancellablePayload {
            F callback;
            C cancelled;
        };

        template<class F>
        static void Trampoline(void* payload) {
            std::unique_ptr<F> fn(static_cast<F*>(payload));
            try {
                (*fn)();
            } catch (...) {
            }
        }

        template<class F>
        static void Destroy(void* payload) {
            delete static_cast<F*>(payload);
        }

        template<class F, class C>
        static void CancellableTrampoline(void* payload) {
            std::unique_ptr<CancellablePayload<F, C>> task(
                static_cast<CancellablePayload<F, C>*>(payload));
            try {
                std::invoke(task->callback);
            } catch (...) {
            }
        }

        template<class F, class C>
        static void Cancel(void* payload) {
            std::unique_ptr<CancellablePayload<F, C>> task(
                static_cast<CancellablePayload<F, C>*>(payload));
            try {
                std::invoke(task->cancelled);
            } catch (...) {
            }
        }

        using CoreFn = bool(IL2CPP_CALLTYPE)(void(*)(void*), void(*)(void*), void*);
        using CoreBool = bool(IL2CPP_CALLTYPE)();
        using CoreU32 = uint32_t(IL2CPP_CALLTYPE)();

    public:
        [[nodiscard]] static bool IsMainThread() noexcept {
            auto* exports = GetExports();
            return exports && exports->m_dispatchIsMainThread &&
                reinterpret_cast<CoreBool>(exports->m_dispatchIsMainThread)();
        }

        [[nodiscard]] static uint32_t WorkerCount() noexcept {
            auto* exports = GetExports();
            if (!exports || !exports->m_dispatchWorkerCount) return 0;
            return reinterpret_cast<CoreU32>(exports->m_dispatchWorkerCount)();
        }

        template<class F>
        [[nodiscard]] static bool RunOnMain(F&& fn) {
            auto* exports = GetExports();
            if (!exports || !exports->m_dispatchPostMain) return false;
            using Callback = std::decay_t<F>;
            auto* payload = new Callback(std::forward<F>(fn));
            bool accepted = reinterpret_cast<CoreFn>(exports->m_dispatchPostMain)(
                &Trampoline<Callback>, &Destroy<Callback>, payload);
            if (!accepted) delete payload;
            return accepted;
        }

        template<class F>
        [[nodiscard]] static bool RunOnWorker(F&& fn) {
            auto* exports = GetExports();
            if (!exports || !exports->m_dispatchPostWorker) return false;
            using Callback = std::decay_t<F>;
            auto* payload = new Callback(std::forward<F>(fn));
            bool accepted = reinterpret_cast<CoreFn>(exports->m_dispatchPostWorker)(
                &Trampoline<Callback>, &Destroy<Callback>, payload);
            if (!accepted) delete payload;
            return accepted;
        }

        template<class F, class C>
        [[nodiscard]] static bool RunOnMain(F&& fn, C&& cancelled) {
            auto* exports = GetExports();
            if (!exports || !exports->m_dispatchPostMain) return false;
            using Callback = std::decay_t<F>;
            using Cancelled = std::decay_t<C>;
            using Payload = CancellablePayload<Callback, Cancelled>;
            auto* payload = new Payload{
                std::forward<F>(fn), std::forward<C>(cancelled)
            };
            bool accepted = reinterpret_cast<CoreFn>(exports->m_dispatchPostMain)(
                &CancellableTrampoline<Callback, Cancelled>,
                &Cancel<Callback, Cancelled>, payload);
            if (!accepted) delete payload;
            return accepted;
        }

        template<class F, class C>
        [[nodiscard]] static bool RunOnWorker(F&& fn, C&& cancelled) {
            auto* exports = GetExports();
            if (!exports || !exports->m_dispatchPostWorker) return false;
            using Callback = std::decay_t<F>;
            using Cancelled = std::decay_t<C>;
            using Payload = CancellablePayload<Callback, Cancelled>;
            auto* payload = new Payload{
                std::forward<F>(fn), std::forward<C>(cancelled)
            };
            bool accepted = reinterpret_cast<CoreFn>(exports->m_dispatchPostWorker)(
                &CancellableTrampoline<Callback, Cancelled>,
                &Cancel<Callback, Cancelled>, payload);
            if (!accepted) delete payload;
            return accepted;
        }
    };

    template<class F>
    [[nodiscard]] bool RunOnMain(F&& fn) {
        return Dispatcher::RunOnMain(std::forward<F>(fn));
    }

    template<class F>
    [[nodiscard]] bool RunOnWorker(F&& fn) {
        return Dispatcher::RunOnWorker(std::forward<F>(fn));
    }

    class ManagedRoot {
        mutable std::mutex m_mutex;
        uint32_t m_handle = 0;

    public:
        ManagedRoot() = default;
        ManagedRoot(const ManagedRoot&) = delete;
        ManagedRoot& operator=(const ManagedRoot&) = delete;

        ~ManagedRoot() {
            (void)Reset();
        }

        [[nodiscard]] bool Reset(void* object = nullptr) {
            auto* exports = GetExports();
            uint32_t replacement = 0;
            if (object) {
                if (!exports || !exports->m_gcHandleNew) return false;
                replacement = reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*, uint8_t)>(
                    exports->m_gcHandleNew)(object, uint8_t{0});
                if (!replacement) return false;
            }

            uint32_t previous = 0;
            {
                std::lock_guard lock(m_mutex);
                previous = std::exchange(m_handle, replacement);
            }
            if (previous && exports && exports->m_gcHandleFree) {
                reinterpret_cast<void(IL2CPP_CALLTYPE)(uint32_t)>(
                    exports->m_gcHandleFree)(previous);
            }
            return true;
        }

        [[nodiscard]] void* Get() const {
            auto* exports = GetExports();
            if (!exports || !exports->m_gcHandleGetTarget) return nullptr;
            std::lock_guard lock(m_mutex);
            if (!m_handle) return nullptr;
            return reinterpret_cast<void*(IL2CPP_CALLTYPE)(uint32_t)>(
                exports->m_gcHandleGetTarget)(m_handle);
        }
    };

    namespace Detail {
        template<class T>
        struct TaskState {
            std::mutex mutex;
            std::condition_variable readyCondition;
            bool ready = false;
            std::optional<T> value;
            std::exception_ptr exception;
            std::vector<std::function<void()>> continuations;
            std::vector<std::shared_ptr<void>> keepAlive;
        };

        template<>
        struct TaskState<void> {
            std::mutex mutex;
            std::condition_variable readyCondition;
            bool ready = false;
            std::exception_ptr exception;
            std::vector<std::function<void()>> continuations;
            std::vector<std::shared_ptr<void>> keepAlive;
        };

        template<class State, class Complete>
        bool CompleteTask(const std::shared_ptr<State>& state, Complete&& complete) {
            std::vector<std::function<void()>> continuations;
            {
                std::lock_guard lock(state->mutex);
                if (state->ready) return false;
                complete(*state);
                state->ready = true;
                continuations.swap(state->continuations);
            }
            state->readyCondition.notify_all();
            for (auto& continuation : continuations) continuation();
            return true;
        }
    }

    template<class T>
    class Task;

    template<class T>
    class TaskSource {
        std::shared_ptr<Detail::TaskState<T>> m_state =
            std::make_shared<Detail::TaskState<T>>();

    public:
        [[nodiscard]] Task<T> GetTask() const;

        [[nodiscard]] bool SetValue(T value) const {
            return Detail::CompleteTask(m_state, [&](auto& state) {
                state.value.emplace(std::move(value));
            });
        }

        [[nodiscard]] bool SetException(std::exception_ptr exception) const {
            return Detail::CompleteTask(m_state, [&](auto& state) {
                state.exception = std::move(exception);
            });
        }

        void KeepAlive(std::shared_ptr<void> value) const {
            std::lock_guard lock(m_state->mutex);
            m_state->keepAlive.push_back(std::move(value));
        }
    };

    template<>
    class TaskSource<void> {
        std::shared_ptr<Detail::TaskState<void>> m_state =
            std::make_shared<Detail::TaskState<void>>();

    public:
        [[nodiscard]] Task<void> GetTask() const;

        [[nodiscard]] bool SetValue() const {
            return Detail::CompleteTask(m_state, [](auto&) {});
        }

        [[nodiscard]] bool SetException(std::exception_ptr exception) const {
            return Detail::CompleteTask(m_state, [&](auto& state) {
                state.exception = std::move(exception);
            });
        }

        void KeepAlive(std::shared_ptr<void> value) const {
            std::lock_guard lock(m_state->mutex);
            m_state->keepAlive.push_back(std::move(value));
        }
    };

    template<class T>
    class Task {
        std::shared_ptr<Detail::TaskState<T>> m_state;

        explicit Task(std::shared_ptr<Detail::TaskState<T>> state) : m_state(std::move(state)) {}
        friend class TaskSource<T>;

    public:
        Task() = default;

        [[nodiscard]] bool valid() const noexcept { return static_cast<bool>(m_state); }

        [[nodiscard]] bool is_done() const {
            if (!m_state) return false;
            std::lock_guard lock(m_state->mutex);
            return m_state->ready;
        }

        void wait() const {
            if (!m_state) return;
            std::unique_lock lock(m_state->mutex);
            m_state->readyCondition.wait(lock, [&] { return m_state->ready; });
        }

        T get() const {
            if (!m_state) throw std::future_error(std::future_errc::no_state);
            wait();
            std::lock_guard lock(m_state->mutex);
            if (m_state->exception) std::rethrow_exception(m_state->exception);
            return *m_state->value;
        }

        template<class F>
        [[nodiscard]] Task<void> then(F&& fn) const;

        template<class F>
        [[nodiscard]] static Task<T> OnWorker(F&& fn) {
            TaskSource<T> source;
            Task<T> task = source.GetTask();
            std::shared_ptr<ManagedRoot> resultRoot;
            if constexpr (std::is_base_of_v<ManagedObject, T>) {
                resultRoot = std::make_shared<ManagedRoot>();
                source.KeepAlive(resultRoot);
            }
            auto rejected = [source] {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("worker dispatch cancelled")));
            };
            bool accepted = Dispatcher::RunOnWorker(
                [source, resultRoot, fn = std::forward<F>(fn)]() mutable {
                    try {
                        T result = fn();
                        if constexpr (std::is_base_of_v<ManagedObject, T>) {
                            if (result && !resultRoot->Reset(result.raw())) {
                                throw std::runtime_error("failed to root worker task result");
                            }
                        }
                        (void)source.SetValue(std::move(result));
                    } catch (...) {
                        (void)source.SetException(std::current_exception());
                    }
                }, rejected);
            if (!accepted) {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("worker dispatch unavailable")));
            }
            return task;
        }

        template<class F>
        [[nodiscard]] static Task<T> OnMain(F&& fn) {
            TaskSource<T> source;
            Task<T> task = source.GetTask();
            std::shared_ptr<ManagedRoot> resultRoot;
            if constexpr (std::is_base_of_v<ManagedObject, T>) {
                resultRoot = std::make_shared<ManagedRoot>();
                source.KeepAlive(resultRoot);
            }
            auto rejected = [source] {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("main-thread dispatch cancelled")));
            };
            bool accepted = Dispatcher::RunOnMain(
                [source, resultRoot, fn = std::forward<F>(fn)]() mutable {
                    try {
                        T result = fn();
                        if constexpr (std::is_base_of_v<ManagedObject, T>) {
                            if (result && !resultRoot->Reset(result.raw())) {
                                throw std::runtime_error("failed to root main-thread task result");
                            }
                        }
                        (void)source.SetValue(std::move(result));
                    } catch (...) {
                        (void)source.SetException(std::current_exception());
                    }
                }, rejected);
            if (!accepted) {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("main-thread dispatch unavailable")));
            }
            return task;
        }
    };

    template<>
    class Task<void> {
        std::shared_ptr<Detail::TaskState<void>> m_state;

        explicit Task(std::shared_ptr<Detail::TaskState<void>> state) : m_state(std::move(state)) {}
        friend class TaskSource<void>;

    public:
        Task() = default;

        [[nodiscard]] bool valid() const noexcept { return static_cast<bool>(m_state); }

        [[nodiscard]] bool is_done() const {
            if (!m_state) return false;
            std::lock_guard lock(m_state->mutex);
            return m_state->ready;
        }

        void wait() const {
            if (!m_state) return;
            std::unique_lock lock(m_state->mutex);
            m_state->readyCondition.wait(lock, [&] { return m_state->ready; });
        }

        void get() const {
            if (!m_state) throw std::future_error(std::future_errc::no_state);
            wait();
            std::lock_guard lock(m_state->mutex);
            if (m_state->exception) std::rethrow_exception(m_state->exception);
        }

        template<class F>
        [[nodiscard]] Task<void> then(F&& fn) const {
            TaskSource<void> nextSource;
            Task<void> nextTask = nextSource.GetTask();
            if (!m_state) {
                (void)nextSource.SetException(std::make_exception_ptr(
                    std::future_error(std::future_errc::no_state)));
                return nextTask;
            }
            auto callback = std::make_shared<std::decay_t<F>>(std::forward<F>(fn));
            auto state = m_state;
            std::function<void()> continuation = [state, callback, nextSource] {
                {
                    std::lock_guard lock(state->mutex);
                    if (state->exception) {
                        (void)nextSource.SetException(state->exception);
                        return;
                    }
                }
                auto cancelled = [nextSource] {
                    (void)nextSource.SetException(std::make_exception_ptr(
                        std::runtime_error("continuation dispatch cancelled")));
                };
                bool accepted = Dispatcher::RunOnMain([state, callback, nextSource] {
                    (void)state;
                    try {
                        std::invoke(*callback);
                        (void)nextSource.SetValue();
                    } catch (...) {
                        (void)nextSource.SetException(std::current_exception());
                    }
                }, cancelled);
                if (!accepted) {
                    (void)nextSource.SetException(std::make_exception_ptr(
                        std::runtime_error("continuation dispatch unavailable")));
                }
            };

            bool ready = false;
            {
                std::lock_guard lock(m_state->mutex);
                ready = m_state->ready;
                if (!ready) m_state->continuations.push_back(continuation);
            }
            if (ready) continuation();
            return nextTask;
        }

        template<class F>
        [[nodiscard]] static Task<void> OnWorker(F&& fn) {
            TaskSource<void> source;
            Task<void> task = source.GetTask();
            auto rejected = [source] {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("worker dispatch cancelled")));
            };
            bool accepted = Dispatcher::RunOnWorker(
                [source, fn = std::forward<F>(fn)]() mutable {
                    try {
                        fn();
                        (void)source.SetValue();
                    } catch (...) {
                        (void)source.SetException(std::current_exception());
                    }
                }, rejected);
            if (!accepted) {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("worker dispatch unavailable")));
            }
            return task;
        }

        template<class F>
        [[nodiscard]] static Task<void> OnMain(F&& fn) {
            TaskSource<void> source;
            Task<void> task = source.GetTask();
            auto rejected = [source] {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("main-thread dispatch cancelled")));
            };
            bool accepted = Dispatcher::RunOnMain(
                [source, fn = std::forward<F>(fn)]() mutable {
                    try {
                        fn();
                        (void)source.SetValue();
                    } catch (...) {
                        (void)source.SetException(std::current_exception());
                    }
                }, rejected);
            if (!accepted) {
                (void)source.SetException(std::make_exception_ptr(
                    std::runtime_error("main-thread dispatch unavailable")));
            }
            return task;
        }
    };

    template<class T>
    template<class F>
    Task<void> Task<T>::then(F&& fn) const {
        TaskSource<void> nextSource;
        Task<void> nextTask = nextSource.GetTask();
        if (!m_state) {
            (void)nextSource.SetException(std::make_exception_ptr(
                std::future_error(std::future_errc::no_state)));
            return nextTask;
        }

        auto callback = std::make_shared<std::decay_t<F>>(std::forward<F>(fn));
        auto state = m_state;
        std::function<void()> continuation = [state, callback, nextSource] {
            std::optional<T> value;
            {
                std::lock_guard lock(state->mutex);
                if (state->exception) {
                    (void)nextSource.SetException(state->exception);
                    return;
                }
                if (!state->value) {
                    (void)nextSource.SetException(std::make_exception_ptr(
                        std::runtime_error("task completed without a value")));
                    return;
                }
                value.emplace(*state->value);
            }

            auto cancelled = [nextSource] {
                (void)nextSource.SetException(std::make_exception_ptr(
                    std::runtime_error("continuation dispatch cancelled")));
            };
            bool accepted = Dispatcher::RunOnMain(
                [state, callback, nextSource, value = std::move(*value)]() mutable {
                    (void)state;
                    try {
                        std::invoke(*callback, std::move(value));
                        (void)nextSource.SetValue();
                    } catch (...) {
                        (void)nextSource.SetException(std::current_exception());
                    }
                }, cancelled);
            if (!accepted) {
                (void)nextSource.SetException(std::make_exception_ptr(
                    std::runtime_error("continuation dispatch unavailable")));
            }
        };

        bool ready = false;
        {
            std::lock_guard lock(m_state->mutex);
            ready = m_state->ready;
            if (!ready) m_state->continuations.push_back(continuation);
        }
        if (ready) continuation();
        return nextTask;
    }

    template<class T>
    Task<T> TaskSource<T>::GetTask() const {
        return Task<T>{ m_state };
    }

    inline Task<void> TaskSource<void>::GetTask() const {
        return Task<void>{ m_state };
    }

} // namespace IL2CPP::Module
