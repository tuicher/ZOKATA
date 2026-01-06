#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "ZokataRenderer/graphics/vk/Device.h"
#include "ZokataRenderer/graphics/vk/FrameSync.h"
#include "ZokataRenderer/graphics/vk/Instance.h"
#include "ZokataRenderer/graphics/vk/Swapchain.h"

namespace ZKT
{
class IWindow;

struct FrameContext
{
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    uint32_t image_index = 0;
    VkExtent2D extent {};
};

enum class FrameStatus
{
    Ready,
    SwapchainOutOfDate
};

class VulkanContext
{
public:
    /**
     * @brief Manages swapchain, render pass, command buffers, and per-frame sync.
     */
    explicit VulkanContext(const IWindow& window);
    ~VulkanContext();

    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    FrameStatus BeginFrame(FrameContext& frame);
    /**
     * @brief Ends recording and presents the frame.
     */
    FrameStatus EndFrame(const FrameContext& frame);

    /**
     * @brief Recreate swapchain and dependent resources (e.g., after resize).
     */
    void RecreateSwapchain();
    /**
     * @brief Waits for the device to go idle.
     */
    void WaitIdle() const;

    /**
     * @brief Helper to submit a one-time command buffer.
     */
    void SubmitImmediate(const std::function<void(VkCommandBuffer)>& record) const;

    VkInstance InstanceHandle() const;
    VkPhysicalDevice PhysicalDevice() const;
    VkDevice DeviceHandle() const;
    VkQueue GraphicsQueue() const;
    VkQueue PresentQueue() const;
    uint32_t GraphicsQueueFamily() const;
    uint32_t PresentQueueFamily() const;
    VkRenderPass RenderPass() const;
    uint32_t SwapchainImageCount() const;
    uint32_t MinImageCount() const;
    VkFormat SwapchainFormat() const;
    VkExtent2D SwapchainExtent() const;

private:
    const IWindow& window_;

    std::unique_ptr<Instance> instance_;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    std::unique_ptr<Device> device_;
    std::unique_ptr<Swapchain> swapchain_;
    std::unique_ptr<FrameSyncPool> sync_pool_;

    VkRenderPass render_pass_ = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> framebuffers_;

    VkCommandPool command_pool_ = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> command_buffers_;

    uint32_t current_frame_ = 0;
    bool swapchain_dirty_ = false;

    void CreateInstance();
    void CreateSurface();
    void CreateCommandPool();
    void CreateRenderPass();
    void CreateFramebuffers();
    void AllocateCommandBuffers();
    void CleanupSwapchain();
};
}  // namespace ZKT
