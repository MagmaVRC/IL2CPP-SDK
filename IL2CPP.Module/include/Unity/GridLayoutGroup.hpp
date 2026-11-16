#pragma once
#include "LayoutGroup.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class GridLayoutGroup : public LayoutGroup {
    public:
        using LayoutGroup::LayoutGroup;

        void SetCellSize(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.GridLayoutGroup"), IL2CPP_STR("set_cellSize"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }
        void SetSpacing(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.GridLayoutGroup"), IL2CPP_STR("set_spacing"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }
        /// <summary>0 Flexible, 1 FixedColumnCount, 2 FixedRowCount.</summary>
        void SetConstraint(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.GridLayoutGroup"), IL2CPP_STR("set_constraint"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
        void SetConstraintCount(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.UI.GridLayoutGroup"), IL2CPP_STR("set_constraintCount"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
