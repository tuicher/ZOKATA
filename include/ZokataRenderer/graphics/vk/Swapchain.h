#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ZKT
{
class IWindow;
class Device;

class Swapchain
{
public:
    /**
     * @brief Manages the swapchain and its image views for presentation.
     */
    Swapchain(const Device& device, VkSurfaceKHR surface, const IWindow& window);
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    /**
     * @brief Rebuilds the swapchain for a new window extent/format.
     */
    void Recreate(const Device& device, VkSurfaceKHR surface, const IWindow& window);
    /**
     * @brief Explicitly releases swapchain and image view resources.
     */
    void Cleanup(const Device& device);

    VkSwapchainKHR Handle() const;
    VkFormat Format() const;
    VkExtent2D Extent() const;
    uint32_t MinImageCount() const;
    const std::vector<VkImage>& Images() const;
    const std::vector<VkImageView>& ImageViews() const;

private:
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> images_;
    std::vector<VkImageView> image_views_;
    VkFormat image_format_ = VK_FORMAT_B8G8R8A8_UNORM;
    VkExtent2D extent_ {};
    uint32_t min_image_count_ = 2;

    void CreateSwapchain(const Device& device, VkSurfaceKHR surface, const IWindow& window);
    void CreateImageViews(const Device& device);
};
}  // namespace ZKT
