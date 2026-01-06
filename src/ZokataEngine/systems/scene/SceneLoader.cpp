#include "ZokataEngine/systems/scene/SceneLoader.h"

#include <sstream>
#include <stdexcept>
#include <string>

#include "ZokataEngine/systems/scene/components/TransformComponent.h"
#include "ZokataMath/Vector.h"

namespace ZKT
{
namespace ENGINE
{
namespace
{
float SafeFloat(const YAML::Node& node, float fallback = 0.0F)
{
    return node ? node.as<float>(fallback) : fallback;
}

std::string SafeString(const YAML::Node& node, const std::string& fallback = {})
{
    return node ? node.as<std::string>(fallback) : fallback;
}

MATH::Vec3f ReadVec3(const YAML::Node& node, const MATH::Vec3f& fallback)
{
    if (!(node && node.IsMap()))
    {
        return fallback;
    }
    return MATH::Vec3f{
        SafeFloat(node["x"], fallback.x),
        SafeFloat(node["y"], fallback.y),
        SafeFloat(node["z"], fallback.z),
    };
}
}  // namespace

std::unique_ptr<Scene> SceneLoader::LoadFromFile(const std::filesystem::path& path)
{
    YAML::Node root = YAML::LoadFile(path.string());

    const auto version = root["version"].as<int>(1);
    if (version != 1)
    {
        throw std::runtime_error("Unsupported scene version: " + std::to_string(version));
    }

    const YAML::Node scene_node = root["scene"];
    if (!scene_node)
    {
        throw std::runtime_error("Missing 'scene' node");
    }

    const std::string scene_name = SafeString(scene_node["name"], path.stem().string());
    auto scene = std::make_unique<Scene>(scene_name);

    ParseEntities(scene_node["entities"], *scene);

    // TODO: Parse assets (meshes, materials, etc.) and register them in the asset system.
    // TODO: Push parsed scene to renderer/engine state.

    return scene;
}

void SceneLoader::ParseEntities(const YAML::Node& entities_node, Scene& scene, Entity* parent)
{
    if (!(entities_node && entities_node.IsSequence()))
    {
        return;
    }

    for (const auto& entity_node : entities_node)
    {
        SceneEntity entity {};
        entity.id = entity_node["id"].as<int64_t>(-1);
        entity.name = SafeString(entity_node["name"], "Entity");

        const auto components = entity_node["components"];
        if (components && components.IsMap())
        {
            for (const auto& kv : components)
            {
                SceneComponentSummary comp {};
                comp.name = kv.first.as<std::string>();
                entity.components.push_back(std::move(comp));
            }
        }

        Entity& runtime = scene.CreateRuntimeEntity(entity.id, entity.name, parent);

        if (components && components.IsMap())
        {
            if (const auto transform = components["Transform"])
            {
                const MATH::Vec3f position = ReadVec3(transform["position"], {0.0F, 0.0F, 0.0F});
                const MATH::Vec3f rotation = ReadVec3(transform["rotation"], {0.0F, 0.0F, 0.0F});
                const MATH::Vec3f scale = ReadVec3(transform["scale"], {1.0F, 1.0F, 1.0F});

                TransformComponent* tc = runtime.GetComponent<TransformComponent>();
                if (tc == nullptr)
                {
                    tc = &runtime.AddComponent<TransformComponent>();
                }
                tc->SetPosition(position);
                tc->SetEulerDegrees(rotation);
                tc->SetScale(scale);
            }
        }

        scene.AddEntity(std::move(entity));

        // Recurse into children if any.
        if (const auto children = entity_node["children"]; children && children.IsSequence())
        {
            ParseEntities(children, scene, &runtime);
        }
    }
}
}  // namespace ENGINE
}  // namespace ZKT
