#include "ZokataEngine/systems/scene/components/TransformComponent.h"

namespace ZKT
{
namespace ENGINE
{
TransformComponent::TransformComponent() = default;

void TransformComponent::OnEnable() {}

void TransformComponent::OnDisable() {}

void TransformComponent::Start() {}

void TransformComponent::Update(float /*delta_seconds*/) {}

void TransformComponent::FixedUpdate(float /*fixed_seconds*/) {}

const MATH::Vec3f& TransformComponent::GetPosition() const
{
    return position_;
}

const MATH::Vec3f& TransformComponent::GetScale() const
{
    return scale_;
}

void TransformComponent::SetScale(const MATH::Vec3f& s)
{
    scale_ = s;
    MarkDirty();
}

void TransformComponent::Scale(const MATH::Vec3f& multiplier)
{
    scale_ = MATH::Vec3f{
        scale_.x * multiplier.x,
        scale_.y * multiplier.y,
        scale_.z * multiplier.z,
    };
    MarkDirty();
}

const MATH::Quaternion& TransformComponent::GetQuaternion() const
{
    return rotation_;
}

int64_t TransformComponent::Parent() const
{
    return parent_id_;
}

const std::vector<int64_t>& TransformComponent::Children() const
{
    return children_ids_;
}

const MATH::Vec3f& TransformComponent::WorldPosition() const
{
    return world_position_;
}

const MATH::Vec3f& TransformComponent::WorldScale() const
{
    return world_scale_;
}

const MATH::Quaternion& TransformComponent::WorldRotation() const
{
    return world_rotation_;
}

const MATH::Mat4f& TransformComponent::GetModelMatrix() const
{
    if (model_dirty_)
    {
        model_matrix_ = MATH::Mat4f::FromTRS(world_position_, world_rotation_, world_scale_);
        model_dirty_ = false;
    }
    return model_matrix_;
}

void TransformComponent::MarkDirty()
{
    dirty_ = true;
    model_dirty_ = true;
    // Note: in a full ECS we would propagate to children; left as TODO.
}

void TransformComponent::SetPosition(const MATH::Vec3f& pos)
{
    position_ = pos;
    MarkDirty();
}

void TransformComponent::Translate(const MATH::Vec3f& delta)
{
    position_ += delta;
    MarkDirty();
}

MATH::Vec3f TransformComponent::EulerDegrees() const
{
    return rotation_.ToEulerDegrees();
}

void TransformComponent::SetEulerDegrees(const MATH::Vec3f& euler)
{
    rotation_ = MATH::Quaternion::FromEulerDegrees(euler);
    MarkDirty();
}

void TransformComponent::Rotate(const MATH::Vec3f& euler_degrees)
{
    rotation_ = MATH::Quaternion::FromEulerDegrees(euler_degrees) * rotation_;
    rotation_.Normalize();
    MarkDirty();
}

void TransformComponent::SetQuaternion(float w, float x, float y, float z)
{
    rotation_.Set(w, x, y, z);
    MarkDirty();
}

void TransformComponent::RotateAroundAxisDegrees(const MATH::Vec3f& axis, float angle_degrees)
{
    rotation_ = MATH::Quaternion::FromAxisAngle(axis, angle_degrees) * rotation_;
    rotation_.Normalize();
    MarkDirty();
}

void TransformComponent::SetParent(int64_t parent_id)
{
    parent_id_ = parent_id;
    MarkDirty();
}

void TransformComponent::AddChild(int64_t child_id)
{
    if (std::find(children_ids_.begin(), children_ids_.end(), child_id) == children_ids_.end())
    {
        children_ids_.push_back(child_id);
    }
}

void TransformComponent::RemoveChild(int64_t child_id)
{
    children_ids_.erase(std::remove(children_ids_.begin(), children_ids_.end(), child_id), children_ids_.end());
}

void TransformComponent::UpdateWorld(const TransformComponent* parent)
{
    if (parent == nullptr)
    {
        world_position_ = position_;
        world_scale_ = scale_;
        world_rotation_ = rotation_;
    }
    else
    {
        // Scale is component-wise.
        world_scale_ = MATH::Vec3f{
            parent->world_scale_.x * scale_.x,
            parent->world_scale_.y * scale_.y,
            parent->world_scale_.z * scale_.z,
        };

        world_rotation_ = parent->world_rotation_ * rotation_;

        MATH::Vec3f scaled_local {
            parent->world_scale_.x * position_.x,
            parent->world_scale_.y * position_.y,
            parent->world_scale_.z * position_.z,
        };
        const MATH::Vec3f rotated = parent->world_rotation_.Rotate(scaled_local);
        world_position_ = parent->world_position_ + rotated;
    }

    dirty_ = false;
    model_dirty_ = true;
}
}  // namespace ENGINE
}  // namespace ZKT
