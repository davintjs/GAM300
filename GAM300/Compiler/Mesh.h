#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include "assimp/color4.h"

struct Vertex
{
    //std::int16_t posX;
    //std::int16_t posY;
    //std::int16_t posZ;

    //std::int16_t normX;
    //std::int16_t normY;
    //std::int16_t normZ;

    //std::int16_t tanX;
    //std::int16_t tanY;
    //std::int16_t tanZ;
    //std::int8_t  tanSign;

    //std::int16_t texU;
    //std::int16_t texV;

    //std::int8_t colorR;
    //std::int8_t colorG;
    //std::int8_t colorB;
    //std::int8_t colorA;

    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 tex;
    glm::ivec4 color;
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
    // Constructors and destructor
    Material() = default; // Default constructor
    ~Material()
    {
        textures.clear();
    }
    Material(const Material& rhs)
    {
        this->Specular = rhs.Specular;
        this->Diffuse = rhs.Diffuse;
        this->Ambient = rhs.Ambient;

        this->textures = rhs.textures;
    }
    Material(Material&& rhs)
    {
        this->Specular = std::move(rhs.Specular);
        this->Diffuse = std::move(rhs.Diffuse);
        this->Ambient = std::move(rhs.Ambient);

        this->textures = std::move(rhs.textures);
    }

    Material& operator=(const Material& rhs)
    {
        this->Specular = rhs.Specular;
        this->Diffuse = rhs.Diffuse;
        this->Ambient = rhs.Ambient;

        this->textures = rhs.textures;

    }
    Material& operator=(Material&& rhs)
    {
        this->Specular = std::move(rhs.Specular);
        this->Diffuse = std::move(rhs.Diffuse);
        this->Ambient = std::move(rhs.Ambient);

        this->textures = std::move(rhs.textures);
    }

    Material(float sR, float sG, float sB, float shininess,
        float dR, float dG, float dB, float dA,
        float aR, float aG, float aB, float aA)
    {
        Specular = aiColor4D(sR, sG, sB, shininess);
        Diffuse = aiColor4D(dR, dG, dB, dA);
        Ambient = aiColor4D(aR, aG, aB, aA);
    }

    aiColor4D Specular; // Shiny spots on the object when light hits the surface
    aiColor4D Diffuse; // Color when light his the surface and illuminates it
    aiColor4D Ambient; // Color the surface has when not exposed directly to light from object

    std::vector<Texture> textures;

    //std::size_t GUID;
    //std::string matName;
    //std::vector<SampleHolder> _samples;
};

class Geom_Mesh {
public:
    std::vector<Vertex> _vertices; // This individual mesh vertices
    std::vector<unsigned int> _indices; // This individual mesh indices

    //glm::vec3 mPosCompressionOffset; // This individual mesh vertices' positions' center offset from original
    //glm::vec2 mTexCompressionOffset; // This individual mesh textures' coordinates' center offset from original

    int materialIndex = 0; // Material index

    Geom_Mesh() {};
    Geom_Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, int material, glm::vec3 posOffset, glm::vec2 texOffset)
        :_vertices(vertices), _indices(indices), materialIndex(material)//, mPosCompressionOffset(posOffset), mTexCompressionOffset(texOffset)
    {};

};

#endif