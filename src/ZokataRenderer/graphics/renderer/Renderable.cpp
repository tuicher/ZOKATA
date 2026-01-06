#include "ZokataRenderer/graphics/renderer/Renderable.h"

namespace ZKT
{
bool Renderable::IsVisible() const
{
    return true;
}

bool Renderable::NeedsUpload() const
{
    return false;
}

void Renderable::MarkUploaded()
{
}
}  // namespace ZKT
