#pragma once

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>

namespace ZKT
{
namespace MATH
{
class MathUtils
{
public:
    /**
     * @brief Simple math utilities (conversions, trig, clamp/lerp).
     */
    /**
     * @brief Converts degrees to radians.
     */
    static float Radians(float degrees);
    /**
     * @brief Converts radians to degrees.
     */
    static float Degrees(float radians);

    /**
     * @brief Sine of an angle in radians.
     */
    static float Sin(float radians);
    /**
     * @brief Cosine of an angle in radians.
     */
    static float Cos(float radians);
    /**
     * @brief Tangent of an angle in radians.
     */
    static float Tan(float radians);

    /**
     * @brief Clamps value within [min_v, max_v].
     */
    static float Clamp(float value, float min_v, float max_v);

    /**
     * @brief Linear interpolation between a and b by t.
     */
    static float Lerp(float a, float b, float t);
};
}  // namespace MATH
}  // namespace ZKT
