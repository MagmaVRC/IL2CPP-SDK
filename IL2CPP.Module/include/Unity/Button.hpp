#pragma once
#include "Selectable.hpp"
#include "UnityEvent.hpp"
#include "../System/Delegate.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class Button : public Selectable {
    public:
        using Selectable::Selectable;


        [[nodiscard]] void* GetOnClickRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Button"), IL2CPP_STR("get_onClick"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        /// <summary>The onClick UnityEvent (add/remove listeners, invoke).</summary>
        [[nodiscard]] Events::UnityEvent GetOnClick() const {
            return Events::UnityEvent{ GetOnClickRaw() };
        }

        /// <summary>Add an existing delegate as a click listener.</summary>
        void AddClickListener(const System::Delegate& call) {
            GetOnClick().AddListener(call);
        }

        /// Wire a native C++ function to onClick. Signature: void(__fastcall*)(void* target, void* method).
        /// Returns the UnityAction so it can be removed later.
        ///   void __fastcall OnClick(void*, void*) { ... }
        ///   button.AddClickListener(&OnClick);
        template<typename Fn>
        Events::UnityAction AddClickListener(Fn fn, void* context = nullptr) {
            return GetOnClick().AddNativeListener(fn, context);
        }
    };

} // namespace IL2CPP::Module::Unity
