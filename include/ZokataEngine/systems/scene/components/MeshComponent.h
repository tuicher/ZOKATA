#pragma once

#include <string>

#include "ZokataEngine/systems/scene/Component.h"
#include "ZokataRenderer/graphics/renderer/Renderable.h"

namespace ZKT
{
namespace ENGINE
{
// Scene-level mesh component: holds CPU mesh/material data, no Vulkan specifics.
/**
 * @brief Mesh component exposing geometry/material to the renderer without API details.
 */
class MeshComponent : public Component, public Renderable
{
public:
    MeshComponent() = default;
    explicit MeshComponent(MeshGeometry geometry);
    MeshComponent(MeshGeometry geometry, MaterialDescriptor material);

    void OnEnable() override;
    void OnDisable() override;
    void Start() override;
    void Update(float delta_seconds) override;
    void FixedUpdate(float fixed_seconds) override;

    /**
     * @brief Returns immutable geometry.
     */
    const MeshGeometry& Geometry() const override;
    /**
     * @brief Returns immutable material descriptor.
     */
    const MaterialDescriptor& Material() const override;
    bool IsVisible() const override;
    bool NeedsUpload() const override;
    void MarkUploaded() override;

    /**
     * @brief Replace geometry/material and mark for upload.
     */
    void SetGeometry(MeshGeometry geometry);
    void SetMaterial(MaterialDescriptor material);

    /**
     * @brief Mutable accessors that mark the component dirty.
     */
    MeshGeometry& GeometryMutable();
    MaterialDescriptor& MaterialMutable();

    void SetVisible(bool visible);

    /**
     * @brief Optional asset identifiers for deferred asset resolution.
     */
    const std::string& MeshAssetId() const;
    void SetMeshAssetId(std::string id);

    const std::string& MaterialAssetId() const;
    void SetMaterialAssetId(std::string id);

private:
    MeshGeometry geometry_;
    MaterialDescriptor material_;
    bool visible_ = true;
    bool dirty_ = true;
    std::string mesh_asset_id_;
    std::string material_asset_id_;
};
}  // namespace ENGINE
}  // namespace ZKT
