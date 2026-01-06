#include "ZokataRenderer/graphics/vk/Swapchain.h"

#include "ZokataRenderer/graphics/Window.h"
#include "ZokataRenderer/graphics/vk/Device.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

namespace ZKT
{
namespace
{
struct SwapchainSupport
{
    VkSurfaceCapabilitiesKHR capabilities {};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

SwapchainSupport QuerySwapchainSupport(const Device& device, VkSurfaceKHR surface)
{
    SwapchainSupport support;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.Physical(), surface, &support.capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.Physical(), surface, &format_count, nullptr);
    if (format_count != 0)
    {
        support.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.Physical(), surface, &format_count, support.formats.data());
    }

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.Physical(), surface, &present_mode_count, nullptr);
    if (present_mode_count != 0)
    {
        support.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device.Physical(), surface, &present_mode_count, support.present_modes.data());
    }

    return support;
}

VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available)
{
    for (const auto& format : available)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB
            && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    return available.front();
}

VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& available)
{
    for (const auto& mode : available)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const IWindow& window)
{
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
    {
        return capabilities.currentExtent;
    }

    VkExtent2D actual_extent = window.GetFramebufferExtent();
    actual_extent.width = std::clamp(
        actual_extent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(
        actual_extent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height);
    return actual_extent;
}
}  // namespace

Swapchain::Swapchain(const Device& device, VkSurfaceKHR surface, const IWindow& window)
{
    CreateSwapchain(device, surface, window);
    CreateImageViews(device);
}

Swapchain::~Swapchain()
{
    // No automatic cleanup, call Cleanup explicitly with device.
}

VkSwapchainKHR Swapchain::Handle() const
{
    return swapchain_;
}

VkFormat Swapchain::Format() const
{
    return image_format_;
}

VkExtent2D Swapchain::Extent() const
{
    return extent_;
}

uint32_t Swapchain::MinImageCount() const
{
    return min_image_count_;
}

const std::vector<VkImage>& Swapchain::Images() const
{
    return images_;
}

const std::vector<VkImageView>& Swapchain::ImageViews() const
{
    return image_views_;
}

void Swapchain::Recreate(const Device& device, VkSurfaceKHR surface, const IWindow& window)
{
    Cleanup(device);
    CreateSwapchain(device, surface, window);
    CreateImageViews(device);
}

void Swapchain::Cleanup(const Device& device)
{
    for (auto view : image_views_)
    {
        vkDestroyImageView(device.Logical(), view, nullptr);
    }
    image_views_.clear();
    images_.clear();

    if (swapchain_ != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(device.Logical(), swapchain_, nullptr);
        swapchain_ = VK_NULL_HANDLE;
    }
}

void Swapchain::CreateSwapchain(const Device& device, VkSurfaceKHR surface, const IWindow& window)
{
    const SwapchainSupport support = QuerySwapchainSupport(device, surface);
    const VkSurfaceFormatKHR surface_format = ChooseSurfaceFormat(support.formats);
    const VkPresentModeKHR present_mode = ChoosePresentMode(support.present_modes);
    const VkExtent2D extent = ChooseSwapExtent(support.capabilities, window);

    uint32_t image_count = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0
        && image_count > support.capabilities.maxImageCount)
    {
        image_count = support.capabilities.maxImageCount;
    }

    min_image_count_ = image_count;

    VkSwapchainCreateInfoKHR create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const uint32_t queue_family_indices[] = {
        device.GraphicsQueueFamily(),
        device.PresentQueueFamily()
    };

    if (device.GraphicsQueueFamily() != device.PresentQueueFamily())
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = swapchain_;

    VkSwapchainKHR new_swapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(device.Logical(), &create_info, nullptr, &new_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swapchain.");
    }

    if (create_info.oldSwapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(device.Logical(), create_info.oldSwapchain, nullptr);
    }

    swapchain_ = new_swapchain;
    image_format_ = surface_format.format;
    extent_ = extent;

    uint32_t retrieved_image_count = 0;
    vkGetSwapchainImagesKHR(device.Logical(), swapchain_, &retrieved_image_count, nullptr);
    images_.resize(retrieved_image_count);
    vkGetSwapchainImagesKHR(device.Logical(), swapchain_, &retrieved_image_count, images_.data());
}

void Swapchain::CreateImageViews(const Device& device)
{
    image_views_.resize(images_.size());
    for (size_t i = 0; i < images_.size(); ++i)
    {
        VkImageViewCreateInfo view_info {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = images_[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = image_format_;
        view_info.components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        };
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device.Logical(), &view_info, nullptr, &image_views_[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swapchain image view.");
        }
    }
}
}  // namespace ZKT
