#include "ZokataRenderer/graphics/VulkanContext.h"

#include "ZokataRenderer/graphics/Window.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <GLFW/glfw3.h>

namespace ZKT
{
namespace
{
constexpr uint32_t kMaxFramesInFlight = 2;
}  // namespace

VulkanContext::VulkanContext(const IWindow& window)
    : window_(window)
{
    CreateInstance();
    CreateSurface();
    device_ = std::make_unique<Device>(instance_->Get(), surface_);
    swapchain_ = std::make_unique<Swapchain>(*device_, surface_, window_);
    CreateCommandPool();
    CreateRenderPass();
    CreateFramebuffers();
    AllocateCommandBuffers();
    sync_pool_ = std::make_unique<FrameSyncPool>(device_->Logical(), kMaxFramesInFlight);
}

VulkanContext::~VulkanContext()
{
    WaitIdle();

    CleanupSwapchain();

    if (command_pool_ != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(device_->Logical(), command_pool_, nullptr);
    }

    if (surface_ != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(instance_->Get(), surface_, nullptr);
        surface_ = VK_NULL_HANDLE;
    }
}

VkInstance VulkanContext::InstanceHandle() const
{
    return instance_->Get();
}

VkPhysicalDevice VulkanContext::PhysicalDevice() const
{
    return device_->Physical();
}

VkDevice VulkanContext::DeviceHandle() const
{
    return device_->Logical();
}

VkQueue VulkanContext::GraphicsQueue() const
{
    return device_->GraphicsQueue();
}

VkQueue VulkanContext::PresentQueue() const
{
    return device_->PresentQueue();
}

uint32_t VulkanContext::GraphicsQueueFamily() const
{
    return device_->GraphicsQueueFamily();
}

uint32_t VulkanContext::PresentQueueFamily() const
{
    return device_->PresentQueueFamily();
}

VkRenderPass VulkanContext::RenderPass() const
{
    return render_pass_;
}

uint32_t VulkanContext::SwapchainImageCount() const
{
    return static_cast<uint32_t>(swapchain_->Images().size());
}

uint32_t VulkanContext::MinImageCount() const
{
    return swapchain_->MinImageCount();
}

VkFormat VulkanContext::SwapchainFormat() const
{
    return swapchain_->Format();
}

VkExtent2D VulkanContext::SwapchainExtent() const
{
    return swapchain_->Extent();
}

void VulkanContext::WaitIdle() const
{
    if (device_)
    {
        vkDeviceWaitIdle(device_->Logical());
    }
}

void VulkanContext::CreateInstance()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    if (!glfw_extensions || glfw_extension_count == 0)
    {
        throw std::runtime_error("GLFW did not provide any instance extensions.");
    }

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    instance_ = std::make_unique<Instance>(extensions);
}

void VulkanContext::CreateSurface()
{
    surface_ = window_.CreateSurface(instance_->Get());
}

void VulkanContext::CreateCommandPool()
{
    VkCommandPoolCreateInfo pool_info {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = device_->GraphicsQueueFamily();
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device_->Logical(), &pool_info, nullptr, &command_pool_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command pool.");
    }
}

void VulkanContext::CreateRenderPass()
{
    VkAttachmentDescription color_attachment {};
    color_attachment.format = swapchain_->Format();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(device_->Logical(), &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass.");
    }
}

