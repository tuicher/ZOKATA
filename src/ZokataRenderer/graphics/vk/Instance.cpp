#include "ZokataRenderer/graphics/vk/Instance.h"

#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

namespace ZKT
{
Instance::Instance(const std::vector<const char*>& required_extensions)
{
    VkApplicationInfo app_info {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "ZOKATA";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.pEngineName = "ZOKATA";
    app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    create_info.ppEnabledExtensionNames = required_extensions.data();

    if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
}

Instance::~Instance()
{
    if (instance_ != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instance_, nullptr);
        instance_ = VK_NULL_HANDLE;
    }
}

VkInstance Instance::Get() const
{
    return instance_;
}
}  // namespace ZKT
