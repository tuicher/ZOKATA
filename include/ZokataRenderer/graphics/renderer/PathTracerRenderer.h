#pragma once

#include <glm/vec3.hpp>

#include "ZokataRenderer/graphics/renderer/Renderer.h"

namespace ZKT
{
/**
 * @brief Hardware ray-tracing path tracer with basic UI controls.
 */
class PathTracerRenderer final : public IRenderer
{
public:
    PathTracerRenderer() = default;
    const char* Name() const override;
    RendererType Type() const override;
    void RenderGui(const FrameDescriptor& frame) override;

private:
    glm::vec3 sky_color_ {0.15F, 0.2F, 0.35F};
    int samples_per_pixel_ = 1;
    int max_bounces_ = 4;
};
}  // namespace ZKT
