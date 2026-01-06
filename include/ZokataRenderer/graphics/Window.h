#pragma once

#include <cstdint>
#include <string>

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace ZKT
{
struct WindowConfig
{
    uint32_t width = 1280;
    uint32_t height = 720;
    std::string title = "ZOKATA";
};

/**
 * @brief Abstract window interface for event handling and surface creation.
 */
class IWindow
{
public:
    virtual ~IWindow() = default;
    /**
     * @brief Process window and input events.
     */
    virtual void PollEvents() = 0;
    /**
     * @brief Whether the window requested close.
     */
    virtual bool ShouldClose() const = 0;
    /**
     * @brief Create a VkSurfaceKHR bound to this window.
     */
    virtual VkSurfaceKHR CreateSurface(VkInstance instance) const = 0;
    /**
     * @brief Current framebuffer extent in pixels.
     */
    virtual VkExtent2D GetFramebufferExtent() const = 0;
    /**
     * @brief Indicates a resize occurred since last reset.
     */
    virtual bool WasResized() const = 0;
    /**
     * @brief Clears the resize flag.
     */
    virtual void ResetResizedFlag() = 0;
    virtual GLFWwindow* GetNativeHandle() const = 0;
};

class GlfwWindow final : public IWindow
{
public:
    explicit GlfwWindow(const WindowConfig& config);
    ~GlfwWindow() override;

    void PollEvents() override;
    bool ShouldClose() const override;
    VkSurfaceKHR CreateSurface(VkInstance instance) const override;
    VkExtent2D GetFramebufferExtent() const override;
    bool WasResized() const override;
    void ResetResizedFlag() override;
    GLFWwindow* GetNativeHandle() const override;

private:
    GLFWwindow* window_ = nullptr;
    bool framebuffer_resized_ = false;
};
}  // namespace ZKT
