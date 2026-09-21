#pragma once
#include "Object.hpp"
#include "../MethodHandler.hpp"
#include "../Reflection.hpp"
#include "../System/Array.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <vector>
#include <string_view>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::Unity {

    class Texture2D : public Object {
    public:
        using Object::Object;

        static Texture2D Create(int width = 2, int height = 2) {
            auto klass = Class::find(IL2CPP_STR("UnityEngine.Texture2D"));
            if (!klass) return {};
            auto obj = klass.new_object();
            if (!obj) return {};
            static auto ctor = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR(".ctor"), 2);
            if (!ctor) return {};
            void* params[] = { &width, &height };
            MethodHandler::invoke(ctor, obj.raw(), params);
            return Texture2D{obj.raw()};
        }

        bool LoadImage(const System::Array<uint8_t>& bytes, bool markNonReadable = false) {
            if (!bytes) return false;
            // Selected by parameter type, not by argument count. Unity 6 added a
            // LoadImage(Texture2D, ReadOnlySpan<byte>, bool) overload that shares its arity
            // with 2022.3's LoadImage(Texture2D, byte[], bool), so resolving on the count
            // alone hands a managed array to a method expecting a byref span and the load
            // quietly fails on one of the two builds.
            struct Found { Method m{}; int argc = 0; };
            // Not cached until it succeeds: a miss here would otherwise be remembered from
            // whichever call happened before the metadata was ready.
            static Found found{};
            if (!found.m) {
                auto k = Class::find(IL2CPP_STR("UnityEngine.ImageConversion"));
                if (!k) return false;
                for (auto& m : k.get_methods()) {
                    if (!m.name() || std::string_view(m.name()) != IL2CPP_STR("LoadImage")) continue;
                    const int argc = static_cast<int>(m.param_count());
                    if (argc != 2 && argc != 3) continue;
                    auto t = m.get_param_type(1);
                    auto c = t ? t.get_class() : Class{};
                    if (!c || c.full_name() != IL2CPP_STR("System.Byte[]")) continue;
                    // The three-argument form also carries markNonReadable, so prefer it.
                    if (!found.m || argc > found.argc) found = Found{ m, argc };
                }
                if (!found.m) return false;
            }

            if (found.argc == 3) {
                void* params[] = { raw(), bytes.raw(), &markNonReadable };
                return MethodHandler::invoke<bool>(found.m, nullptr, params);
            }
            void* params[] = { raw(), bytes.raw() };
            return MethodHandler::invoke<bool>(found.m, nullptr, params);
        }

        bool LoadImage(const std::vector<uint8_t>& data) {
            return LoadImage(System::Array<uint8_t>::FromBytes(data));
        }

        [[nodiscard]] std::vector<uint8_t> EncodeToPNG() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ImageConversion"), IL2CPP_STR("EncodeToPNG"), 1);
            if (!m) return {};
            void* params[] = { raw() };
            auto* arr = MethodHandler::invoke<void*>(m, nullptr, params);
            return arr ? System::Array<uint8_t>{arr}.to_vector() : std::vector<uint8_t>{};
        }

        [[nodiscard]] std::vector<uint8_t> EncodeToJPG(int quality = 75) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ImageConversion"), IL2CPP_STR("EncodeToJPG"), 2);
            if (!m) return {};
            void* params[] = { raw(), &quality };
            auto* arr = MethodHandler::invoke<void*>(m, nullptr, params);
            return arr ? System::Array<uint8_t>{arr}.to_vector() : std::vector<uint8_t>{};
        }

        static Texture2D LoadFromFile(std::string_view path) {
            static auto readAll = MethodHandler::resolve(IL2CPP_STR("System.IO.File"), IL2CPP_STR("ReadAllBytes"), 1);
            auto* e = GetExports();
            if (readAll && e && e->m_stringNew) {
                void* pathStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
                if (pathStr) {
                    void* params[] = { pathStr };
                    void* byteArr = MethodHandler::invoke<void*>(readAll, nullptr, params);
                    if (byteArr) {
                        auto tex = Create();
                        if (tex && tex.LoadImage(System::Array<uint8_t>{byteArr})) { tex.DontUnload(); return tex; }
                    }
                }
            }
            HANDLE hFile = CreateFileA(std::string(path).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
            if (hFile == INVALID_HANDLE_VALUE) return {};
            DWORD sz = GetFileSize(hFile, nullptr);
            if (sz == 0 || sz == INVALID_FILE_SIZE) { CloseHandle(hFile); return {}; }
            std::vector<uint8_t> data(sz);
            DWORD rd = 0;
            ReadFile(hFile, data.data(), sz, &rd, nullptr);
            CloseHandle(hFile);
            if (rd == 0) return {};
            auto tex = Create();
            if (tex && tex.LoadImage(data)) { tex.DontUnload(); return tex; }
            return {};
        }

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

        /// <summary>Resize the texture, discarding its contents.</summary>
        /// <param name="width">New width in pixels.</param>
        /// <param name="height">New height in pixels.</param>
        /// <returns>True when the texture was reallocated.</returns>
        [[nodiscard]] bool Resize(int width, int height) {
            // Unity 6 dropped the Resize spelling; Reinitialize is the same method and
            // exists on 2022.3 as well.
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Texture2D"), IL2CPP_STR("Reinitialize"), 2);
            void* params[] = { &width, &height };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
