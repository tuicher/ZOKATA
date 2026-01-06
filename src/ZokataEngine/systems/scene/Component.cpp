#include "ZokataEngine/systems/scene/Component.h"

namespace ZKT
{
namespace ENGINE
{
void Component::OnEnable() {}

void Component::OnDisable() {}

void Component::Start() {}

void Component::Update(float /*delta_seconds*/) {}

void Component::FixedUpdate(float /*fixed_seconds*/) {}

void Component::SetOwner(Entity* owner)
{
    owner_ = owner;
}

Entity* Component::Owner()
{
    return owner_;
}

const Entity* Component::Owner() const
{
    return owner_;
}

void Component::SetEnabled(bool enabled)
{
    enabled_ = enabled;
}

bool Component::Enabled() const
{
    return enabled_;
}
}  // namespace ENGINE
}  // namespace ZKT
