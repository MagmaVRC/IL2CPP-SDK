#pragma once
#include "Object.hpp"
#include "../MethodHandler.hpp"
#include "../System/Array.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <cstring>
#include <vector>

namespace IL2CPP::Module::Unity {

    class Mesh : public Object {
        static void* CreateVector3Array(const Vector3* data, size_t count) {
            auto* e = GetExports();
            if (!e || !data || count == 0) return nullptr;
            void* v3Class = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)("UnityEngine.Vector3");
            if (!v3Class) return nullptr;
            void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(e->m_arrayNew)(v3Class, count);
            if (!arr) return nullptr;
            std::memcpy(static_cast<char*>(arr) + 0x20, data, count * sizeof(Vector3));
            return arr;
        }

        static void* CreateVector2Array(const Vector2* data, size_t count) {
            auto* e = GetExports();
            if (!e || !data || count == 0) return nullptr;
            void* v2Class = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)("UnityEngine.Vector2");
            if (!v2Class) return nullptr;
            void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(e->m_arrayNew)(v2Class, count);
            if (!arr) return nullptr;
            std::memcpy(static_cast<char*>(arr) + 0x20, data, count * sizeof(Vector2));
            return arr;
        }

        static void* CreateColor32Array(const Color32* data, size_t count) {
            auto* e = GetExports();
            if (!e || !data || count == 0) return nullptr;
            void* c32Class = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)("UnityEngine.Color32");
            if (!c32Class) return nullptr;
            void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(e->m_arrayNew)(c32Class, count);
            if (!arr) return nullptr;
            std::memcpy(static_cast<char*>(arr) + 0x20, data, count * sizeof(Color32));
            return arr;
        }

        static void* CreateIntArray(const int* data, size_t count) {
            auto* e = GetExports();
            if (!e || !data || count == 0) return nullptr;
            void* intClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)("System.Int32");
            if (!intClass) return nullptr;
            void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(e->m_arrayNew)(intClass, count);
            if (!arr) return nullptr;
            std::memcpy(static_cast<char*>(arr) + 0x20, data, count * sizeof(int));
            return arr;
        }

    public:
        using Object::Object;

        [[nodiscard]] std::vector<Vector3> GetVertices() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_vertices", 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector3> a{ arr };
            return a.to_vector();
        }
        void SetVertices(const std::vector<Vector3>& verts) {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "set_vertices", 1);
            void* arr = CreateVector3Array(verts.data(), verts.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<Vector3> GetNormals() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_normals", 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector3> a{ arr };
            return a.to_vector();
        }
        void SetNormals(const std::vector<Vector3>& normals) {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "set_normals", 1);
            void* arr = CreateVector3Array(normals.data(), normals.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<Vector4> GetTangents() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_tangents", 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector4> a{ arr };
            return a.to_vector();
        }

        [[nodiscard]] std::vector<Vector2> GetUV() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_uv", 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector2> a{ arr };
            return a.to_vector();
        }
        void SetUV(const std::vector<Vector2>& uv) {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "set_uv", 1);
            void* arr = CreateVector2Array(uv.data(), uv.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<Vector2> GetUV2() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_uv2", 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector2> a{ arr };
            return a.to_vector();
        }
        void SetUV2(const std::vector<Vector2>& uv2) {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "set_uv2", 1);
            void* arr = CreateVector2Array(uv2.data(), uv2.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<Color32> GetColors32() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_colors32", 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Color32> a{ arr };
            return a.to_vector();
        }
        void SetColors32(const std::vector<Color32>& colors) {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "set_colors32", 1);
            void* arr = CreateColor32Array(colors.data(), colors.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<int> GetTriangles() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_triangles", 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<int> a{ arr };
            return a.to_vector();
        }
        void SetTriangles(const std::vector<int>& triangles) {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "set_triangles", 1);
            void* arr = CreateIntArray(triangles.data(), triangles.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetSubMeshCount() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_subMeshCount", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetSubMeshCount(int count) {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "set_subMeshCount", 1);
            void* params[] = { &count };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Bounds GetBounds() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_bounds", 0);
            return MethodHandler::invoke<Bounds>(m, raw());
        }
        void SetBounds(const Bounds& bounds) {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "set_bounds", 1);
            Bounds b = bounds;
            void* params[] = { &b };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetVertexCount() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_vertexCount", 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] bool GetIsReadable() const {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "get_isReadable", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        void Clear() {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "Clear", 0);
            MethodHandler::invoke(m, raw());
        }

        void RecalculateNormals() {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "RecalculateNormals", 0);
            MethodHandler::invoke(m, raw());
        }

        void RecalculateBounds() {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "RecalculateBounds", 0);
            MethodHandler::invoke(m, raw());
        }

        void RecalculateTangents() {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "RecalculateTangents", 0);
            MethodHandler::invoke(m, raw());
        }

        void MarkDynamic() {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "MarkDynamic", 0);
            MethodHandler::invoke(m, raw());
        }

        void UploadMeshData(bool markNoLongerReadable) {
            static auto m = MethodHandler::resolve("UnityEngine.Mesh", "UploadMeshData", 1);
            void* params[] = { &markNoLongerReadable };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
