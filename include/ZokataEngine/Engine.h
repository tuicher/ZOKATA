#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "ZokataEngine/systems/scene/SceneManager.h"

namespace ZKT
{
namespace ENGINE
{
class Engine
{
public:
    /**
     * @brief Entry point for the engine application: loads scenes and runs the renderer loop.
     */
    Engine();
    /**
     * @brief Discovers scenes, sets callbacks, and enters the main application loop.
     */
    void Run();

private:
    std::filesystem::path scenes_root_;
    SceneManager scene_manager_;

    void DrawSceneHierarchyGui();
    void DrawEntityNodeGui(Entity& entity);
};
}  // namespace ENGINE
}  // namespace ZKT
