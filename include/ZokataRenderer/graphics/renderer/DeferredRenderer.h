#pragma once

#include "ZokataRenderer/graphics/renderer/Renderer.h"

namespace ZKT
{
/**
 * @brief Placeholder deferred renderer that provides basic UI and state.
 */
class DeferredRenderer final : public IRenderer
{
public:
    DeferredRenderer() = default;

    const char* Name() const override;
    RendererType Type() const override;
    void RenderGui(const FrameDescriptor& frame) override;
    void OnSwapchainUpdated(VkExtent2D extent) override;

private:
    VkExtent2D last_extent_ {0, 0};
    bool recreate_gbuffer_ = false;
    int samples_ = 1;
};
}  // namespace ZKT
