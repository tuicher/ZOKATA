#include "ZokataMath/MathUtils.h"

namespace ZKT
{
namespace MATH
{
float MathUtils::Radians(float degrees)
{
    return glm::radians(degrees);
}

float MathUtils::Degrees(float radians)
{
    return glm::degrees(radians);
}

float MathUtils::Sin(float radians)
{
    return glm::sin(radians);
}

float MathUtils::Cos(float radians)
{
    return glm::cos(radians);
}

float MathUtils::Tan(float radians)
{
    return glm::tan(radians);
}

float MathUtils::Clamp(float value, float min_v, float max_v)
{
    return (std::max)(min_v, (std::min)(value, max_v));
}

float MathUtils::Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}
}  // namespace MATH
}  // namespace ZKT
