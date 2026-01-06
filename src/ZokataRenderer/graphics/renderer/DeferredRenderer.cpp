#include "ZokataRenderer/graphics/renderer/DeferredRenderer.h"

#include <algorithm>

#include <imgui.h>

namespace ZKT
{
const char* DeferredRenderer::Name() const
{
    return "Deferred Renderer";
}

RendererType DeferredRenderer::Type() const
{
    return RendererType::Deferred;
}

void DeferredRenderer::RenderGui(const FrameDescriptor& frame)
{
    ImGui::Begin("ZOKATA Renderer");
    ImGui::TextUnformatted("Render backend");
    ImGui::Separator();
    ImGui::Text("Frame: %llu", static_cast<unsigned long long>(frame.frame_index));
    ImGui::Text("Delta: %.3f ms", frame.delta_seconds * 1000.0F);
    ImGui::Text("Renderer: %s", Name());
    ImGui::Spacing();

    ImGui::SliderInt("MSAA samples", &samples_, 1, 8);
    ImGui::Checkbox("Recreate G-Buffer", &recreate_gbuffer_);

    ImGui::Spacing();
    ImGui::TextWrapped(
        "Deferred path placeholder: G-Buffer targets (albedo/normal/depth) "
        "and lighting/composition passes should be wired here.");
    ImGui::End();

    const ImGuiWindowFlags overlay_flags = ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav;

    const float PAD = 10.0F;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos = ImVec2(work_pos.x + work_size.x - PAD,
                               work_pos.y + PAD);
    ImVec2 pivot = ImVec2(1.0F, 0.0F);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, pivot);
    ImGui::SetNextWindowBgAlpha(0.35F);

    if (ImGui::Begin("Overlay", nullptr, overlay_flags))
    {
        ImGui::Text("FPS: %.1f", 1.0F / (std::max)(frame.delta_seconds, 0.0001F));
        ImGui::Text("MSAA: x%d", samples_);
        ImGui::Text("Swapchain: %ux%u", last_extent_.width, last_extent_.height);
    }
    ImGui::End();
}

void DeferredRenderer::OnSwapchainUpdated(VkExtent2D extent)
{
    last_extent_ = extent;
    recreate_gbuffer_ = true;
}
}  // namespace ZKT
