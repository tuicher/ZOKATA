#include "ZokataEngine/systems/scene/components/Primitives/MeshPrimitives.h"

#include <algorithm>

#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

namespace ZKT
{
namespace ENGINE
{
namespace
{
struct CubeFace
{
    MATH::Vec3f normal;
    MATH::Vec3f tangent;
    MATH::Vec3f bitangent;
};

MeshVertex MakeVertex(const MATH::Vec3f& position,
                      const MATH::Vec3f& normal,
                      const MATH::Vec2f& uv,
                      const MATH::Vec3f& tangent)
{
    MeshVertex vertex {};
    vertex.position = position;
    vertex.normal = normal;
    vertex.uv = uv;
    vertex.tangent = tangent;
    return vertex;
}
}  // namespace

MeshGeometry MakeCube(const CubeParams& params)
{
    MeshGeometry mesh {};
    mesh.vertices.reserve(24);
    mesh.indices.reserve(36);

    const MATH::Vec3f half {
        params.size.x * 0.5F,
        params.size.y * 0.5F,
        params.size.z * 0.5F,
    };

    const CubeFace faces[] = {
        {{1.0F, 0.0F, 0.0F}, {0.0F, 0.0F, -1.0F}, {0.0F, 1.0F, 0.0F}},   // +X
        {{-1.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F, 0.0F}},   // -X
        {{0.0F, 1.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 0.0F, -1.0F}},   // +Y
        {{0.0F, -1.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F}},   // -Y
        {{0.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}},    // +Z
        {{0.0F, 0.0F, -1.0F}, {-1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}},  // -Z
    };

    for (const CubeFace& face : faces)
    {
        const MATH::Vec3f center {
            face.normal.x * half.x,
            face.normal.y * half.y,
            face.normal.z * half.z,
        };

        const MATH::Vec3f u {
            face.tangent.x * half.x,
            face.tangent.y * half.y,
            face.tangent.z * half.z,
        };
        const MATH::Vec3f v {
            face.bitangent.x * half.x,
            face.bitangent.y * half.y,
            face.bitangent.z * half.z,
        };

        const uint32_t base_index = static_cast<uint32_t>(mesh.vertices.size());

        mesh.vertices.push_back(MakeVertex(center - u - v, face.normal, {0.0F, 0.0F}, face.tangent));
        mesh.vertices.push_back(MakeVertex(center + u - v, face.normal, {1.0F, 0.0F}, face.tangent));
        mesh.vertices.push_back(MakeVertex(center + u + v, face.normal, {1.0F, 1.0F}, face.tangent));
        mesh.vertices.push_back(MakeVertex(center - u + v, face.normal, {0.0F, 1.0F}, face.tangent));

        mesh.indices.push_back(base_index + 0);
        mesh.indices.push_back(base_index + 1);
        mesh.indices.push_back(base_index + 2);

        mesh.indices.push_back(base_index + 0);
        mesh.indices.push_back(base_index + 2);
        mesh.indices.push_back(base_index + 3);
    }

    return mesh;
}

MeshGeometry MakeSphere(const SphereParams& params)
{
    MeshGeometry mesh {};

    const float radius = std::max(0.0F, params.radius);
    const uint32_t longitude = std::max<uint32_t>(3, params.longitude_segments);
    const uint32_t latitude = std::max<uint32_t>(2, params.latitude_segments);

    const uint32_t rings = latitude + 1;
    const uint32_t segments = longitude + 1;
    mesh.vertices.reserve(static_cast<size_t>(rings) * static_cast<size_t>(segments));
    mesh.indices.reserve(static_cast<size_t>(latitude) * static_cast<size_t>(longitude) * 6);

    const float two_pi = glm::two_pi<float>();
    const float pi = glm::pi<float>();

    for (uint32_t lat = 0; lat <= latitude; ++lat)
    {
        const float v = static_cast<float>(lat) / static_cast<float>(latitude);
        const float theta = v * pi;
        const float sin_theta = glm::sin(theta);
        const float cos_theta = glm::cos(theta);

        for (uint32_t lon = 0; lon <= longitude; ++lon)
        {
            const float u = static_cast<float>(lon) / static_cast<float>(longitude);
            const float phi = u * two_pi;
            const float sin_phi = glm::sin(phi);
            const float cos_phi = glm::cos(phi);

            const MATH::Vec3f normal {
                sin_theta * cos_phi,
                cos_theta,
                sin_theta * sin_phi,
            };
            const MATH::Vec3f position {
                normal.x * radius,
                normal.y * radius,
                normal.z * radius,
            };
            const MATH::Vec3f tangent {-sin_phi, 0.0F, cos_phi};

            mesh.vertices.push_back(MakeVertex(position, normal, {u, 1.0F - v}, tangent));
        }
    }

    for (uint32_t lat = 0; lat < latitude; ++lat)
    {
        const uint32_t row_a = lat * (longitude + 1);
        const uint32_t row_b = (lat + 1) * (longitude + 1);

        for (uint32_t lon = 0; lon < longitude; ++lon)
        {
            const uint32_t a = row_a + lon;
            const uint32_t b = row_a + lon + 1;
            const uint32_t c = row_b + lon + 1;
            const uint32_t d = row_b + lon;

            mesh.indices.push_back(a);
            mesh.indices.push_back(b);
            mesh.indices.push_back(c);

            mesh.indices.push_back(a);
            mesh.indices.push_back(c);
            mesh.indices.push_back(d);
        }
    }

    return mesh;
}
}  // namespace ENGINE
}  // namespace ZKT
