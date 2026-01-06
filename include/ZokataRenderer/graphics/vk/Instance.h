#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ZKT
{
class Instance
{
public:
    /**
     * @brief Creates the Vulkan instance with required extensions.
     */
    Instance(const std::vector<const char*>& required_extensions);
    ~Instance();

    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;

    VkInstance Get() const;

private:
    VkInstance instance_ = VK_NULL_HANDLE;
};
}  // namespace ZKT
