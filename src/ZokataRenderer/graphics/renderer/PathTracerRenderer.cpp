#include "ZokataRenderer/graphics/renderer/PathTracerRenderer.h"

#include <algorithm>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace ZKT
{
const char* PathTracerRenderer::Name() const
{
    return "Path Tracer (HW RT)";
}

RendererType PathTracerRenderer::Type() const
{
    return RendererType::PathTracer;
}

void PathTracerRenderer::RenderGui(const FrameDescriptor& frame)
{
    ImGui::Begin("ZOKATA Renderer");
    ImGui::TextUnformatted("Render backend");
    ImGui::Separator();
    ImGui::Text("Frame: %llu", static_cast<unsigned long long>(frame.frame_index));
    ImGui::Text("Delta: %.3f ms", frame.delta_seconds * 1000.0F);
    ImGui::Text("Renderer: %s", Name());
    ImGui::Spacing();

    ImGui::SliderInt("Samples per pixel", &samples_per_pixel_, 1, 8192);
    ImGui::SliderInt("Max bounces", &max_bounces_, 1, 256);

    float sky_color[3] = {sky_color_.r, sky_color_.g, sky_color_.b};
    if (ImGui::ColorEdit3("Sky color", sky_color))
    {
        sky_color_ = glm::make_vec3(sky_color);
    }

    ImGui::Spacing();
    ImGui::TextWrapped(
        "This is the Vulkan RT path tracer skeleton. "
        "The UI is ready to extend with buffers, pipelines, and TLAS/BLAS.");
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
        ImGui::Text("FPS: %.1f", 1.0F / std::max(frame.delta_seconds, 0.0001F));
        ImGui::Text("Samples: %d", samples_per_pixel_);
        ImGui::Text("Bounces: %d", max_bounces_);
    }
    ImGui::End();
}
}  // namespace ZKT
