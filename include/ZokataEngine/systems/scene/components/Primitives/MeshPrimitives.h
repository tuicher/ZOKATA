#pragma once

#include <cstdint>

#include "ZokataMath/Vector.h"
#include "ZokataRenderer/graphics/renderer/Renderable.h"

namespace ZKT
{
namespace ENGINE
{
/**
 * @brief Parameters for a cube (box) mesh.
 */
struct CubeParams
{
    MATH::Vec3f size {1.0F, 1.0F, 1.0F};
};

/**
 * @brief Parameters for a UV sphere mesh.
 */
struct SphereParams
{
    float radius = 0.5F;
    uint32_t longitude_segments = 32;
    uint32_t latitude_segments = 16;
};

/**
 * @brief Builds a cube mesh with positions, normals, uvs, tangents, and indices.
 */
MeshGeometry MakeCube(const CubeParams& params = {});

/**
 * @brief Builds a UV sphere mesh with positions, normals, uvs, tangents, and indices.
 */
MeshGeometry MakeSphere(const SphereParams& params = {});
}  // namespace ENGINE
}  // namespace ZKT
