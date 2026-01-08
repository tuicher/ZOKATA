#include "ZokataEngine/systems/scene/components/CameraComponent.h"

#include <algorithm>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>

#include "ZokataEngine/systems/scene/Entity.h"

namespace ZKT
{
namespace ENGINE
{
void CameraComponent::OnEnable() {}

void CameraComponent::OnDisable() {}

void CameraComponent::Start()
{
    SyncFromTransform();
}

void CameraComponent::Update(float /*delta_seconds*/)
{
    SyncFromTransform();
    GetViewProjectionMatrix();
}

void CameraComponent::FixedUpdate(float /*fixed_seconds*/) {}

ProjectionType CameraComponent::Projection() const
{
    return projection_type_;
}

void CameraComponent::SetProjection(ProjectionType type)
{
    if (projection_type_ == type)
    {
        return;
    }
    projection_type_ = type;
    MarkProjectionDirty();
}

float CameraComponent::VerticalFovDegrees() const
{
    return vertical_fov_degrees_;
}

void CameraComponent::SetVerticalFovDegrees(float degrees)
{
    vertical_fov_degrees_ = degrees;
    MarkProjectionDirty();
}

float CameraComponent::AspectRatio() const
{
    return aspect_ratio_;
}

void CameraComponent::SetAspectRatio(float ratio)
{
    aspect_ratio_ = ratio;
    MarkProjectionDirty();
}

float CameraComponent::NearPlane() const
{
    return near_plane_;
}

void CameraComponent::SetNearPlane(float near_plane)
{
    near_plane_ = near_plane;
    MarkProjectionDirty();
}

float CameraComponent::FarPlane() const
{
    return far_plane_;
}

void CameraComponent::SetFarPlane(float far_plane)
{
    far_plane_ = far_plane;
    MarkProjectionDirty();
}

float CameraComponent::OrthoHeight() const
{
    return ortho_height_;
}

void CameraComponent::SetOrthoHeight(float height)
{
    ortho_height_ = height;
    MarkProjectionDirty();
}

const MATH::Vec4f& CameraComponent::ClearColor() const
{
    return clear_color_;
}

void CameraComponent::SetClearColor(const MATH::Vec4f& color)
{
    clear_color_ = color;
}

float CameraComponent::Exposure() const
{
    return exposure_;
}

void CameraComponent::SetExposure(float exposure)
{
    exposure_ = exposure;
}

const MATH::Mat4f& CameraComponent::GetViewMatrix() const
{
    SyncFromTransform();
    if (view_dirty_)
    {
        glm::mat4 transform =
            glm::translate(glm::mat4(1.0F), cached_position_.ToGlm()) *
            glm::mat4_cast(cached_rotation_.ToGlm());
        view_matrix_ = MATH::Mat4f(glm::inverse(transform));
        view_dirty_ = false;
    }
    return view_matrix_;
}

const MATH::Mat4f& CameraComponent::GetProjectionMatrix() const
{
    if (projection_dirty_)
    {
        const float safe_aspect = std::max(0.0001F, aspect_ratio_);
        const float safe_near = std::max(0.0001F, near_plane_);
        const float safe_far = std::max(safe_near + 0.0001F, far_plane_);

        glm::mat4 projection {};
        if (projection_type_ == ProjectionType::Perspective)
        {
            projection = glm::perspective(glm::radians(vertical_fov_degrees_), safe_aspect, safe_near, safe_far);
        }
        else
        {
            const float safe_height = std::max(0.0001F, ortho_height_);
            const float half_height = safe_height * 0.5F;
            const float half_width = half_height * safe_aspect;
            projection = glm::ortho(-half_width, half_width, -half_height, half_height, safe_near, safe_far);
        }

        projection_matrix_ = MATH::Mat4f(projection);
        projection_dirty_ = false;
    }
    return projection_matrix_;
}

const MATH::Mat4f& CameraComponent::GetViewProjectionMatrix() const
{
    const MATH::Mat4f& view = GetViewMatrix();
    const MATH::Mat4f& projection = GetProjectionMatrix();

    if (view_projection_dirty_)
    {
        view_projection_matrix_ = MATH::Mat4f(projection.ToGlm() * view.ToGlm());
        view_projection_dirty_ = false;
    }
    return view_projection_matrix_;
}

MATH::Mat4f CameraComponent::GetModelViewProjectionMatrix(const MATH::Mat4f& model) const
{
    const MATH::Mat4f& view_projection = GetViewProjectionMatrix();
    return MATH::Mat4f(view_projection.ToGlm() * model.ToGlm());
}

void CameraComponent::MarkViewDirty() const
{
    view_dirty_ = true;
    view_projection_dirty_ = true;
}

void CameraComponent::MarkProjectionDirty() const
{
    projection_dirty_ = true;
    view_projection_dirty_ = true;
}

void CameraComponent::SyncFromTransform() const
{
    const Entity* owner = Owner();
    if (owner == nullptr)
    {
        return;
    }

    const TransformComponent& transform = owner->Transform();
    const MATH::Vec3f position = transform.WorldPosition();
    const MATH::Quaternion rotation = transform.WorldRotation();

    if (position != cached_position_ || rotation != cached_rotation_)
    {
        cached_position_ = position;
        cached_rotation_ = rotation;
        MarkViewDirty();
    }
}
}  // namespace ENGINE
}  // namespace ZKT
