#include "ZokataEngine/systems/scene/Entity.h"

#include <utility>

namespace ZKT
{
namespace ENGINE
{
Entity::Entity(int64_t id, std::string name)
    : id_(id)
    , name_(std::move(name))
{
    EnsureTransform();
}

Entity::~Entity() = default;

int64_t Entity::Id() const
{
    return id_;
}

const std::string& Entity::Name() const
{
    return name_;
}

Entity* Entity::Parent() const
{
    return parent_;
}

const std::vector<std::unique_ptr<Entity>>& Entity::Children() const
{
    return children_;
}

std::vector<std::unique_ptr<Entity>>& Entity::Children()
{
    return children_;
}

void Entity::SetParent(Entity* parent)
{
    parent_ = parent;
}

TransformComponent& Entity::Transform()
{
    EnsureTransform();
    return *transform_;
}

const TransformComponent& Entity::Transform() const
{
    return *transform_;
}

void Entity::AddComponent(std::unique_ptr<Component> comp)
{
    if (comp)
    {
        comp->SetOwner(this);
        if (auto* as_transform = dynamic_cast<TransformComponent*>(comp.get()))
        {
            transform_ = as_transform;
        }
        components_.push_back(std::move(comp));
    }
}

Entity* Entity::AddChild(std::unique_ptr<Entity> child)
{
    child->SetParent(this);
    Entity* raw_ptr = child.get();
    children_.push_back(std::move(child));
    return raw_ptr;
}

void Entity::EnsureTransform()
{
    if (transform_ == nullptr)
    {
        transform_ = &AddComponent<TransformComponent>();
    }
}

void Entity::OnEnableRecursive()
{
    OnEnableSelf();
    for (auto& child : children_)
    {
        child->OnEnableRecursive();
    }
}

void Entity::StartRecursive()
{
    StartSelf();
    for (auto& child : children_)
    {
        child->StartRecursive();
    }
}

void Entity::UpdateRecursive(float delta_seconds)
{
    UpdateSelf(delta_seconds);
    for (auto& child : children_)
    {
        child->UpdateRecursive(delta_seconds);
    }
}

void Entity::FixedUpdateRecursive(float fixed_seconds)
{
    FixedUpdateSelf(fixed_seconds);
    for (auto& child : children_)
    {
        child->FixedUpdateRecursive(fixed_seconds);
    }
}

void Entity::OnEnableSelf()
{
    OnEnableEntity();
    for (auto& comp : components_)
    {
        if (comp->Enabled())
        {
            comp->OnEnable();
        }
    }
}

void Entity::StartSelf()
{
    StartEntity();
    for (auto& comp : components_)
    {
        if (comp->Enabled())
        {
            comp->Start();
        }
    }
}

void Entity::UpdateSelf(float delta_seconds)
{
    UpdateEntity(delta_seconds);
    for (auto& comp : components_)
    {
        if (comp->Enabled())
        {
            comp->Update(delta_seconds);
        }
    }
}

void Entity::FixedUpdateSelf(float fixed_seconds)
{
    FixedUpdateEntity(fixed_seconds);
    for (auto& comp : components_)
    {
        if (comp->Enabled())
        {
            comp->FixedUpdate(fixed_seconds);
        }
    }
}
}  // namespace ENGINE
}  // namespace ZKT
