#pragma once

#include <filesystem>
#include <memory>

#include <yaml-cpp/yaml.h>

#include "ZokataEngine/systems/scene/Scene.h"

namespace ZKT
{
namespace ENGINE
{
class SceneLoader
{
public:
    /**
     * @brief Loads a scene from a YAML file and builds entities/components.
     */
    std::unique_ptr<Scene> LoadFromFile(const std::filesystem::path& path);

private:
    void ParseEntities(const YAML::Node& entities_node, Scene& scene, Entity* parent = nullptr);
};
}  // namespace ENGINE
}  // namespace ZKT