void VulkanContext::CreateFramebuffers()
{
    const auto& image_views = swapchain_->ImageViews();
    framebuffers_.resize(image_views.size());
    for (size_t i = 0; i < image_views.size(); ++i)
    {
        VkImageView attachments[] = {image_views[i]};

        VkFramebufferCreateInfo framebuffer_info {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = render_pass_;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = swapchain_->Extent().width;
        framebuffer_info.height = swapchain_->Extent().height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(device_->Logical(), &framebuffer_info, nullptr, &framebuffers_[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create framebuffer.");
        }
    }
}

void VulkanContext::AllocateCommandBuffers()
{
    if (!command_buffers_.empty())
    {
        vkFreeCommandBuffers(
            device_->Logical(),
            command_pool_,
            static_cast<uint32_t>(command_buffers_.size()),
            command_buffers_.data());
    }

    command_buffers_.resize(framebuffers_.size());
    VkCommandBufferAllocateInfo alloc_info {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool_;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

    if (vkAllocateCommandBuffers(device_->Logical(), &alloc_info, command_buffers_.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers.");
    }
}

void VulkanContext::CleanupSwapchain()
{
    if (!command_buffers_.empty())
    {
        vkFreeCommandBuffers(
            device_->Logical(),
            command_pool_,
            static_cast<uint32_t>(command_buffers_.size()),
            command_buffers_.data());
        command_buffers_.clear();
    }

    for (auto framebuffer : framebuffers_)
    {
        vkDestroyFramebuffer(device_->Logical(), framebuffer, nullptr);
    }
    framebuffers_.clear();

    if (render_pass_ != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(device_->Logical(), render_pass_, nullptr);
        render_pass_ = VK_NULL_HANDLE;
    }

    if (swapchain_)
    {
        swapchain_->Cleanup(*device_);
    }
}

void VulkanContext::RecreateSwapchain()
{
    VkExtent2D extent = window_.GetFramebufferExtent();
    while (extent.width == 0 || extent.height == 0)
    {
        glfwWaitEvents();
        extent = window_.GetFramebufferExtent();
    }

    WaitIdle();
    CleanupSwapchain();
    swapchain_->Recreate(*device_, surface_, window_);
    sync_pool_->ImagesInFlight().assign(swapchain_->Images().size(), VK_NULL_HANDLE);
    CreateRenderPass();
    CreateFramebuffers();
    AllocateCommandBuffers();
    swapchain_dirty_ = false;
    current_frame_ = 0;
}

FrameStatus VulkanContext::BeginFrame(FrameContext& frame)
{
    if (window_.WasResized())
    {
        swapchain_dirty_ = true;
    }

    if (swapchain_dirty_)
    {
        return FrameStatus::SwapchainOutOfDate;
    }

    FrameSync& sync = (*sync_pool_)[current_frame_];
    vkWaitForFences(device_->Logical(), 1, &sync.in_flight, VK_TRUE, UINT64_MAX);

    uint32_t image_index = 0;
    VkResult acquire = vkAcquireNextImageKHR(
        device_->Logical(),
        swapchain_->Handle(),
        UINT64_MAX,
        sync.image_available,
        VK_NULL_HANDLE,
        &image_index);

    if (acquire == VK_ERROR_OUT_OF_DATE_KHR)
    {
        swapchain_dirty_ = true;
        return FrameStatus::SwapchainOutOfDate;
    }

    auto& images_in_flight = sync_pool_->ImagesInFlight();
    if (images_in_flight.empty())
    {
        images_in_flight.assign(swapchain_->Images().size(), VK_NULL_HANDLE);
    }

    if (images_in_flight[image_index] != VK_NULL_HANDLE)
    {
        vkWaitForFences(device_->Logical(), 1, &images_in_flight[image_index], VK_TRUE, UINT64_MAX);
    }
    images_in_flight[image_index] = sync.in_flight;

    vkResetFences(device_->Logical(), 1, &sync.in_flight);

    VkCommandBuffer command_buffer = command_buffers_[image_index];
    vkResetCommandBuffer(command_buffer, 0);

    VkCommandBufferBeginInfo begin_info {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording a command buffer.");
    }

    VkClearValue clear_color = {{0.02F, 0.02F, 0.025F, 1.0F}};

    VkRenderPassBeginInfo render_pass_info {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass_;
    render_pass_info.framebuffer = framebuffers_[image_index];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swapchain_->Extent();
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    frame.command_buffer = command_buffer;
    frame.image_index = image_index;
    frame.extent = swapchain_->Extent();

    return swapchain_dirty_ ? FrameStatus::SwapchainOutOfDate : FrameStatus::Ready;
}

FrameStatus VulkanContext::EndFrame(const FrameContext& frame)
{
    vkCmdEndRenderPass(frame.command_buffer);
    if (vkEndCommandBuffer(frame.command_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to end the command buffer.");
    }

    FrameSync& sync = (*sync_pool_)[current_frame_];

    const VkSemaphore wait_semaphores[] = {sync.image_available};
    const VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    const VkSemaphore signal_semaphores[] = {sync.render_finished};

    VkSubmitInfo submit_info {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &frame.command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(device_->GraphicsQueue(), 1, &submit_info, sync.in_flight) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit the command buffer.");
    }

    VkPresentInfoKHR present_info {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    VkSwapchainKHR swapchains[] = {swapchain_->Handle()};
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &frame.image_index;

    VkResult result = vkQueuePresentKHR(device_->PresentQueue(), &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        swapchain_dirty_ = true;
    }

    current_frame_ = (current_frame_ + 1) % static_cast<uint32_t>(sync_pool_->Size());
    return swapchain_dirty_ ? FrameStatus::SwapchainOutOfDate : FrameStatus::Ready;
}

void VulkanContext::SubmitImmediate(const std::function<void(VkCommandBuffer)>& record) const
{
    VkCommandBufferAllocateInfo alloc_info {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool_;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    vkAllocateCommandBuffers(device_->Logical(), &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(command_buffer, &begin_info);

    record(command_buffer);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(device_->GraphicsQueue(), 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(device_->GraphicsQueue());

    vkFreeCommandBuffers(device_->Logical(), command_pool_, 1, &command_buffer);
}
}  // namespace ZKT
