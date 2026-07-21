#pragma once
#include "Object.hpp"
#include "../Dispatcher.hpp"
#include "../System/Delegate.hpp"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace IL2CPP::Module::Unity {

    class AsyncOperation : public Object {
        template<typename Fn>
        struct CallbackState {
            Fn callback;
            std::shared_ptr<ManagedRoot> operationRoot;
        };

        struct Subscription {
            void* target = nullptr;
            std::shared_ptr<ManagedRoot> operationRoot;
            std::shared_ptr<ManagedRoot> actionRoot;
            void(*cleanup)(void*) = nullptr;
        };

        template<typename Fn>
        using CallbackMap = std::unordered_map<void*, std::unique_ptr<CallbackState<Fn>>>;

        template<typename Fn>
        static CallbackMap<Fn>& Callbacks() {
            static CallbackMap<Fn> callbacks;
            return callbacks;
        }

        template<typename Fn>
        static std::mutex& CallbackMutex() {
            static std::mutex mutex;
            return mutex;
        }

        static std::unordered_map<void*, Subscription>& Subscriptions() {
            static std::unordered_map<void*, Subscription> subscriptions;
            return subscriptions;
        }

        static std::mutex& SubscriptionMutex() {
            static std::mutex mutex;
            return mutex;
        }

        static std::optional<Subscription> TakeSubscription(void* target) {
            std::lock_guard lock(SubscriptionMutex());
            auto& subscriptions = Subscriptions();
            auto it = subscriptions.find(target);
            if (it == subscriptions.end()) return std::nullopt;
            Subscription subscription = std::move(it->second);
            subscriptions.erase(it);
            return subscription;
        }

        template<typename Fn>
        static std::unique_ptr<CallbackState<Fn>> TakeCallback(void* target) {
            std::lock_guard lock(CallbackMutex<Fn>());
            auto& callbacks = Callbacks<Fn>();
            auto it = callbacks.find(target);
            if (it == callbacks.end()) return {};
            auto callback = std::move(it->second);
            callbacks.erase(it);
            return callback;
        }

        template<typename Fn>
        static void CleanupCallback(void* target) {
            auto state = TakeCallback<Fn>(target);
            if (state) (void)state->operationRoot->Reset();
        }

        template<typename Fn>
        static void InvokeCallback(Fn& callback, AsyncOperation operation) {
            if constexpr (std::is_invocable_v<Fn&, AsyncOperation>) {
                std::invoke(callback, operation);
            } else if constexpr (std::is_invocable_v<Fn&, void*>) {
                std::invoke(callback, operation.raw());
            } else {
                std::invoke(callback);
            }
        }

        template<typename Fn>
        static void __fastcall CompletionThunk(void* target, void* operation, void*) noexcept {
            auto subscription = TakeSubscription(target);
            (void)subscription;
            auto state = TakeCallback<Fn>(target);
            if (!state) return;
            try {
                InvokeCallback(state->callback, AsyncOperation{ operation });
            } catch (...) {
            }
            (void)state->operationRoot->Reset();
        }

        static bool CancelAllOnMain() {
            std::vector<Subscription> subscriptions;
            {
                std::lock_guard lock(SubscriptionMutex());
                auto& active = Subscriptions();
                subscriptions.reserve(active.size());
                for (auto& [target, subscription] : active) {
                    (void)target;
                    subscriptions.push_back(std::move(subscription));
                }
                active.clear();
            }

            static auto remove = MethodHandler::resolve(
                IL2CPP_STR("UnityEngine.AsyncOperation"), IL2CPP_STR("remove_completed"), 1);
            bool success = true;
            for (auto& subscription : subscriptions) {
                void* operation = subscription.operationRoot->Get();
                void* action = subscription.actionRoot->Get();
                bool removed = false;
                if (remove && operation && action) {
                    void* parameters[] = { action };
                    void* exception = nullptr;
                    remove.invoke(operation, parameters, &exception);
                    removed = exception == nullptr;
                }
                if (removed) {
                    if (subscription.cleanup) subscription.cleanup(subscription.target);
                } else {
                    std::lock_guard lock(SubscriptionMutex());
                    Subscriptions().emplace(subscription.target, std::move(subscription));
                    success = false;
                }
            }
            return success;
        }

        template<typename Fn>
        [[nodiscard]] bool Subscribe(Fn&& fn, std::shared_ptr<ManagedRoot> operationRoot) const {
            static auto add = MethodHandler::resolve(
                IL2CPP_STR("UnityEngine.AsyncOperation"), IL2CPP_STR("add_completed"), 1);
            if (!add) return false;

            auto* exports = GetExports();
            Class targetClass = Class::find(IL2CPP_STR("System.Object"));
            Class delegateClass = add.get_param_type(0).get_class();
            if (!exports || !exports->m_objectNew || !targetClass || !delegateClass) return false;

            void* target = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(
                exports->m_objectNew)(targetClass.raw());
            if (!target) return false;

            using Callback = std::decay_t<Fn>;
            {
                std::lock_guard lock(CallbackMutex<Callback>());
                Callbacks<Callback>().emplace(target, std::make_unique<CallbackState<Callback>>(
                    CallbackState<Callback>{ std::forward<Fn>(fn), operationRoot }));
            }

            System::Delegate action = System::Delegate::CreateNative(
                delegateClass, reinterpret_cast<void*>(&CompletionThunk<Callback>), target);
            if (!action) {
                TakeCallback<Callback>(target);
                return false;
            }

            auto actionRoot = std::make_shared<ManagedRoot>();
            if (!actionRoot->Reset(action.raw())) {
                TakeCallback<Callback>(target);
                return false;
            }
            {
                std::lock_guard lock(SubscriptionMutex());
                Subscriptions().emplace(target, Subscription{
                    target, operationRoot, std::move(actionRoot), &CleanupCallback<Callback>
                });
            }

            void* parameters[] = { action.raw() };
            void* exception = nullptr;
            add.invoke(raw(), parameters, &exception);
            if (exception) {
                TakeSubscription(target);
                TakeCallback<Callback>(target);
                return false;
            }
            return true;
        }

    public:
        using Object::Object;

        [[nodiscard]] bool GetIsDone() const {
            static auto method = MethodHandler::resolve(
                IL2CPP_STR("UnityEngine.AsyncOperation"), IL2CPP_STR("get_isDone"), 0);
            return MethodHandler::invoke<bool>(method, raw());
        }

        [[nodiscard]] float GetProgress() const {
            static auto method = MethodHandler::resolve(
                IL2CPP_STR("UnityEngine.AsyncOperation"), IL2CPP_STR("get_progress"), 0);
            return MethodHandler::invoke<float>(method, raw());
        }

        template<typename Fn>
        [[nodiscard]] bool OnCompleted(Fn&& fn) const {
            if (!raw()) return false;

            auto operationRoot = std::make_shared<ManagedRoot>();
            if (!operationRoot->Reset(raw())) return false;

            using Callback = std::decay_t<Fn>;
            if (Dispatcher::IsMainThread()) {
                return Subscribe(Callback(std::forward<Fn>(fn)), std::move(operationRoot));
            }

            auto callback = std::make_shared<Callback>(std::forward<Fn>(fn));
            auto failed = [operationRoot, callback] {
                try {
                    InvokeCallback(*callback, AsyncOperation{});
                } catch (...) {
                }
                (void)operationRoot->Reset();
            };
            bool accepted = Dispatcher::RunOnMain(
                [operationRoot, callback, failed] {
                    void* operation = operationRoot->Get();
                    bool subscribed = operation && AsyncOperation{ operation }.Subscribe(
                        [callback](AsyncOperation completed) {
                            InvokeCallback(*callback, completed);
                        }, operationRoot);
                    if (!subscribed) failed();
                }, failed);
            if (!accepted) {
                try {
                    InvokeCallback(*callback, AsyncOperation{});
                } catch (...) {
                }
                (void)operationRoot->Reset();
            }
            return accepted;
        }

        [[nodiscard]] static bool CancelAllSubscriptions() {
            if (Dispatcher::IsMainThread()) {
                return CancelAllOnMain();
            }

            struct WaitState {
                std::mutex mutex;
                std::condition_variable condition;
                bool done = false;
                bool success = false;
            };
            auto state = std::make_shared<WaitState>();
            auto finish = [state](bool success) {
                {
                    std::lock_guard lock(state->mutex);
                    state->done = true;
                    state->success = success;
                }
                state->condition.notify_all();
            };
            bool accepted = Dispatcher::RunOnMain(
                [finish] {
                    finish(CancelAllOnMain());
                }, [finish] { finish(false); });
            if (!accepted) return false;
            std::unique_lock lock(state->mutex);
            state->condition.wait(lock, [&] { return state->done; });
            return state->success;
        }
    };

} // namespace IL2CPP::Module::Unity
