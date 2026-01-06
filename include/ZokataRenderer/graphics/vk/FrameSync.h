#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ZKT
{
struct FrameSync
{
    VkSemaphore image_available = VK_NULL_HANDLE;
    VkSemaphore render_finished = VK_NULL_HANDLE;
    VkFence in_flight = VK_NULL_HANDLE;
};

class FrameSyncPool
{
public:
    /**
     * @brief Allocates per-frame semaphores and fences for render/present.
     */
    explicit FrameSyncPool(VkDevice device, uint32_t frame_count);
    ~FrameSyncPool();

    FrameSyncPool(const FrameSyncPool&) = delete;
    FrameSyncPool& operator=(const FrameSyncPool&) = delete;

    FrameSync& operator[](size_t index);
    const FrameSync& operator[](size_t index) const;
    size_t Size() const;
    std::vector<VkFence>& ImagesInFlight();

private:
    VkDevice device_ = VK_NULL_HANDLE;
    std::vector<FrameSync> frames_;
    std::vector<VkFence> images_in_flight_;
};
}  // namespace ZKT
