#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ZokataEngine/systems/scene/Entity.h"

namespace ZKT
{
namespace ENGINE
{
struct SceneComponentSummary
{
    std::string name;
};

struct SceneEntity
{
    int64_t id {-1};
    std::string name;
    std::vector<SceneComponentSummary> components;
};

// A runtime scene owns entities and drives their lifecycle.
/**
 * @brief Container of entities that manages hierarchy and scene lifecycle.
 */
class Scene
{
public:
    explicit Scene(std::string name = "");

    const std::string& Name() const;

    // Parsed/static view of the scene (from YAML).
    /**
     * @brief Returns the static entities parsed from scene data.
     */
    const std::vector<SceneEntity>& Entities() const;
    /**
     * @brief Adds a parsed/static entity entry.
     */
    void AddEntity(SceneEntity entity);

    // Runtime entity management.
    /**
     * @brief Creates a runtime entity (optionally parented) and registers it.
     */
    Entity& CreateRuntimeEntity(int64_t id, const std::string& name = "", Entity* parent = nullptr);
    /**
     * @brief Adds a root-level runtime entity already created by caller.
     */
    void AddRoot(std::unique_ptr<Entity> entity);
    const std::vector<std::unique_ptr<Entity>>& RuntimeRoots() const;
    const std::vector<Entity*>& AllRuntimeEntities() const;

    // Lifecycle orchestration (pre-order traversal).
    /**
     * @brief Propagates OnEnable through all runtime entities.
     */
    void OnEnable();
    /**
     * @brief Propagates Start through all runtime entities.
     */
    void Start();
    /**
     * @brief Propagates Update through all runtime entities.
     */
    void Update(float delta_seconds);
    /**
     * @brief Propagates FixedUpdate through all runtime entities.
     */
    void FixedUpdate(float fixed_seconds);

private:
    std::string name_;
    std::vector<SceneEntity> entities_;

    std::vector<std::unique_ptr<Entity>> roots_;
    std::vector<Entity*> all_entities_;

    void RegisterEntity(Entity& entity);
    void ForEachEntityPreorder(const std::function<void(Entity&)>& fn);
    static void TraversePreorder(Entity& entity, const std::function<void(Entity&)>& fn);
};
}  // namespace ENGINE
}  // namespace ZKT
