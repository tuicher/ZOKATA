#pragma once

#include "ZokataEngine/systems/scene/Component.h"
#include "ZokataMath/Matrix.h"
#include "ZokataMath/Quaternion.h"
#include "ZokataMath/Vector.h"

namespace ZKT
{
namespace ENGINE
{
enum class ProjectionType
{
    Perspective,
    Orthographic
};

/**
 * @brief Camera component with cached view/projection data.
 */
class CameraComponent : public Component
{
public:
    CameraComponent() = default;

    void OnEnable() override;
    void OnDisable() override;
    void Start() override;
    void Update(float delta_seconds) override;
    void FixedUpdate(float fixed_seconds) override;

    ProjectionType Projection() const;
    void SetProjection(ProjectionType type);

    float VerticalFovDegrees() const;
    void SetVerticalFovDegrees(float degrees);

    float AspectRatio() const;
    void SetAspectRatio(float ratio);

    float NearPlane() const;
    void SetNearPlane(float near_plane);

    float FarPlane() const;
    void SetFarPlane(float far_plane);

    float OrthoHeight() const;
    void SetOrthoHeight(float height);

    const MATH::Vec4f& ClearColor() const;
    void SetClearColor(const MATH::Vec4f& color);

    float Exposure() const;
    void SetExposure(float exposure);

    const MATH::Mat4f& GetViewMatrix() const;
    const MATH::Mat4f& GetProjectionMatrix() const;
    const MATH::Mat4f& GetViewProjectionMatrix() const;
    MATH::Mat4f GetModelViewProjectionMatrix(const MATH::Mat4f& model) const;

private:
    void MarkViewDirty() const;
    void MarkProjectionDirty() const;
    void SyncFromTransform() const;

    ProjectionType projection_type_ = ProjectionType::Perspective;
    float vertical_fov_degrees_ = 60.0F;
    float aspect_ratio_ = 16.0F / 9.0F;
    float near_plane_ = 0.1F;
    float far_plane_ = 1000.0F;
    float ortho_height_ = 10.0F;

    MATH::Vec4f clear_color_ {0.0F, 0.0F, 0.0F, 1.0F};
    float exposure_ = 1.0F;

    mutable MATH::Vec3f cached_position_ {0.0F, 0.0F, 0.0F};
    mutable MATH::Quaternion cached_rotation_ {};

    mutable MATH::Mat4f view_matrix_ {};
    mutable MATH::Mat4f projection_matrix_ {};
    mutable MATH::Mat4f view_projection_matrix_ {};
    mutable bool view_dirty_ = true;
    mutable bool projection_dirty_ = true;
    mutable bool view_projection_dirty_ = true;
};
}  // namespace ENGINE
}  // namespace ZKT
