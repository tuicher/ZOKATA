#pragma once

#include <cstdint>

#include <vulkan/vulkan.h>

namespace ZKT
{
struct FrameDescriptor
{
    float delta_seconds = 0.0F;
    uint64_t frame_index = 0;
};

enum class RendererType
{
    PathTracer,
    Deferred
};

class IRenderer
{
public:
    virtual ~IRenderer() = default;
    /**
     * @brief Human-friendly renderer name (for UI/logs).
     */
    virtual const char* Name() const = 0;
    /**
     * @brief Renderer kind (e.g. Deferred, PathTracer).
     */
    virtual RendererType Type() const = 0;
    /**
     * @brief Draws the renderer's control/debug UI.
     */
    virtual void RenderGui(const FrameDescriptor& frame) = 0;
    virtual void OnSwapchainUpdated(VkExtent2D extent);
};
}  // namespace ZKT
