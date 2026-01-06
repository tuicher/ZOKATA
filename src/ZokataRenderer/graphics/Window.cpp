#include "ZokataRenderer/graphics/Window.h"

#include <stdexcept>

#include <GLFW/glfw3.h>

namespace ZKT
{
namespace
{
class GlfwLifetime
{
public:
    GlfwLifetime()
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW.");
        }

        if (!glfwVulkanSupported())
        {
            throw std::runtime_error("GLFW installation does not detect Vulkan support.");
        }
    }

    GlfwLifetime(const GlfwLifetime&) = delete;
    GlfwLifetime& operator=(const GlfwLifetime&) = delete;

    ~GlfwLifetime() { glfwTerminate(); }
};
}  // namespace

GlfwWindow::GlfwWindow(const WindowConfig& config)
{
    static GlfwLifetime glfw_runtime;
    (void)glfw_runtime;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window_ = glfwCreateWindow(static_cast<int>(config.width),
                               static_cast<int>(config.height),
                               config.title.c_str(),
                               nullptr,
                               nullptr);

    if (!window_)
    {
        throw std::runtime_error("Failed to create a GLFW window.");
    }

    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(
        window_,
        [](GLFWwindow* window, int /*width*/, int /*height*/) {
            auto* self = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
            if (self)
            {
                self->framebuffer_resized_ = true;
            }
        });
}

GlfwWindow::~GlfwWindow()
{
    if (window_)
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
}

void GlfwWindow::PollEvents()
{
    glfwPollEvents();
}

bool GlfwWindow::ShouldClose() const
{
    return window_ ? glfwWindowShouldClose(window_) != 0 : true;
}

VkSurfaceKHR GlfwWindow::CreateSurface(VkInstance instance) const
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (glfwCreateWindowSurface(instance, window_, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create VkSurfaceKHR from the GLFW window.");
    }

    return surface;
}

VkExtent2D GlfwWindow::GetFramebufferExtent() const
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    return {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
    };
}

bool GlfwWindow::WasResized() const
{
    return framebuffer_resized_;
}

void GlfwWindow::ResetResizedFlag()
{
    framebuffer_resized_ = false;
}

GLFWwindow* GlfwWindow::GetNativeHandle() const
{
    return window_;
}
}  // namespace ZKT
