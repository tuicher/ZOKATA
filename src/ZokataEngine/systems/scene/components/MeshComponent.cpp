#include "ZokataEngine/systems/scene/components/MeshComponent.h"

#include <utility>

namespace ZKT
{
namespace ENGINE
{
MeshComponent::MeshComponent(MeshGeometry geometry)
    : geometry_(std::move(geometry))
{
}

MeshComponent::MeshComponent(MeshGeometry geometry, MaterialDescriptor material)
    : geometry_(std::move(geometry))
    , material_(std::move(material))
{
}

void MeshComponent::OnEnable() {}

void MeshComponent::OnDisable() {}

void MeshComponent::Start() {}

void MeshComponent::Update(float /*delta_seconds*/) {}

void MeshComponent::FixedUpdate(float /*fixed_seconds*/) {}

const MeshGeometry& MeshComponent::Geometry() const
{
    return geometry_;
}

const MaterialDescriptor& MeshComponent::Material() const
{
    return material_;
}

bool MeshComponent::IsVisible() const
{
    return visible_;
}

bool MeshComponent::NeedsUpload() const
{
    return dirty_;
}

void MeshComponent::MarkUploaded()
{
    dirty_ = false;
}

void MeshComponent::SetGeometry(MeshGeometry geometry)
{
    geometry_ = std::move(geometry);
    dirty_ = true;
}

void MeshComponent::SetMaterial(MaterialDescriptor material)
{
    material_ = std::move(material);
    dirty_ = true;
}

MeshGeometry& MeshComponent::GeometryMutable()
{
    dirty_ = true;
    return geometry_;
}

MaterialDescriptor& MeshComponent::MaterialMutable()
{
    dirty_ = true;
    return material_;
}

void MeshComponent::SetVisible(bool visible)
{
    visible_ = visible;
}

const std::string& MeshComponent::MeshAssetId() const
{
    return mesh_asset_id_;
}

void MeshComponent::SetMeshAssetId(std::string id)
{
    mesh_asset_id_ = std::move(id);
    // TODO: Resolve asset -> geometry once asset system is wired.
}

const std::string& MeshComponent::MaterialAssetId() const
{
    return material_asset_id_;
}

void MeshComponent::SetMaterialAssetId(std::string id)
{
    material_asset_id_ = std::move(id);
    // TODO: Resolve asset -> material/shader/textures once material system exists.
}
}  // namespace ENGINE
}  // namespace ZKT
