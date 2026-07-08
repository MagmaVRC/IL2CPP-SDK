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
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_vertices"), 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector3> a{ arr };
            return a.to_vector();
        }
        void SetVertices(const std::vector<Vector3>& verts) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("set_vertices"), 1);
            void* arr = CreateVector3Array(verts.data(), verts.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<Vector3> GetNormals() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_normals"), 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector3> a{ arr };
            return a.to_vector();
        }
        void SetNormals(const std::vector<Vector3>& normals) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("set_normals"), 1);
            void* arr = CreateVector3Array(normals.data(), normals.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<Vector4> GetTangents() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_tangents"), 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector4> a{ arr };
            return a.to_vector();
        }

        [[nodiscard]] std::vector<Vector2> GetUV() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_uv"), 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector2> a{ arr };
            return a.to_vector();
        }
        void SetUV(const std::vector<Vector2>& uv) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("set_uv"), 1);
            void* arr = CreateVector2Array(uv.data(), uv.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<Vector2> GetUV2() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_uv2"), 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Vector2> a{ arr };
            return a.to_vector();
        }
        void SetUV2(const std::vector<Vector2>& uv2) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("set_uv2"), 1);
            void* arr = CreateVector2Array(uv2.data(), uv2.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<Color32> GetColors32() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_colors32"), 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<Color32> a{ arr };
            return a.to_vector();
        }
        void SetColors32(const std::vector<Color32>& colors) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("set_colors32"), 1);
            void* arr = CreateColor32Array(colors.data(), colors.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] std::vector<int> GetTriangles() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_triangles"), 0);
            void* arr = MethodHandler::invoke<void*>(m, raw());
            if (!arr) return {};
            System::Array<int> a{ arr };
            return a.to_vector();
        }
        void SetTriangles(const std::vector<int>& triangles) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("set_triangles"), 1);
            void* arr = CreateIntArray(triangles.data(), triangles.size());
            if (!arr) return;
            void* params[] = { arr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetSubMeshCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_subMeshCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetSubMeshCount(int count) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("set_subMeshCount"), 1);
            void* params[] = { &count };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Bounds GetBounds() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_bounds"), 0);
            return MethodHandler::invoke<Bounds>(m, raw());
        }
        void SetBounds(const Bounds& bounds) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("set_bounds"), 1);
            Bounds b = bounds;
            void* params[] = { &b };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetVertexCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_vertexCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] bool GetIsReadable() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_isReadable"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        void Clear() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("Clear"), 0);
            MethodHandler::invoke(m, raw());
        }

        void RecalculateNormals() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("RecalculateNormals"), 0);
            MethodHandler::invoke(m, raw());
        }

        void RecalculateBounds() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("RecalculateBounds"), 0);
            MethodHandler::invoke(m, raw());
        }

        void RecalculateTangents() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("RecalculateTangents"), 0);
            MethodHandler::invoke(m, raw());
        }

        void MarkDynamic() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("MarkDynamic"), 0);
            MethodHandler::invoke(m, raw());
        }

        void UploadMeshData(bool markNoLongerReadable) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("UploadMeshData"), 1);
            void* params[] = { &markNoLongerReadable };
            MethodHandler::invoke(m, raw(), params);
        }

        // ---- GPU buffer access (D3D11: returns ID3D11Buffer*) ----

        [[nodiscard]] void* GetNativeVertexBufferPtr(int bufferIndex) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetNativeVertexBufferPtr"), 1);
            void* params[] = { &bufferIndex };
            return MethodHandler::invoke<void*>(m, raw(), params);
        }

        [[nodiscard]] void* GetNativeIndexBufferPtr() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetNativeIndexBufferPtr"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }

        [[nodiscard]] int GetVertexBufferStride(int stream) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetVertexBufferStride"), 1);
            void* params[] = { &stream };
            return MethodHandler::invoke<int>(m, raw(), params);
        }

        // 0 = UInt16, 1 = UInt32 (UnityEngine.Rendering.IndexFormat)
        [[nodiscard]] int GetIndexFormat() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("get_indexFormat"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        // ---- Submesh queries (attr/format args are the raw enum ints) ----

        [[nodiscard]] uint32_t GetIndexStart(int submesh) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetIndexStart"), 1);
            void* params[] = { &submesh };
            return MethodHandler::invoke<uint32_t>(m, raw(), params);
        }

        [[nodiscard]] uint32_t GetIndexCount(int submesh) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetIndexCount"), 1);
            void* params[] = { &submesh };
            return MethodHandler::invoke<uint32_t>(m, raw(), params);
        }

        [[nodiscard]] uint32_t GetBaseVertex(int submesh) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetBaseVertex"), 1);
            void* params[] = { &submesh };
            return MethodHandler::invoke<uint32_t>(m, raw(), params);
        }

        // UnityEngine.MeshTopology: 0 = Triangles
        [[nodiscard]] int GetTopology(int submesh) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetTopology"), 1);
            void* params[] = { &submesh };
            return MethodHandler::invoke<int>(m, raw(), params);
        }

        // ---- Vertex attribute layout (attr = UnityEngine.Rendering.VertexAttribute, 0 = Position) ----

        [[nodiscard]] bool HasVertexAttribute(int attr) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("HasVertexAttribute"), 1);
            void* params[] = { &attr };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }

        // UnityEngine.Rendering.VertexAttributeFormat: 0 = Float32
        [[nodiscard]] int GetVertexAttributeFormat(int attr) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetVertexAttributeFormat"), 1);
            void* params[] = { &attr };
            return MethodHandler::invoke<int>(m, raw(), params);
        }

        [[nodiscard]] int GetVertexAttributeDimension(int attr) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetVertexAttributeDimension"), 1);
            void* params[] = { &attr };
            return MethodHandler::invoke<int>(m, raw(), params);
        }

        [[nodiscard]] int GetVertexAttributeStream(int attr) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetVertexAttributeStream"), 1);
            void* params[] = { &attr };
            return MethodHandler::invoke<int>(m, raw(), params);
        }

        [[nodiscard]] int GetVertexAttributeOffset(int attr) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Mesh"), IL2CPP_STR("GetVertexAttributeOffset"), 1);
            void* params[] = { &attr };
            return MethodHandler::invoke<int>(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
