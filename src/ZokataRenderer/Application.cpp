#include "ZokataRenderer/Application.h"

#include <chrono>
#include <stdexcept>
#include <utility>

#include <imgui.h>

namespace ZKT
{
namespace
{
ImGuiInitInfo MakeImGuiInit(const GlfwWindow& window, const VulkanContext& context)
{
    ImGuiInitInfo info {};
    info.instance = context.InstanceHandle();
    info.physical_device = context.PhysicalDevice();
    info.device = context.DeviceHandle();
    info.graphics_queue_family = context.GraphicsQueueFamily();
    info.graphics_queue = context.GraphicsQueue();
    info.render_pass = context.RenderPass();
    info.image_count = context.SwapchainImageCount();
    info.min_image_count = context.MinImageCount();
    info.api_version = VK_API_VERSION_1_2;
    info.window = window.GetNativeHandle();
    return info;
}
}  // namespace

Application::Application()
    : window_(WindowConfig{1280, 720, "ZOKATA"})
    , context_(window_)
{
    renderer_ = std::make_unique<DeferredRenderer>();
    SetupImGui();
    ActiveRenderer().OnSwapchainUpdated(context_.SwapchainExtent());
}

Application::~Application()
{
    context_.WaitIdle();
    imgui_layer_.Shutdown();
}

void Application::SetupImGui()
{
    imgui_layer_.Initialize(MakeImGuiInit(window_, context_));
}

void Application::SetGuiCallback(std::function<void()> callback)
{
    extra_gui_ = std::move(callback);
}

void Application::RecreateSwapchainAndUi()
{
    context_.RecreateSwapchain();
    ActiveRenderer().OnSwapchainUpdated(context_.SwapchainExtent());
    imgui_layer_.Recreate(MakeImGuiInit(window_, context_));
    window_.ResetResizedFlag();
}

IRenderer& Application::ActiveRenderer()
{
    return *renderer_;
}

void Application::Run()
{
    using clock = std::chrono::steady_clock;
    auto last_frame = clock::now();

    while (!window_.ShouldClose())
    {
        window_.PollEvents();

        FrameContext frame {};
        FrameStatus status = context_.BeginFrame(frame);
        if (status == FrameStatus::SwapchainOutOfDate)
        {
            RecreateSwapchainAndUi();
            continue;
        }

        const auto now = clock::now();
        const float delta_seconds = std::chrono::duration<float>(now - last_frame).count();
        last_frame = now;

        imgui_layer_.NewFrame();

        FrameDescriptor gui_frame {
            .delta_seconds = delta_seconds,
            .frame_index = frame_index_++,
        };

        if (extra_gui_)
        {
            extra_gui_();
        }

        ActiveRenderer().RenderGui(gui_frame);

        imgui_layer_.Render(frame.command_buffer);

        status = context_.EndFrame(frame);
        if (status == FrameStatus::SwapchainOutOfDate || window_.WasResized())
        {
            RecreateSwapchainAndUi();
        }
    }

    context_.WaitIdle();
}
}  // namespace ZKT
