#include "ZokataMath/Matrix.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace ZKT
{
namespace MATH
{
Mat4::Mat4() = default;

Mat4::Mat4(const glm::mat4& value)
    : value_(value)
{
}

Mat4 Mat4::Identity()
{
    return Mat4(glm::mat4(1.0F));
}

Mat4 Mat4::FromTRS(const Vec3f& translation, const Quaternion& rotation, const Vec3f& scale)
{
    glm::mat4 mat = glm::translate(glm::mat4(1.0F), translation.ToGlm());
    mat *= glm::mat4_cast(rotation.ToGlm());
    mat = glm::scale(mat, scale.ToGlm());
    return Mat4(mat);
}

const glm::mat4& Mat4::ToGlm() const
{
    return value_;
}

glm::mat4& Mat4::ToGlm()
{
    return value_;
}
}  // namespace MATH
}  // namespace ZKT
