#pragma once
#include "Object.hpp"
#include "Texture2D.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Sprite : public Object {
    public:
        using Object::Object;

        [[nodiscard]] void* GetTexture() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Sprite"), IL2CPP_STR("get_texture"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        [[nodiscard]] Rect GetRect() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Sprite"), IL2CPP_STR("get_rect"), 0);
            return MethodHandler::invoke<Rect>(m, raw());
        }
        [[nodiscard]] Vector2 GetPivot() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Sprite"), IL2CPP_STR("get_pivot"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        [[nodiscard]] Vector4 GetBorder() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Sprite"), IL2CPP_STR("get_border"), 0);
            return MethodHandler::invoke<Vector4>(m, raw());
        }
        [[nodiscard]] float GetPixelsPerUnit() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Sprite"), IL2CPP_STR("get_pixelsPerUnit"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        [[nodiscard]] bool GetPacked() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Sprite"), IL2CPP_STR("get_packed"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] static Sprite Create(void* texture, const Rect& rect, const Vector2& pivot, float pixelsPerUnit = 100.0f) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Sprite"), IL2CPP_STR("Create"), 4);
            Rect r = rect; Vector2 p = pivot;
            void* params[] = { texture, &r, &p, &pixelsPerUnit };
            return Sprite{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        static Sprite LoadFromFile(std::string_view path, float pixelsPerUnit = 100.0f) {
            auto tex = Texture2D::LoadFromFile(path);
            if (!tex) return {};
            Rect rect{0.f, 0.f, (float)tex.GetWidth(), (float)tex.GetHeight()};
            Vector2 pivot{0.5f, 0.5f};
            return Create(tex.raw(), rect, pivot, pixelsPerUnit);
        }
    };

} // namespace IL2CPP::Module::Unity
