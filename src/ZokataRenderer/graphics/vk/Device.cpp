#include "ZokataRenderer/graphics/vk/Device.h"

#include <set>
#include <stdexcept>
#include <vector>

namespace ZKT
{
namespace
{
const std::vector<const char*> kRequiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};
}  // namespace

Device::Device(VkInstance instance, VkSurfaceKHR surface)
{
    PickPhysicalDevice(instance, surface);
    CreateLogicalDevice(surface);
}

Device::~Device()
{
    if (device_ != VK_NULL_HANDLE)
    {
        vkDestroyDevice(device_, nullptr);
        device_ = VK_NULL_HANDLE;
    }
}

VkPhysicalDevice Device::Physical() const
{
    return physical_device_;
}

VkDevice Device::Logical() const
{
    return device_;
}

VkQueue Device::GraphicsQueue() const
{
    return graphics_queue_;
}

VkQueue Device::PresentQueue() const
{
    return present_queue_;
}

uint32_t Device::GraphicsQueueFamily() const
{
    return graphics_queue_family_;
}

uint32_t Device::PresentQueueFamily() const
{
    return present_queue_family_;
}

bool Device::QueueFamilyIndices::Complete() const
{
    return graphics_family.has_value() && present_family.has_value();
}

Device::QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
    QueueFamilyIndices indices;
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    for (uint32_t i = 0; i < queue_family_count; ++i)
    {
        const auto& family = queue_families[i];
        if (family.queueCount > 0 && (family.queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            indices.graphics_family = i;
        }

        VkBool32 present_supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_supported);
        if (family.queueCount > 0 && present_supported)
        {
            indices.present_family = i;
        }

        if (indices.Complete())
        {
            break;
        }
    }

    return indices;
}

void Device::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    if (device_count == 0)
    {
        throw std::runtime_error("No GPUs with Vulkan support detected.");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    auto check_device_extension_support = [](VkPhysicalDevice device) {
        uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(
            device, nullptr, &extension_count, available_extensions.data());

        std::set<std::string> required(kRequiredExtensions.begin(), kRequiredExtensions.end());
        for (const auto& ext : available_extensions)
        {
            required.erase(ext.extensionName);
        }
        return required.empty();
    };

    auto is_device_suitable = [&](VkPhysicalDevice device) -> bool
    {
        QueueFamilyIndices indices = FindQueueFamilies(device, surface);
        if (!indices.Complete())
        {
            return false;
        }

        if (!check_device_extension_support(device))
        {
            return false;
        }

        return true;
    };

    VkPhysicalDevice selected = VK_NULL_HANDLE;
    for (const auto& device : devices)
    {
        VkPhysicalDeviceProperties properties {};
        vkGetPhysicalDeviceProperties(device, &properties);
        const bool discrete = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

        if (is_device_suitable(device))
        {
            selected = device;
            if (discrete)
            {
                break;
            }
        }
    }

    if (selected == VK_NULL_HANDLE)
    {
        throw std::runtime_error("No suitable Vulkan device found.");
    }

    physical_device_ = selected;
}

void Device::CreateLogicalDevice(VkSurfaceKHR surface)
{
    QueueFamilyIndices indices = FindQueueFamilies(physical_device_, surface);
    graphics_queue_family_ = indices.graphics_family.value();
    present_queue_family_ = indices.present_family.value();

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_families = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    float queue_priority = 1.0F;
    for (uint32_t family : unique_families)
    {
        VkDeviceQueueCreateInfo queue_create {};
        queue_create.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create.queueFamilyIndex = family;
        queue_create.queueCount = 1;
        queue_create.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create);
    }

    VkPhysicalDeviceFeatures device_features {};
    VkDeviceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<uint32_t>(kRequiredExtensions.size());
    create_info.ppEnabledExtensionNames = kRequiredExtensions.data();

    if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device.");
    }

    vkGetDeviceQueue(device_, indices.graphics_family.value(), 0, &graphics_queue_);
    vkGetDeviceQueue(device_, indices.present_family.value(), 0, &present_queue_);
}
}  // namespace ZKT
