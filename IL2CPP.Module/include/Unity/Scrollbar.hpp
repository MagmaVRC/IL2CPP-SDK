#pragma once
#include "Selectable.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class Scrollbar : public Selectable {
    public:
        using Selectable::Selectable;

        /// <summary>0 LeftToRight, 1 RightToLeft, 2 BottomToTop, 3 TopToBottom.</summary>
        void SetDirection(int direction) {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("set_direction"), 1);
            void* params[] = { &direction };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] int GetDirection() const {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("get_direction"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        /// <summary>Thumb position in [0,1], measured along the direction; fires onValueChanged.
        /// On a bar wired to a ScrollRect through <c>ScrollRect::SetVerticalScrollbar</c> this re-enters the
        /// ScrollRect's own offset (value change drives SetVerticalNormalizedPosition, which rewrites the
        /// content's anchoredPosition); write through <c>SetValueWithoutNotify</c> instead, or leave value to
        /// the ScrollRect entirely. See <c>ScrollRect::SetVerticalScrollbar</c>.</summary>
        void SetValue(float value) {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("set_value"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] float GetValue() const {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("get_value"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        /// <summary>Moves the thumb without raising onValueChanged. This is the only safe way to write the
        /// value of a bar wired to a ScrollRect: <c>SetValue</c> would re-enter the ScrollRect's offset. A wired
        /// bar has its value rewritten by the ScrollRect each layout pass anyway.</summary>
        void SetValueWithoutNotify(float value) {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("SetValueWithoutNotify"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        /// <summary>Thumb length as a fraction of the track in [0,1]. On a bar wired to a ScrollRect the
        /// ScrollRect recomputes size from its own viewport/content bounds each layout pass and overwrites this;
        /// only an unwired bar keeps a caller-written size.</summary>
        void SetSize(float size) {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("set_size"), 1);
            void* params[] = { &size };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] float GetSize() const {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("get_size"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }

        /// <summary>0 for free movement; otherwise the thumb snaps to that many discrete steps.</summary>
        void SetNumberOfSteps(int steps) {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("set_numberOfSteps"), 1);
            void* params[] = { &steps };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] int GetNumberOfSteps() const {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("get_numberOfSteps"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        /// <summary>The RectTransform the thumb is driven through; must be a child of the track rect.</summary>
        void SetHandleRectRaw(void* rect) {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("set_handleRect"), 1);
            void* params[] = { rect };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] void* GetHandleRectRaw() const {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("get_handleRect"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        /// <summary>UnityEvent&lt;float&gt; raised on value changes; pass to UnityEvent helpers to subscribe.</summary>
        [[nodiscard]] void* GetOnValueChangedRaw() const {
            static Method m; if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.Scrollbar"), IL2CPP_STR("get_onValueChanged"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
