#pragma once

#include <glm/glm.hpp>
#include <type_traits>
#include <cmath>

namespace ZKT
{
namespace MATH
{
template <typename T>
struct Vec2
{
    T x {};
    T y {};

    /**
     * @brief Default-constructs to zero-initialized components.
     */
    constexpr Vec2() = default;
    /**
     * @brief Constructs with explicit x/y values.
     */
    constexpr Vec2(T x_, T y_) : x(x_), y(y_) {}
    explicit constexpr Vec2(const glm::vec<2, T>& v) : x(v.x), y(v.y) {}

    glm::vec<2, T> ToGlm() const { return {x, y}; }

    // Arithmetic
    Vec2 operator+(const Vec2& rhs) const { return {x + rhs.x, y + rhs.y}; }
    Vec2 operator-(const Vec2& rhs) const { return {x - rhs.x, y - rhs.y}; }
    Vec2 operator*(T scalar) const { return {x * scalar, y * scalar}; }
    Vec2 operator/(T scalar) const { return {x / scalar, y / scalar}; }

    Vec2& operator+=(const Vec2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    Vec2& operator-=(const Vec2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    Vec2& operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Vec2& operator/=(T scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    bool operator==(const Vec2& rhs) const { return x == rhs.x && y == rhs.y; }
    bool operator!=(const Vec2& rhs) const { return !(*this == rhs); }

    // Metrics
    /**
     * @brief Returns squared length (avoids sqrt when only comparing magnitudes).
     */
    T LengthSquared() const { return x * x + y * y; }
    /**
     * @brief Returns vector length.
     */
    T Length() const { return static_cast<T>(std::sqrt(LengthSquared())); }

    /**
     * @brief Returns a normalized copy (safe for zero length).
     */
    Vec2 Normalized() const
    {
        const T len = Length();
        return len == static_cast<T>(0) ? Vec2{} : (*this) / len;
    }

    /**
     * @brief Dot product of two 2D vectors.
     */
    static T Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
};

template <typename T>
struct Vec3
{
    T x {};
    T y {};
    T z {};

    constexpr Vec3() = default;
    constexpr Vec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
    explicit constexpr Vec3(const glm::vec<3, T>& v) : x(v.x), y(v.y), z(v.z) {}

    glm::vec<3, T> ToGlm() const { return {x, y, z}; }

    // Arithmetic
    Vec3 operator+(const Vec3& rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
    Vec3 operator-(const Vec3& rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
    Vec3 operator*(T scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    Vec3 operator/(T scalar) const { return {x / scalar, y / scalar, z / scalar}; }

    Vec3& operator+=(const Vec3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    Vec3& operator-=(const Vec3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    Vec3& operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    Vec3& operator/=(T scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    bool operator==(const Vec3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!=(const Vec3& rhs) const { return !(*this == rhs); }

    // Metrics
    /**
     * @brief Returns squared length (avoids sqrt when only comparing magnitudes).
     */
    T LengthSquared() const { return x * x + y * y + z * z; }
    /**
     * @brief Returns vector length.
     */
    T Length() const { return static_cast<T>(std::sqrt(LengthSquared())); }

    /**
     * @brief Returns a normalized copy (safe for zero length).
     */
    Vec3 Normalized() const
    {
        const T len = Length();
        return len == static_cast<T>(0) ? Vec3{} : (*this) / len;
    }

    /**
     * @brief Dot product of two 3D vectors.
     */
    static T Dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    /**
     * @brief Cross product of two 3D vectors.
     */
    static Vec3 Cross(const Vec3& a, const Vec3& b)
    {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x,
        };
    }
};

template <typename T>
struct Vec4
{
    T x {};
    T y {};
    T z {};
    T w {};

    /**
     * @brief Default-constructs to zero-initialized components.
     */
    constexpr Vec4() = default;
    /**
     * @brief Constructs with explicit x/y/z/w values.
     */
    constexpr Vec4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
    explicit constexpr Vec4(const glm::vec<4, T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    glm::vec<4, T> ToGlm() const { return {x, y, z, w}; }

    // Arithmetic
    Vec4 operator+(const Vec4& rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w}; }
    Vec4 operator-(const Vec4& rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w}; }
    Vec4 operator*(T scalar) const { return {x * scalar, y * scalar, z * scalar, w * scalar}; }
    Vec4 operator/(T scalar) const { return {x / scalar, y / scalar, z / scalar, w / scalar}; }

    Vec4& operator+=(const Vec4& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }
    Vec4& operator-=(const Vec4& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }
    Vec4& operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }
    Vec4& operator/=(T scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    bool operator==(const Vec4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
    bool operator!=(const Vec4& rhs) const { return !(*this == rhs); }

    // Metrics
    /**
     * @brief Returns squared length (avoids sqrt when only comparing magnitudes).
     */
    T LengthSquared() const { return x * x + y * y + z * z + w * w; }
    /**
     * @brief Returns vector length.
     */
    T Length() const { return static_cast<T>(std::sqrt(LengthSquared())); }

    /**
     * @brief Returns a normalized copy (safe for zero length).
     */
    Vec4 Normalized() const
    {
        const T len = Length();
        return len == static_cast<T>(0) ? Vec4{} : (*this) / len;
    }

    /**
     * @brief Dot product of two 4D vectors.
     */
    static T Dot(const Vec4& a, const Vec4& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
};

using Vec2i = Vec2<int>;
using Vec3i = Vec3<int>;
using Vec4i = Vec4<int>;

using Vec2f = Vec2<float>;
using Vec3f = Vec3<float>;
using Vec4f = Vec4<float>;

using Vec2d = Vec2<double>;
using Vec3d = Vec3<double>;
using Vec4d = Vec4<double>;
}  // namespace MATH
}  // namespace ZKT
