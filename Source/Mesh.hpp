#pragma once

class VulkanDevice;
class VertexBuffer;
class IndexBuffer;
class Texture;

#include <filesystem>
#include <memory>

class Mesh
{
public:
    Mesh(VulkanDevice* device, std::filesystem::path objPath, const std::filesystem::path& texturePath);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh operator = (const Mesh&) = delete;
    
    inline VertexBuffer& GetVertexBuffer() const { return *m_VertexBuffer; }
    inline IndexBuffer& GetIndexBuffer() const { return *m_IndexBuffer; }
    inline Texture& GetTexture() const { return *m_Texture; }
private:
    VulkanDevice* m_Device;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<Texture> m_Texture;

    std::filesystem::path m_ObjPath;
    std::filesystem::path m_TexturePath;

    void LoadMesh();
};
