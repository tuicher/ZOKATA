#include "ZokataEngine/Engine.h"

#include <filesystem>
#include <sstream>
#include <string>
#include <cinttypes>

#include <imgui.h>

#include "ZokataLog/Log.h"
#include "ZokataRenderer/Application.h"
#include "ZokataEngine/systems/scene/SceneManager.h"
#include "ZokataEngine/systems/scene/Scene.h"
#include "ZokataEngine/systems/scene/components/TransformComponent.h"

namespace fs = std::filesystem;

namespace ZKT
{
namespace ENGINE
{
namespace
{
constexpr const char* kScenesFolder = "scenes";

std::filesystem::path FindScenesRoot()
{
    auto path = fs::current_path();
    for (int i = 0; i < 6; ++i)
    {
        const auto candidate = path / kScenesFolder;
        if (fs::exists(candidate) && fs::is_directory(candidate))
        {
            return candidate;
        }

        if (!path.has_parent_path())
        {
            break;
        }
        path = path.parent_path();
    }
    return {};
}

}  // namespace

Engine::Engine()
    : scenes_root_(FindScenesRoot())
{
}

void Engine::Run()
{
    // Let SceneManager discover YAML scenes and load them.
    scene_manager_.DiscoverAndLoadScenes(scenes_root_);

    // Provide a GUI callback to render scene hierarchy.
    ZKT::Application app;
    app.SetGuiCallback([this]() { DrawSceneHierarchyGui(); });

    // Kick lifecycle for active scene before entering the main app loop.
    scene_manager_.OnEnableActive();
    scene_manager_.StartActive();

    app.Run();
}

void Engine::DrawSceneHierarchyGui()
{
    Scene* scene = scene_manager_.ActiveScene();
    if (scene == nullptr)
    {
        return;
    }

    if (!ImGui::Begin("Scene Hierarchy"))
    {
        ImGui::End();
        return;
    }

    for (auto& root : scene->RuntimeRoots())
    {
        DrawEntityNodeGui(*root);
    }

    ImGui::End();
}

void Engine::DrawEntityNodeGui(Entity& entity)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    // Keep a stable tree id using the entity pointer and display both name and id.
    const bool opened = ImGui::TreeNodeEx(
        (void*)&entity,
        flags,
        "%s [id %" PRId64 "]",
        entity.Name().c_str(),
        static_cast<int64_t>(entity.Id()));

    if (opened)
    {
        ImGui::Text("Name: %s", entity.Name().c_str());
        ImGui::Text("Id: %" PRId64, static_cast<int64_t>(entity.Id()));

        ImGui::TextUnformatted("Components:");
        // Editable Transform component, if present.
        if (auto* tc = entity.GetComponent<TransformComponent>())
        {
            ImGui::Separator();
            if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_None))
            {
                ImGui::PushID(&entity);
                MATH::Vec3f pos = tc->GetPosition();
                MATH::Vec3f rot = tc->EulerDegrees();
                MATH::Vec3f scl = tc->GetScale();

                if (ImGui::DragFloat3("Position", &pos.x, 0.01F))
                {
                    tc->SetPosition(pos);
                }
                if (ImGui::DragFloat3("Rotation", &rot.x, 0.5F))
                {
                    tc->SetEulerDegrees(rot);
                }
                if (ImGui::DragFloat3("Scale", &scl.x, 0.01F))
                {
                    tc->SetScale(scl);
                }
                ImGui::PopID();
                ImGui::TreePop();
            }
        }

        ImGui::Separator();
        ImGui::TextUnformatted("Children:");
        ImGui::Indent();
        for (auto& child : entity.Children())
        {
            DrawEntityNodeGui(*child);
        }
        ImGui::Unindent();
        ImGui::TreePop();
    }
}
}  // namespace ENGINE
}  // namespace ZKT
