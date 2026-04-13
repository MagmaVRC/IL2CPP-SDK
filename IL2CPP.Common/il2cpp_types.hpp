#pragma once
#include <cstdint>
#include <cmath>
#include <string>
#include <numbers>
#include <sstream>
#include <iomanip>

namespace IL2CPP {

    constexpr float DEG2RAD = std::numbers::pi_v<float> / 180.f;
    constexpr float RAD2DEG = 180.f / std::numbers::pi_v<float>;

    static inline std::string fmtf(float f) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << f;
        return oss.str();
    }

    struct Vector2 {
        float x, y;
        constexpr Vector2() : x(0.f), y(0.f) {}
        constexpr Vector2(float f1, float f2) : x(f1), y(f2) {}
        [[nodiscard]] std::string ToString() const { return "vec2(" + fmtf(x) + ", " + fmtf(y) + ")"; }
    };

    struct Vector2Int {
        int x, y;
        constexpr Vector2Int() : x(0), y(0) {}
        constexpr Vector2Int(int f1, int f2) : x(f1), y(f2) {}
        [[nodiscard]] std::string ToString() const { return "vec2i(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
    };

    struct Vector3 {
        float x, y, z;
        constexpr Vector3() : x(0.f), y(0.f), z(0.f) {}
        constexpr Vector3(float fx, float fy) : x(fx), y(fy), z(0.f) {}
        constexpr Vector3(float fx, float fy, float fz) : x(fx), y(fy), z(fz) {}

        [[nodiscard]] __forceinline float Magnitude() const { return sqrtf(x * x + y * y + z * z); }
        [[nodiscard]] __forceinline float SqrMagnitude() const { return x * x + y * y + z * z; }

        [[nodiscard]] __forceinline Vector3 Normalize() const {
            float len = Magnitude();
            if (len > 0.f) [[likely]] return *this * (1.f / len);
            return *this;
        }
        void NormalizeInPlace() { float len = Magnitude(); if (len > 0.f) [[likely]] { x /= len; y /= len; z /= len; } }

        [[nodiscard]] static __forceinline float Distance(const Vector3& a, const Vector3& b) { return (a - b).Magnitude(); }
        [[nodiscard]] __forceinline float Dot(const Vector3& b) const { return x * b.x + y * b.y + z * b.z; }
        [[nodiscard]] __forceinline Vector3 Cross(const Vector3& b) const {
            return Vector3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
        }
        [[nodiscard]] static __forceinline Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
            t = (t < 0.f) ? 0.f : ((t > 1.f) ? 1.f : t);
            return a + (b - a) * t;
        }

        [[nodiscard]] __forceinline Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
        [[nodiscard]] __forceinline Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
        [[nodiscard]] __forceinline Vector3 operator*(float f) const { return Vector3(x * f, y * f, z * f); }
        [[nodiscard]] __forceinline Vector3 operator/(float f) const { return Vector3(x / f, y / f, z / f); }
        [[nodiscard]] __forceinline Vector3 operator-() const { return Vector3(-x, -y, -z); }
        Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
        Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
        Vector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
        Vector3& operator/=(float f) { x /= f; y /= f; z /= f; return *this; }

        void ToVectors(Vector3* forward, Vector3* right, Vector3* up) const {
            float radX = x * DEG2RAD, radY = y * DEG2RAD, radZ = z * DEG2RAD;
            float sinX = sinf(radX), cosX = cosf(radX);
            float sinY = sinf(radY), cosY = cosf(radY);
            float sinZ = sinf(radZ), cosZ = cosf(radZ);
            if (forward) { forward->x = cosX * cosY; forward->y = -sinX; forward->z = cosX * sinY; }
            if (right) { right->x = -sinZ * sinX * cosY - cosZ * -sinY; right->y = -sinZ * cosX; right->z = -sinZ * sinX * sinY - cosZ * cosY; }
            if (up) { up->x = cosZ * sinX * cosY + -sinZ * -sinY; up->y = cosZ * cosX; up->z = cosZ * sinX * sinY + -sinZ * cosY; }
        }
        [[nodiscard]] std::string ToString() const { return "vec3(" + fmtf(x) + ", " + fmtf(y) + ", " + fmtf(z) + ")"; }
    };

    struct Vector3Int {
        int x, y, z;
        constexpr Vector3Int() : x(0), y(0), z(0) {}
        constexpr Vector3Int(int fx, int fy, int fz) : x(fx), y(fy), z(fz) {}
        [[nodiscard]] std::string ToString() const { return "vec3i(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; }
    };

    struct Vector4 {
        float x, y, z, w;
        constexpr Vector4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
        constexpr Vector4(float f1, float f2, float f3, float f4) : x(f1), y(f2), z(f3), w(f4) {}
        [[nodiscard]] std::string ToString() const { return "vec4(" + fmtf(x) + ", " + fmtf(y) + ", " + fmtf(z) + ", " + fmtf(w) + ")"; }
    };

    struct Vector4Int {
        int x, y, z, w;
        constexpr Vector4Int() : x(0), y(0), z(0), w(0) {}
        constexpr Vector4Int(int f1, int f2, int f3, int f4) : x(f1), y(f2), z(f3), w(f4) {}
        [[nodiscard]] std::string ToString() const { return "vec4i(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")"; }
    };

    struct Quaternion {
        float x, y, z, w;
        constexpr Quaternion() : x(0.f), y(0.f), z(0.f), w(0.f) {}
        constexpr Quaternion(float f1, float f2, float f3, float f4) : x(f1), y(f2), z(f3), w(f4) {}

        [[nodiscard]] static Quaternion Euler(float m_fX, float m_fY, float m_fZ) {
            m_fX = m_fX * DEG2RAD * 0.5f; m_fY = m_fY * DEG2RAD * 0.5f; m_fZ = m_fZ * DEG2RAD * 0.5f;
            float sX = sinf(m_fX), cX = cosf(m_fX), sY = sinf(m_fY), cY = cosf(m_fY), sZ = sinf(m_fZ), cZ = cosf(m_fZ);
            return Quaternion(
                cY * sX * cZ + sY * cX * sZ,
                sY * cX * cZ - cY * sX * sZ,
                cY * cX * sZ - sY * sX * cZ,
                cY * cX * cZ + sY * sX * sZ);
        }
        [[nodiscard]] static Quaternion Euler(const Vector3& r) { return Euler(r.x, r.y, r.z); }

        [[nodiscard]] Vector3 ToEuler() const {
            Vector3 e; float d = x * x + y * y + z * z + w * w; float t = x * w - y * z;
            if (t > 0.4995f * d) { e.x = std::numbers::pi_v<float> * 0.5f; e.y = 2.f * atan2f(y, x); e.z = 0.f; }
            else if (t < -0.4995f * d) { e.x = std::numbers::pi_v<float> * -0.5f; e.y = -2.f * atan2f(y, x); e.z = 0.f; }
            else { e.x = asinf(2.f * (w * x - y * z)); e.y = atan2f(2.f * w * y + 2.f * z * x, 1.f - 2.f * (x * x + y * y)); e.z = atan2f(2.f * w * z + 2.f * x * y, 1.f - 2.f * (z * z + x * x)); }
            e.x *= RAD2DEG; e.y *= RAD2DEG; e.z *= RAD2DEG; return e;
        }

        [[nodiscard]] __forceinline Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }
        [[nodiscard]] __forceinline Quaternion Inverse() const { float l = x * x + y * y + z * z + w * w; return Quaternion(-x / l, -y / l, -z / l, w / l); }
        [[nodiscard]] __forceinline float Dot(const Quaternion& q) const { return x * q.x + y * q.y + z * q.z + w * q.w; }
        [[nodiscard]] __forceinline float Length() const { return sqrtf(x * x + y * y + z * z + w * w); }
        [[nodiscard]] __forceinline Quaternion Normalized() const { float l = Length(); if (l < 1e-6f) return Quaternion(0, 0, 0, 1); return Quaternion(x / l, y / l, z / l, w / l); }
        Quaternion& Normalize() { float l = Length(); if (l < 1e-6f) { x = y = z = 0; w = 1; } else { x /= l; y /= l; z /= l; w /= l; } return *this; }

        [[nodiscard]] __forceinline Vector3 Forward() const { return *this * Vector3(0, 0, 1); }
        [[nodiscard]] __forceinline Vector3 Back() const { return *this * Vector3(0, 0, -1); }
        [[nodiscard]] __forceinline Vector3 Right() const { return *this * Vector3(1, 0, 0); }
        [[nodiscard]] __forceinline Vector3 Left() const { return *this * Vector3(-1, 0, 0); }
        [[nodiscard]] __forceinline Vector3 Up() const { return *this * Vector3(0, 1, 0); }
        [[nodiscard]] __forceinline Vector3 Down() const { return *this * Vector3(0, -1, 0); }

        [[nodiscard]] __forceinline Quaternion operator+(const Quaternion& q) const { return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w); }
        [[nodiscard]] __forceinline Vector3 operator*(const Vector3& v) const { Vector3 qv(x, y, z); Vector3 uv = qv.Cross(v); Vector3 uuv = qv.Cross(uv); uv *= 2.f * w; uuv *= 2.f; return v + uv + uuv; }
        [[nodiscard]] __forceinline Quaternion operator*(const Quaternion& q) const { return Quaternion(w * q.x + x * q.w + y * q.z - z * q.y, w * q.y - x * q.z + y * q.w + z * q.x, w * q.z + x * q.y - y * q.x + z * q.w, w * q.w - x * q.x - y * q.y - z * q.z); }
        Quaternion& operator+=(const Quaternion& q) { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
        Quaternion& operator*=(const Quaternion& q) { float nx = w * q.x + x * q.w + y * q.z - z * q.y, ny = w * q.y - x * q.z + y * q.w + z * q.x, nz = w * q.z + x * q.y - y * q.x + z * q.w, nw = w * q.w - x * q.x - y * q.y - z * q.z; x = nx; y = ny; z = nz; w = nw; return *this; }

        [[nodiscard]] std::string ToString() const { return "quaternion(" + fmtf(x) + ", " + fmtf(y) + ", " + fmtf(z) + ", " + fmtf(w) + ")"; }
    };

    struct Matrix3x3 { float m[3][3] = {}; constexpr Matrix3x3() = default; float* operator[](int i) { return m[i]; } const float* operator[](int i) const { return m[i]; } };
    struct Matrix4x4 { float m[4][4] = {}; constexpr Matrix4x4() = default; float* operator[](int i) { return m[i]; } const float* operator[](int i) const { return m[i]; } };

    struct Bounds { Vector3 m_vCenter, m_vExtents; constexpr Bounds() = default; constexpr Bounds(const Vector3& c, const Vector3& e) : m_vCenter(c), m_vExtents(e) {} [[nodiscard]] std::string ToString() const { return "Bounds(center=" + m_vCenter.ToString() + ", extents=" + m_vExtents.ToString() + ")"; } };
    struct BoundsInt { Vector3Int m_vPosition, m_vSize; constexpr BoundsInt() = default; constexpr BoundsInt(const Vector3Int& p, const Vector3Int& s) : m_vPosition(p), m_vSize(s) {} [[nodiscard]] std::string ToString() const { return "BoundsInt(position=" + m_vPosition.ToString() + ", size=" + m_vSize.ToString() + ")"; } };
    struct Plane { Vector3 m_vNormal; float fDistance = 0.f; constexpr Plane() = default; constexpr Plane(const Vector3& n, float d) : m_vNormal(n), fDistance(d) {} [[nodiscard]] std::string ToString() const { return "Plane(normal=" + m_vNormal.ToString() + ", distance=" + fmtf(fDistance) + ")"; } };
    struct Ray { Vector3 origin, direction; constexpr Ray() : direction(0, 0, 1) {} constexpr Ray(const Vector3& o, const Vector3& d) : origin(o), direction(d) {} [[nodiscard]] std::string ToString() const { return "Ray(origin=" + origin.ToString() + ", direction=" + direction.ToString() + ")"; } };
    struct Rect { float fX = 0, fY = 0, fWidth = 0, fHeight = 0; constexpr Rect() = default; constexpr Rect(float a, float b, float c, float d) : fX(a), fY(b), fWidth(c), fHeight(d) {} };
    struct RectInt { int x = 0, y = 0, width = 0, height = 0; constexpr RectInt() = default; constexpr RectInt(int a, int b, int c, int d) : x(a), y(b), width(c), height(d) {} };

    struct RaycastHit { Vector3 point; Vector3 normal; uint32_t FaceID; float distance; Vector2 uv; int collider; };

    struct Color { float r = 0, g = 0, b = 0, a = 0; constexpr Color() = default; constexpr Color(float fr, float fg, float fb, float fa = 1.f) : r(fr), g(fg), b(fb), a(fa) {} [[nodiscard]] std::string ToString() const { return "Color(" + fmtf(r) + ", " + fmtf(g) + ", " + fmtf(b) + ", " + fmtf(a) + ")"; } };
    struct Color32 { uint8_t r = 0, g = 0, b = 0, a = 0; constexpr Color32() = default; constexpr Color32(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca) : r(cr), g(cg), b(cb), a(ca) {} };

    struct LayerMask { int m_Mask = 0; constexpr LayerMask() = default; constexpr LayerMask(int mask) : m_Mask(mask) {} [[nodiscard]] constexpr int value() const { return m_Mask; } };
    struct Scene { int handle = 0; };

    enum class LoadSceneMode : int { Single, Additive };
    enum class TouchPhase : int { Began = 0, Moved, Stationary, Ended, Canceled };
    enum class TouchType : int { Direct = 0, Indirect, Stylus };
    enum class RigidbodyConstraints : int { None = 0, FreezePositionX = 2, FreezePositionY = 4, FreezePositionZ = 8, FreezeRotationX = 16, FreezeRotationY = 32, FreezeRotationZ = 64, FreezePosition = 14, FreezeRotation = 112, FreezeAll = 126 };
    enum class CollisionDetectionMode : int { Discrete = 0, Continuous, ContinuousDynamic, ContinuousSpeculative };
    enum class RigidbodyInterpolation : int { None = 0, Interpolate, Extrapolate };
    enum class ForceMode : int { Force = 0, Impulse = 1, VelocityChange = 2, Acceleration = 5 };
    enum class Space : int { World = 0, Self = 1 };

    enum class PrimitiveType : int { Sphere = 0, Capsule, Cylinder, Cube, Plane, Quad };
    enum class SendMessageOptions : int { RequireReceiver = 0, DontRequireReceiver = 1 };

    enum class KeyCode : int {
        None = 0, Backspace = 8, Tab = 9, Clear = 12, Return = 13, Pause = 19, Escape = 27,
        Space = 32, Exclaim = 33, DoubleQuote = 34, Hash = 35, Dollar = 36, Ampersand = 38,
        Quote = 39, LeftParen = 40, RightParen = 41, Asterisk = 42, Plus = 43, Comma = 44,
        Minus = 45, Period = 46, Slash = 47,
        Alpha0 = 48, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,
        Colon = 58, Semicolon, Less, Equals, Greater, Question, At,
        LeftBracket = 91, Backslash, RightBracket, Caret, Underscore, BackQuote,
        A = 97, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Delete = 127,
        Keypad0 = 256, Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,
        KeypadPeriod, KeypadDivide, KeypadMultiply, KeypadMinus, KeypadPlus, KeypadEnter, KeypadEquals,
        UpArrow = 273, DownArrow, RightArrow, LeftArrow, Insert, Home, End, PageUp, PageDown,
        F1 = 282, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15,
        Numlock = 300, CapsLock, ScrollLock, RightShift, LeftShift, RightControl, LeftControl,
        RightAlt, LeftAlt, RightApple, LeftApple = 310, LeftWindows, RightWindows, AltGr = 313,
        Help = 315, Print, SysReq, Break, Menu,
        Mouse0 = 323, Mouse1, Mouse2, Mouse3, Mouse4, Mouse5, Mouse6,
    };

    enum class RenderMode : int {
        WorldSpace = 0,
        ScreenSpaceOverlay = 1,
        ScreenSpaceCamera = 2
    };

    enum class ImageType : int {
        Simple = 0,
        Sliced = 1,
        Tiled = 2,
        Filled = 3
    };

    enum class TextAlignmentOptions : int {
        TopLeft = 0x101,
        Top = 0x102,
        TopRight = 0x104,
        TopJustified = 0x108,
        Left = 0x201,
        Center = 0x202,
        Right = 0x204,
        Justified = 0x208,
        BottomLeft = 0x401,
        Bottom = 0x402,
        BottomRight = 0x404,
        BottomJustified = 0x408,
        BaselineLeft = 0x801,
        Baseline = 0x802,
        BaselineRight = 0x804,
        BaselineJustified = 0x808,
        MidlineLeft = 0x1001,
        Midline = 0x1002,
        MidlineRight = 0x1004,
        MidlineJustified = 0x1008,
        CaplineLeft = 0x2001,
        Capline = 0x2002,
        CaplineRight = 0x2004,
        CaplineJustified = 0x2008
    };

    enum class FontStyles : int {
        Normal = 0x0,
        Bold = 0x1,
        Italic = 0x2,
        Underline = 0x4,
        LowerCase = 0x8,
        UpperCase = 0x10,
        SmallCaps = 0x20,
        Strikethrough = 0x40,
        Superscript = 0x80,
        Subscript = 0x100,
        Highlight = 0x200
    };

    struct Touch {
        int fingerId; Vector2 position, rawPosition, deltaPosition;
        float deltaTime; int tapCount; TouchPhase phase; TouchType type;
        float pressure, maximumPossiblePressure, radius, radiusVariance, altitudeAngle, azimuthAngle;
    };

    struct HumanPose { Vector3 bodyPosition; Quaternion bodyRotation; /* il2cppArray<float> muscles omitted - requires IL2CPP types */ };

} // namespace IL2CPP
