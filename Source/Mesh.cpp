#include "Mesh.hpp"

#include <utility>
#include "VulkanDevice.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Texture.hpp"
#include "obj_loader.h"

#include "gtx/string_cast.hpp"

Mesh::Mesh(VulkanDevice* device, std::filesystem::path objPath, const std::filesystem::path& texturePath)
    :   m_Device(device), m_ObjPath(std::move(objPath)), m_TexturePath(texturePath)
{
    LoadMesh();
    m_Texture = std::make_unique<Texture>(device, texturePath);
}

Mesh::~Mesh() = default;

void Mesh::LoadMesh()
{
    auto loader = objl::Loader();

    if (!loader.LoadFile(m_ObjPath.string()))
        throw std::runtime_error("Failed to load mesh at path: " + m_ObjPath.string());

    auto vertices = std::vector<Vertex>(loader.LoadedVertices.size());

    for (size_t i = 0; i < loader.LoadedVertices.size(); i++)
    {
        const auto& loaderVertex = loader.LoadedVertices[i];

        const auto position = glm::vec3(loaderVertex.Position.X, loaderVertex.Position.Y, loaderVertex.Position.Z);
        const auto texCoords = glm::vec2(loaderVertex.TextureCoordinate.X, loaderVertex.TextureCoordinate.Y);

        vertices[i] = Vertex(position, texCoords);
    }

    m_VertexBuffer = std::make_unique<VertexBuffer>(m_Device, vertices);
    m_IndexBuffer = std::make_unique<IndexBuffer>(m_Device, loader.LoadedIndices);
}
