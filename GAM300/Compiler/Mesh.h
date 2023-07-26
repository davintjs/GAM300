#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include "assimp/color4.h"

struct Vertex
{
    std::int16_t posX;
    std::int16_t posY;
    std::int16_t posZ;

    std::int16_t normX;
    std::int16_t normY;
    std::int16_t normZ;

    std::int16_t tanX;
    std::int16_t tanY;
    std::int16_t tanZ;

    std::int16_t texU;
    std::int16_t texV;

    std::int8_t colorR;
    std::int8_t colorG;
    std::int8_t colorB;
    std::int8_t colorA;
};

struct Texture
{
    std::string filepath;
};

//struct SampleHolder
//{
//    int binding;
//    std::string type;
//};

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
    std::vector<unsigned int> _indices; // This individual mesh indices

    glm::vec3 mPosCompressionOffset; // This individual mesh vertices' positions' center offset from original
    glm::vec2 mTexCompressionOffset; // This individual mesh textures' coordinates' center offset from original

    int materialIndex = 0; // Material index

    Mesh() {};
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, int material)
        :_vertices(vertices), _indices(indices), materialIndex(material)
    {};

};

#endif