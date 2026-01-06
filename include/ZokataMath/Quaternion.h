#pragma once

// Needed for glm/gtx/euler_angles.hpp and other experimental extensions.
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include "ZokataMath/Vector.h"

namespace ZKT
{
namespace MATH
{
/**
 * @brief Lightweight wrapper over glm::quat with friendly rotation utilities.
 */
class Quaternion
{
public:
    /**
     * @brief Constructs the identity quaternion.
     */
    Quaternion();
    /**
     * @brief Wraps an existing glm::quat.
     */
    explicit Quaternion(const glm::quat& q);
    /**
     * @brief Constructs from raw components (w, x, y, z).
     */
    Quaternion(float w, float x, float y, float z);

    /**
     * @brief Returns the identity quaternion.
     */
    static Quaternion Identity();

    /**
     * @brief Builds a quaternion from Euler angles in degrees.
     */
    static Quaternion FromEulerDegrees(const Vec3f& euler_degrees);
    /**
     * @brief Builds a quaternion from Euler angles in radians.
     */
    static Quaternion FromEulerRadians(const Vec3f& euler_radians);

    /**
     * @brief Converts to Euler angles in degrees.
     */
    Vec3f ToEulerDegrees() const;
    /**
     * @brief Converts to Euler angles in radians.
     */
    Vec3f ToEulerRadians() const;

    /**
     * @brief Builds a quaternion from an axis-angle pair (degrees).
     */
    static Quaternion FromAxisAngle(const Vec3f& axis, float angle_degrees);

    /**
     * @brief Returns a normalized copy.
     */
    Quaternion Normalized() const;
    /**
     * @brief Normalizes in place.
     */
    void Normalize();

    /**
     * @brief Conjugate of this quaternion.
     */
    Quaternion Conjugate() const;
    /**
     * @brief Inverse of this quaternion.
     */
    Quaternion Inverse() const;

    // Rotate a vector by this quaternion.
    /**
     * @brief Rotates a vector using this quaternion.
     */
    Vec3f Rotate(const Vec3f& v) const;

    // Operations
    Quaternion operator*(const Quaternion& rhs) const;
    Quaternion& operator*=(const Quaternion& rhs);

    bool operator==(const Quaternion& rhs) const;
    bool operator!=(const Quaternion& rhs) const;

    /**
     * @brief Access to the underlying glm quaternion.
     */
    const glm::quat& ToGlm() const;
    glm::quat& ToGlm();

    // Direct setter when you already have 4 components.
    /**
     * @brief Sets components directly (w, x, y, z).
     */
    void Set(float w, float x, float y, float z);

    /**
     * @brief Spherical linear interpolation between two quaternions.
     */
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

private:
    glm::quat value_ {1.0F, 0.0F, 0.0F, 0.0F};
};
}  // namespace MATH
}  // namespace ZKT
