#include "ZokataRenderer/graphics/ImGuiLayer.h"

#include <array>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace ZKT
{
ImGuiLayer::~ImGuiLayer()
{
    Shutdown();
}

void ImGuiLayer::CreateDescriptorPool(VkDevice device)
{
    const VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
    };

    VkDescriptorPoolCreateInfo pool_info {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * static_cast<uint32_t>(std::size(pool_sizes));
    pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create ImGui descriptor pool.");
    }
}

void ImGuiLayer::SetupBackend(const ImGuiInitInfo& info)
{
    if (!info.window)
    {
        throw std::runtime_error("ImGui window handle is invalid.");
    }

    if (!glfw_initialized_)
    {
        ImGui_ImplGlfw_InitForVulkan(info.window, true);
        glfw_initialized_ = true;
    }

    ImGui_ImplVulkan_InitInfo init_info {};
    init_info.ApiVersion = info.api_version;
    init_info.Instance = info.instance;
    init_info.PhysicalDevice = info.physical_device;
    init_info.Device = info.device;
    init_info.QueueFamily = info.graphics_queue_family;
    init_info.Queue = info.graphics_queue;
    init_info.DescriptorPool = descriptor_pool_;
    init_info.RenderPass = info.render_pass;
    init_info.MinImageCount = info.min_image_count;
    init_info.ImageCount = info.image_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Subpass = 0;
    init_info.UseDynamicRendering = false;

    if (!ImGui_ImplVulkan_Init(&init_info))
    {
        throw std::runtime_error("ImGui_ImplVulkan_Init failed.");
    }

    if (!ImGui_ImplVulkan_CreateFontsTexture())
    {
        throw std::runtime_error("Failed to create ImGui font texture.");
    }

    backend_initialized_ = true;
    min_image_count_ = info.min_image_count;
}

void ImGuiLayer::Initialize(const ImGuiInitInfo& info)
{
    if (context_initialized_)
    {
        return;
    }

    device_ = info.device;
    CreateDescriptorPool(device_);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    SetupBackend(info);
    context_initialized_ = true;
}

void ImGuiLayer::Recreate(const ImGuiInitInfo& info)
{
    if (!context_initialized_)
    {
        Initialize(info);
        return;
    }

    ImGui_ImplVulkan_Shutdown();
    backend_initialized_ = false;
    SetupBackend(info);
}

void ImGuiLayer::NewFrame()
{
    if (!backend_initialized_)
    {
        return;
    }

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::Render(VkCommandBuffer command_buffer)
{
    if (!backend_initialized_)
    {
        return;
    }

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
}

void ImGuiLayer::Shutdown()
{
    if (!context_initialized_)
    {
        return;
    }

    ImGui_ImplVulkan_Shutdown();
    if (glfw_initialized_)
    {
        ImGui_ImplGlfw_Shutdown();
        glfw_initialized_ = false;
    }
    ImGui::DestroyContext();
    context_initialized_ = false;
    backend_initialized_ = false;

    if (device_ != VK_NULL_HANDLE && descriptor_pool_ != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
        descriptor_pool_ = VK_NULL_HANDLE;
    }
}
}  // namespace ZKT
