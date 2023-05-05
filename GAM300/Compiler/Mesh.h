#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include "assimp/color4.h"

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
};

struct SampleHolder
{
    int binding;
    std::string type;
};

struct Material
{
    aiColor4D Specular; // Shiny spots on the object when light hits the surface
    aiColor4D Diffuse; // Color when light his the surface and illuminates it
    aiColor4D Ambient; // Color the surface has when not exposed directly to light from object

    std::vector<Texture> textures;

    //std::size_t GUID;
    //std::string matName;
    //std::vector<SampleHolder> _samples;
};

class Mesh {
public:
    std::vector<Vertex> _vertices; // This individual mesh vertices
    std::vector<int> _indices; // This individual mesh indices

    int materialIndex = 0; // Material index
public:
    Mesh() {};
    Mesh(std::vector<Vertex> vertices, std::vector<int> indices, int material)
        :_vertices(vertices), _indices(indices), materialIndex(material)
    {};

};

#endif