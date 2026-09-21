#include <Cysharp/UniTask.hpp>
#include <il2cpp_module.hpp>

#include <mutex>
#include <vector>

namespace IL2CPP::Module::Cysharp {

    namespace {

        // UniTask<T> is { IUniTaskSource source; T result; short token; }, and plain
        // UniTask is the same without the result. Both are laid out with the source
        // first, which is the only field read without knowing T.
        constexpr size_t kOffSource = 0;
        constexpr size_t kOffResult = 8;
        constexpr size_t kOffToken = 16;

        struct PendingTask {
            UniTaskHandle handle;
            std::function<void(UniTaskStatus)> done;
        };

        std::vector<PendingTask> g_pending;
        std::mutex g_pendingMutex;

    } // namespace

    UniTaskStatus UniTaskHandle::Status() const {
        // A method that finished before it ever suspended returns a task with no source;
        // there is nothing left to wait for.
        if (!m_source) return UniTaskStatus::Succeeded;

        ManagedObject source(m_source);
        auto klass = source.get_class();
        if (!klass) return UniTaskStatus::Faulted;

        // Resolved against the source's own runtime class: GetStatus is an interface
        // method, and only the implementation carries code.
        const Method getStatus = klass.get_method(IL2CPP_STR("GetStatus"), 1);
        if (!getStatus) return UniTaskStatus::Pending;

        int16_t token = m_token;
        void* params[] = { &token };
        return static_cast<UniTaskStatus>(
            MethodHandler::invoke<int32_t>(getStatus, m_source, params));
    }

    void UniTaskHandle::Release() {
        if (m_root) {
            GCHandleFree(m_root);
            m_root = 0;
        }
        m_source = nullptr;
        m_result = nullptr;
    }

    UniTaskHandle UniTask::Start(const Method& method, void* obj,
                                 std::initializer_list<void*> args) {
        UniTaskHandle handle;

        void* code = method.pointer();
        if (!code) return handle;

        auto* e = GetExports();
        if (!e || !e->m_methodGetFlags) return handle;

        alignas(16) unsigned char task[128]{};
        void* mi = method.raw();
        void* const* a = args.begin();

        using F0 = void(IL2CPP_CALLTYPE)(void*, void*);
        using F1 = void(IL2CPP_CALLTYPE)(void*, void*, void*);
        using F2 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*);
        using F3 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*);
        using F4 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*, void*);
        using F5 = void(IL2CPP_CALLTYPE)(void*, void*, void*, void*, void*, void*, void*);

        if (method.is_static()) {
            switch (args.size()) {
                case 0: reinterpret_cast<F0>(code)(task, mi); break;
                case 1: reinterpret_cast<F1>(code)(task, a[0], mi); break;
                case 2: reinterpret_cast<F2>(code)(task, a[0], a[1], mi); break;
                case 3: reinterpret_cast<F3>(code)(task, a[0], a[1], a[2], mi); break;
                case 4: reinterpret_cast<F4>(code)(task, a[0], a[1], a[2], a[3], mi); break;
                default: return handle;
            }
        } else {
            switch (args.size()) {
                case 0: reinterpret_cast<F1>(code)(task, obj, mi); break;
                case 1: reinterpret_cast<F2>(code)(task, obj, a[0], mi); break;
                case 2: reinterpret_cast<F3>(code)(task, obj, a[0], a[1], mi); break;
                case 3: reinterpret_cast<F4>(code)(task, obj, a[0], a[1], a[2], mi); break;
                case 4: reinterpret_cast<F5>(code)(task, obj, a[0], a[1], a[2], a[3], mi); break;
                default: return handle;
            }
        }

        handle.m_source = *reinterpret_cast<void**>(task + kOffSource);
        handle.m_result = *reinterpret_cast<void**>(task + kOffResult);
        handle.m_token = *reinterpret_cast<int16_t*>(task + kOffToken);

        // The source is reachable only from the struct we are holding on the stack, so
        // it needs rooting before anything can allocate again.
        if (handle.m_source)
            handle.m_root = GCHandleNew(static_cast<il2cppObject*>(handle.m_source), false);
        return handle;
    }

    void UniTask::Then(const UniTaskHandle& handle, std::function<void(UniTaskStatus)> done) {
        if (!done) return;
        if (!handle.Valid()) {
            // Nothing to wait on: it already finished, or it never started.
            done(UniTaskStatus::Succeeded);
            return;
        }
        std::lock_guard lock(g_pendingMutex);
        g_pending.push_back({ handle, std::move(done) });
    }

    void UniTask::Pump() {
        std::vector<PendingTask> ready;
        {
            std::lock_guard lock(g_pendingMutex);
            if (g_pending.empty()) return;
            for (size_t i = g_pending.size(); i-- > 0;) {
                if (g_pending[i].handle.Status() == UniTaskStatus::Pending) continue;
                ready.push_back(std::move(g_pending[i]));
                g_pending.erase(g_pending.begin() + static_cast<ptrdiff_t>(i));
            }
        }
        // Fired outside the lock: a continuation is free to start another task.
        for (auto& task : ready) {
            const UniTaskStatus status = task.handle.Status();
            task.handle.Release();
            task.done(status);
        }
    }

    size_t UniTask::Pending() {
        std::lock_guard lock(g_pendingMutex);
        return g_pending.size();
    }

} // namespace IL2CPP::Module::Cysharp
