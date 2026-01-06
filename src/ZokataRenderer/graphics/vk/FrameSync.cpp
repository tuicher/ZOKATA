#include "ZokataRenderer/graphics/vk/FrameSync.h"

#include <stdexcept>

namespace ZKT
{
FrameSyncPool::FrameSyncPool(VkDevice device, uint32_t frame_count)
    : device_(device)
{
    frames_.resize(frame_count);

    VkSemaphoreCreateInfo semaphore_info {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto& sync : frames_)
    {
        if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &sync.image_available) != VK_SUCCESS
            || vkCreateSemaphore(device_, &semaphore_info, nullptr, &sync.render_finished) != VK_SUCCESS
            || vkCreateFence(device_, &fence_info, nullptr, &sync.in_flight) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create synchronization objects.");
        }
    }
}

FrameSyncPool::~FrameSyncPool()
{
    if (device_ == VK_NULL_HANDLE)
    {
        return;
    }

    for (auto& sync : frames_)
    {
        vkDestroySemaphore(device_, sync.image_available, nullptr);
        vkDestroySemaphore(device_, sync.render_finished, nullptr);
        vkDestroyFence(device_, sync.in_flight, nullptr);
    }
}

FrameSync& FrameSyncPool::operator[](size_t index)
{
    return frames_[index];
}

const FrameSync& FrameSyncPool::operator[](size_t index) const
{
    return frames_[index];
}

size_t FrameSyncPool::Size() const
{
    return frames_.size();
}

std::vector<VkFence>& FrameSyncPool::ImagesInFlight()
{
    return images_in_flight_;
}
}  // namespace ZKT
