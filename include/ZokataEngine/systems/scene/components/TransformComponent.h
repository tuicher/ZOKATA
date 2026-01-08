#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

#include "ZokataEngine/systems/scene/Component.h"
#include "ZokataMath/Matrix.h"
#include "ZokataMath/Quaternion.h"
#include "ZokataMath/Vector.h"

namespace ZKT
{
namespace ENGINE
{
/**
 * @brief Transform component with local/world position, rotation, and scale.
 *
 * Manages parent-child hierarchy and updates accumulated world transforms.
 */
class TransformComponent : public Component
{
public:
    TransformComponent();

    void OnEnable() override;
    void OnDisable() override;
    void Start() override;
    void Update(float delta_seconds) override;
    void FixedUpdate(float fixed_seconds) override;

    // Position (local)
    const MATH::Vec3f& GetPosition() const;
    /**
     * @brief Sets local position.
     */
    void SetPosition(const MATH::Vec3f& pos);
    /**
     * @brief Translates local position by delta.
     */
    void Translate(const MATH::Vec3f& delta);

    // Scale (local)
    const MATH::Vec3f& GetScale() const;
    /**
     * @brief Sets local scale.
     */
    void SetScale(const MATH::Vec3f& s);
    /**
     * @brief Scales local scale by a component-wise multiplier.
     */
    void Scale(const MATH::Vec3f& multiplier);

    // Rotation helpers
    // Helpers to work with Euler angles (degrees) like Unity exposes.
    MATH::Vec3f EulerDegrees() const;
    /**
     * @brief Sets rotation from Euler angles in degrees.
     */
    void SetEulerDegrees(const MATH::Vec3f& euler);
    /**
     * @brief Rotates by Euler angles in degrees (applied to current rotation).
     */
    void Rotate(const MATH::Vec3f& euler_degrees);

    // Direct setter when you want to provide raw quaternion components.
    void SetQuaternion(float w, float x, float y, float z);
    const MATH::Quaternion& GetQuaternion() const;

    void RotateAroundAxisDegrees(const MATH::Vec3f& axis, float angle_degrees);

    // Hierarchy
    /**
     * @brief Sets parent id reference (does not change Entity hierarchy).
     */
    void SetParent(int64_t parent_id);
    int64_t Parent() const;
    const std::vector<int64_t>& Children() const;
    /**
     * @brief Adds/removes child id references.
     */
    void AddChild(int64_t child_id);
    void RemoveChild(int64_t child_id);

    // World values (computed from parent if provided)
    const MATH::Vec3f& WorldPosition() const;
    const MATH::Vec3f& WorldScale() const;
    const MATH::Quaternion& WorldRotation() const;
    /**
     * @brief Returns cached model matrix (built from world transform).
     */
    const MATH::Mat4f& GetModelMatrix() const;
    /**
     * @brief Recomputes world transforms from the local transform and optional parent.
     */
    void UpdateWorld(const TransformComponent* parent);

private:
    void MarkDirty();

    MATH::Vec3f position_ {0.0F, 0.0F, 0.0F};
    MATH::Vec3f scale_ {1.0F, 1.0F, 1.0F};
    MATH::Quaternion rotation_ {};

    MATH::Vec3f world_position_ {0.0F, 0.0F, 0.0F};
    MATH::Vec3f world_scale_ {1.0F, 1.0F, 1.0F};
    MATH::Quaternion world_rotation_ {};

    int64_t parent_id_ = -1;
    std::vector<int64_t> children_ids_;
    bool dirty_ = true;
    mutable bool model_dirty_ = true;
    mutable MATH::Mat4f model_matrix_ {};
};
}  // namespace ENGINE
}  // namespace ZKT
