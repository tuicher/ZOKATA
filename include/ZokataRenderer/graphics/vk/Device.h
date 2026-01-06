#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

namespace ZKT
{
class Device
{
public:
    /**
     * @brief Picks a physical device and creates the logical one with graphics/present queues.
     */
    Device(VkInstance instance, VkSurfaceKHR surface);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    VkPhysicalDevice Physical() const;
    VkDevice Logical() const;
    VkQueue GraphicsQueue() const;
    VkQueue PresentQueue() const;
    uint32_t GraphicsQueueFamily() const;
    uint32_t PresentQueueFamily() const;

private:
    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue graphics_queue_ = VK_NULL_HANDLE;
    VkQueue present_queue_ = VK_NULL_HANDLE;
    uint32_t graphics_queue_family_ = 0;
    uint32_t present_queue_family_ = 0;

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        [[nodiscard]] bool Complete() const;
    };

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const;
    void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    void CreateLogicalDevice(VkSurfaceKHR surface);
};
}  // namespace ZKT
