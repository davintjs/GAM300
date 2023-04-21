#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"

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

class Mesh {
public:
    std::vector<Vertex> _vertices;
    std::vector<int> _indices;

    int _material; // sampler3d
public:
    Mesh() {};
    Mesh(std::vector<Vertex> vertices, std::vector<int> indices, int material)
        :_vertices(vertices), _indices(indices), _material(material)
    {};

};

#endif