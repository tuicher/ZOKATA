#pragma once

#include <memory>
#include <functional>

#include "ZokataRenderer/graphics/ImGuiLayer.h"
#include "ZokataRenderer/graphics/VulkanContext.h"
#include "ZokataRenderer/graphics/Window.h"
#include "ZokataRenderer/graphics/renderer/DeferredRenderer.h"
#include "ZokataRenderer/graphics/renderer/Renderer.h"

namespace ZKT
{
/**
 * @brief Entry point for the render layer: window, Vulkan context, and ImGui.
 */
class Application
{
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /**
     * @brief Main loop: handles window events, GUI, and renderer frames.
     */
    void Run();
    /**
     * @brief Provides an extra ImGui callback executed each frame.
     */
    void SetGuiCallback(std::function<void()> callback);

private:
    GlfwWindow window_;
    VulkanContext context_;
    ImGuiLayer imgui_layer_;
    std::unique_ptr<IRenderer> renderer_;
    uint64_t frame_index_ = 0;
    std::function<void()> extra_gui_;

    void SetupImGui();
    void RecreateSwapchainAndUi();
    IRenderer& ActiveRenderer();
};
}  // namespace ZKT
