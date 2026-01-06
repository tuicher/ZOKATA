#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ZokataMath/Vector.h"

namespace ZKT
{
// CPU-side mesh data required by the renderer. No API-specific state.
/**
 * @brief CPU-side mesh vertex, graphics-API agnostic.
 */
struct MeshVertex
{
    MATH::Vec3f position {0.0F, 0.0F, 0.0F};
    MATH::Vec3f normal {0.0F, 0.0F, 1.0F};
    MATH::Vec2f uv {0.0F, 0.0F};
    MATH::Vec3f tangent {1.0F, 0.0F, 0.0F};
};

struct MeshGeometry
{
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    bool Empty() const { return vertices.empty(); }
    bool Indexed() const { return !indices.empty(); }
};

struct MaterialDescriptor
{
    std::string shader_id;              // TODO: promote to a strong material/shader handle
    std::vector<std::string> textures;  // TODO: replace with typed slots (albedo, normal, etc.)
};

// Minimal interface a renderer needs to draw something. No Vulkan surface leaks here.
/**
 * @brief Minimal contract a drawable must satisfy for the renderer.
 */
class Renderable
{
public:
    virtual ~Renderable() = default;

    /**
     * @brief CPU geometry data to draw.
     */
    virtual const MeshGeometry& Geometry() const = 0;
    /**
     * @brief Material/shader metadata for this renderable.
     */
    virtual const MaterialDescriptor& Material() const = 0;

    /**
     * @brief Whether the renderable should be considered for rendering.
     */
    virtual bool IsVisible() const;
    /**
     * @brief Indicates CPU data changed and GPU upload is pending.
     */
    virtual bool NeedsUpload() const;  // CPU data changed; GPU upload pending.
    /**
     * @brief Marks CPU data as synchronized with GPU resources.
     */
    virtual void MarkUploaded();       // Renderer calls after syncing resources.
};
}  // namespace ZKT
