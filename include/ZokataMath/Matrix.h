#pragma once

#include <glm/glm.hpp>

#include "ZokataMath/Quaternion.h"
#include "ZokataMath/Vector.h"

namespace ZKT
{
namespace MATH
{
/**
 * @brief Lightweight wrapper over glm::mat4 for engine-facing math.
 */
class Mat4
{
public:
    /**
     * @brief Constructs the identity matrix.
     */
    Mat4();
    /**
     * @brief Wraps an existing glm::mat4.
     */
    explicit Mat4(const glm::mat4& value);

    /**
     * @brief Returns the identity matrix.
     */
    static Mat4 Identity();

    /**
     * @brief Builds a matrix from translation, rotation, and scale.
     */
    static Mat4 FromTRS(const Vec3f& translation, const Quaternion& rotation, const Vec3f& scale);

    /**
     * @brief Access to the underlying glm matrix.
     */
    const glm::mat4& ToGlm() const;
    glm::mat4& ToGlm();

private:
    glm::mat4 value_ {1.0F};
};

using Mat4f = Mat4;
}  // namespace MATH
}  // namespace ZKT
