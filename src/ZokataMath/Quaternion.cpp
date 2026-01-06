#include "ZokataMath/Quaternion.h"

namespace ZKT
{
namespace MATH
{
Quaternion::Quaternion() = default;

Quaternion::Quaternion(const glm::quat& q)
    : value_(q)
{
}

Quaternion::Quaternion(float w, float x, float y, float z)
    : value_(w, x, y, z)
{
}

Quaternion Quaternion::Identity()
{
    return Quaternion();
}

Quaternion Quaternion::FromEulerDegrees(const Vec3f& euler_degrees)
{
    glm::vec3 radians = glm::radians(glm::vec3{euler_degrees.x, euler_degrees.y, euler_degrees.z});
    return Quaternion(glm::quat(radians));
}

Quaternion Quaternion::FromEulerRadians(const Vec3f& euler_radians)
{
    return Quaternion(glm::quat(glm::vec3{euler_radians.x, euler_radians.y, euler_radians.z}));
}

Vec3f Quaternion::ToEulerDegrees() const
{
    glm::vec3 radians = glm::eulerAngles(value_);
    glm::vec3 degrees = glm::degrees(radians);
    return Vec3f{degrees.x, degrees.y, degrees.z};
}

Vec3f Quaternion::ToEulerRadians() const
{
    glm::vec3 radians = glm::eulerAngles(value_);
    return Vec3f{radians.x, radians.y, radians.z};
}

Quaternion Quaternion::FromAxisAngle(const Vec3f& axis, float angle_degrees)
{
    const float angle_rad = glm::radians(angle_degrees);
    return Quaternion(glm::angleAxis(angle_rad, axis.ToGlm()));
}

Quaternion Quaternion::Normalized() const
{
    return Quaternion(glm::normalize(value_));
}

void Quaternion::Normalize()
{
    value_ = glm::normalize(value_);
}

Quaternion Quaternion::Conjugate() const
{
    return Quaternion(glm::conjugate(value_));
}

Quaternion Quaternion::Inverse() const
{
    return Quaternion(glm::inverse(value_));
}

Vec3f Quaternion::Rotate(const Vec3f& v) const
{
    glm::vec3 rotated = value_ * v.ToGlm();
    return Vec3f{rotated.x, rotated.y, rotated.z};
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
    return Quaternion(value_ * rhs.value_);
}

Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
    value_ *= rhs.value_;
    return *this;
}

bool Quaternion::operator==(const Quaternion& rhs) const
{
    return value_.x == rhs.value_.x && value_.y == rhs.value_.y && value_.z == rhs.value_.z &&
           value_.w == rhs.value_.w;
}

bool Quaternion::operator!=(const Quaternion& rhs) const
{
    return !(*this == rhs);
}

const glm::quat& Quaternion::ToGlm() const
{
    return value_;
}

glm::quat& Quaternion::ToGlm()
{
    return value_;
}

void Quaternion::Set(float w, float x, float y, float z)
{
    value_ = glm::quat(w, x, y, z);
}

Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
{
    return Quaternion(glm::slerp(a.value_, b.value_, t));
}
}  // namespace MATH
}  // namespace ZKT
