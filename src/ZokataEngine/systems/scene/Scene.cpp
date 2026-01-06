#include "ZokataEngine/systems/scene/Scene.h"

#include <utility>

namespace ZKT
{
namespace ENGINE
{
Scene::Scene(std::string name)
    : name_(std::move(name))
{
}

const std::string& Scene::Name() const
{
    return name_;
}

const std::vector<SceneEntity>& Scene::Entities() const
{
    return entities_;
}

void Scene::AddEntity(SceneEntity entity)
{
    entities_.push_back(std::move(entity));
}

const std::vector<std::unique_ptr<Entity>>& Scene::RuntimeRoots() const
{
    return roots_;
}

const std::vector<Entity*>& Scene::AllRuntimeEntities() const
{
    return all_entities_;
}

Entity& Scene::CreateRuntimeEntity(int64_t id, const std::string& name, Entity* parent)
{
    auto entity = std::make_unique<Entity>(id, name);
    Entity& ref = *entity;
    RegisterEntity(ref);
    if (parent != nullptr)
    {
        parent->AddChild(std::move(entity));
    }
    else
    {
        roots_.push_back(std::move(entity));
    }
    return ref;
}

void Scene::AddRoot(std::unique_ptr<Entity> entity)
{
    if (entity)
    {
        RegisterEntity(*entity);
        roots_.push_back(std::move(entity));
    }
}

void Scene::OnEnable()
{
    ForEachEntityPreorder([](Entity& entity) { entity.OnEnableSelf(); });
}

void Scene::Start()
{
    ForEachEntityPreorder([](Entity& entity) { entity.StartSelf(); });
}

void Scene::Update(float delta_seconds)
{
    ForEachEntityPreorder([delta_seconds](Entity& entity) { entity.UpdateSelf(delta_seconds); });
}

void Scene::FixedUpdate(float fixed_seconds)
{
    ForEachEntityPreorder([fixed_seconds](Entity& entity) { entity.FixedUpdateSelf(fixed_seconds); });
}

void Scene::RegisterEntity(Entity& entity)
{
    all_entities_.push_back(&entity);
}

void Scene::ForEachEntityPreorder(const std::function<void(Entity&)>& fn)
{
    for (auto& root : roots_)
    {
        TraversePreorder(*root, fn);
    }
}

void Scene::TraversePreorder(Entity& entity, const std::function<void(Entity&)>& fn)
{
    fn(entity);
    for (auto& child : entity.Children())
    {
        TraversePreorder(*child, fn);
    }
}
}  // namespace ENGINE
}  // namespace ZKT
