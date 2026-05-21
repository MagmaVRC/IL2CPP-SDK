#pragma once
#include "Object.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class Texture2D : public Object {
    public:
        using Object::Object;

        [[nodiscard]] int GetWidth() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("get_width"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        [[nodiscard]] int GetHeight() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("get_height"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        [[nodiscard]] int GetFormat() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("get_format"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        [[nodiscard]] bool GetIsReadable() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("get_isReadable"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] int GetFilterMode() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("get_filterMode"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetFilterMode(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("set_filterMode"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetWrapMode() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("get_wrapMode"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetWrapMode(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("set_wrapMode"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetMipmapCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("get_mipmapCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] int GetAnisoLevel() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("get_anisoLevel"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetAnisoLevel(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("set_anisoLevel"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void Apply(bool updateMipmaps = true, bool makeNoLongerReadable = false) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("Apply"), 2);
            void* params[] = { &updateMipmaps, &makeNoLongerReadable };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Color GetPixel(int x, int y) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("GetPixel"), 2);
            void* params[] = { &x, &y };
            return MethodHandler::invoke<Color>(m, raw(), params);
        }
        void SetPixel(int x, int y, const Color& color) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("SetPixel"), 3);
            Color c = color;
            void* params[] = { &x, &y, &c };
            MethodHandler::invoke(m, raw(), params);
        }

        void ReadPixels(const Rect& source, int destX, int destY) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("ReadPixels"), 3);
            Rect r = source;
            void* params[] = { &r, &destX, &destY };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool Resize(int width, int height) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("Resize"), 2);
            void* params[] = { &width, &height };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
