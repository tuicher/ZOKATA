#pragma once

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace ZKT
{
struct ImGuiInitInfo
{
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t graphics_queue_family = 0;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkRenderPass render_pass = VK_NULL_HANDLE;
    uint32_t image_count = 0;
    uint32_t min_image_count = 2;
    uint32_t api_version = VK_API_VERSION_1_2;
    GLFWwindow* window = nullptr;
};

/**
 * @brief Encapsulates ImGui integration with Vulkan/GLFW (backend + descriptor pool).
 */
class ImGuiLayer
{
public:
    ImGuiLayer() = default;
    ~ImGuiLayer();

    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;

    /**
     * @brief Initializes ImGui backends and descriptor pool.
     */
    void Initialize(const ImGuiInitInfo& info);
    /**
     * @brief Recreates backend state when swapchain changes.
     */
    void Recreate(const ImGuiInitInfo& info);
    /**
     * @brief Begins a new ImGui frame.
     */
    void NewFrame();
    /**
     * @brief Renders ImGui draw data into the provided command buffer.
     */
    void Render(VkCommandBuffer command_buffer);
    /**
     * @brief Shuts down ImGui backends and frees resources.
     */
    void Shutdown();

private:
    VkDevice device_ = VK_NULL_HANDLE;
    VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
    uint32_t min_image_count_ = 2;
    bool backend_initialized_ = false;
    bool context_initialized_ = false;
    bool glfw_initialized_ = false;

    void CreateDescriptorPool(VkDevice device);
    void SetupBackend(const ImGuiInitInfo& info);
};
}  // namespace ZKT
