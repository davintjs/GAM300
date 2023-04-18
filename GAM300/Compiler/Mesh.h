#ifndef MESH_H
#define MESH_H

#include "../GAM300/Source/Precompiled.h"
#include "../External/GLM/glm/glm.hpp"
struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 tex;
    glm::vec4 color;
};

struct Texture
{
    std::string filepath;
    //xgpu::texture instance;
};

struct SampleHolder
{
    int binding;
    std::string type;
};

struct Material
{
    std::size_t GUID;
    std::string matName;
    std::vector<SampleHolder> _samples;
};

//class Mesh
//{
//public:
//    std::vector<Vertex> mVertices;
//    std::vector<int> mIndices;
//
//    int m_iMaterial;
//
//public:
//    Mesh() {};
//
//    Mesh(xgpu::device Dev, const std::vector<Vertex> Vertices, const std::vector<int> Indices, const int iMaterial) noexcept
//        : mDevice(Dev), mVertices(Vertices), mIndices(Indices), m_iMaterial(iMaterial)
//    {
//        setupMesh();
//    }
//
//    Mesh(const std::vector<Vertex> Vertices, const std::vector<int> Indices, const int iMaterial) noexcept
//        : mVertices(Vertices), mIndices(Indices), m_iMaterial(iMaterial) {}
//
//    void Draw(xgpu::cmd_buffer& CmdBuffer);
//
//private:
//
//    // Render data
//    xgpu::buffer m_VertexBuffer;
//    xgpu::buffer m_IndexBuffer;
//    xgpu::vertex_descriptor m_VertexDescriptor;
//
//    // Initializes all the buffer objects/arrays
//    xgpu::device::error* setupMesh(void);
//};

#endif