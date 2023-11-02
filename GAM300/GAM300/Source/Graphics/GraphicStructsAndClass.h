/*!***************************************************************************************
\file			GraphicStructandClass.h
\project
\author         Lian Khai Kiat, Euan Lim, Theophelia Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains structs and classes the graphics needs


All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef GRAPHICSTRUCTANDCLASS_H
#define GRAPHICSTRUCTANDCLASS_H

#include "glm/mat4x4.hpp"
#include "GL/glew.h"
#include "AI/Geometry.h"	// Bean: For Lines, Segments, Planes and Triangles

#define EntityRenderLimit 1000
#define EnitityInstanceLimit 200

#define DEFAULT_MESH DEFAULT_ASSETS["Cube.geom"]
#define DEFAULT_TEXTURE DEFAULT_ASSETS["None.dds"]


struct temp_instance
{
	std::string name;
	glm::vec4 albedo;
	float metallic;
	float roughness;
	float ao;

};

// struct materialInstance
// {

// };



enum class SHADERTYPE
{
	HDR,
	PBR,
	TIR,// Temporary Instance Render
	TDR,// Temporary Debug Instance Render
	SKYBOX,
	BASICLIGHT,
	AFFECTEDLIGHT,
	SHADOW,
	POINTSHADOW,
	UI_SCREEN,
	UI_WORLD,
	BLUR,
	GBUFFER,
	DEFAULT,
	COLOURPICKING,
	COUNT
};

enum LIGHT_TYPE
{
	SPOT_LIGHT,
	DIRECTIONAL_LIGHT,
	POINT_LIGHT
};

struct SpotLight
{
	glm::vec3 direction;
	float inner_CutOff;
	float outer_CutOff;

	glm::vec3 lightColor;
	float intensity;
};

struct PointLight
{
	glm::vec3 lightColor;
	float intensity;
};

struct DirectionalLight
{
	glm::vec3 direction;

	glm::vec3 lightColor;
	float intensity;
};

union Light_Type
{
	SpotLight spotlight;
	PointLight pointlight;
	DirectionalLight directionallight;
};

struct LightProperties
{
	bool inUse = true;
	// Used in point & Spot
	glm::vec3 lightpos;

	// Used in directional & spot
	glm::vec3 direction;

	// Used only in Spot
	float inner_CutOff;
	float outer_CutOff;

	// Used for all
	glm::vec3 lightColor;
	float intensity;
};

struct InstanceProperties
{
	GLenum drawType;

	// instanced stuff
	unsigned int VAO;
	unsigned int debugVAO{};
	// rmb to convert everything to AOS
	unsigned int entitySRTbuffer;
	//glm::mat4 entitySRT[EnitityInstanceLimit];
	std::vector<glm::mat4> entitySRT;

	// -------------- BLINN PHONG --------------------------
	// make into individual buffers
	unsigned int AlbedoBuffer;
	//glm::vec4 Albedo[EnitityInstanceLimit]; // This means colour for now
	std::vector<glm::vec4> Albedo; // This means colour for now

	unsigned int SpecularBuffer;
	//glm::vec4 Specular[EnitityInstanceLimit];
	std::vector<glm::vec4> Specular;

	unsigned int DiffuseBuffer;
	//glm::vec4 Diffuse[EnitityInstanceLimit];
	std::vector<glm::vec4> Diffuse;

	unsigned int AmbientBuffer;
	//glm::vec4 Ambient[EnitityInstanceLimit];
	std::vector<glm::vec4> Ambient;

	unsigned int ShininessBuffer;
	//float Shininess[EnitityInstanceLimit];
	std::vector<float> Shininess;

	// -------------- PBR --------------------------
	// USES Albedo Buffer from above too

	unsigned int Metal_Rough_AO_Texture_Buffer;
	//glm::vec4 M_R_A_Texture[EnitityInstanceLimit];
	std::vector<glm::vec4> M_R_A_Texture;

	unsigned int Metal_Rough_AO_Texture_Constant;
	//glm::vec3 M_R_A_Constant[EnitityInstanceLimit]{ glm::vec3(1.f,1.f,1.f) };
	std::vector<glm::vec4> M_R_A_Constant;

	unsigned int textureIndexBuffer;
	//glm::vec2 textureIndex[EnitityInstanceLimit]; // (texture index, normal map index)
	std::vector<glm::vec2> textureIndex; // (texture index, normal map index)

	unsigned int drawCount = 0;
	unsigned int iter = 0;
	unsigned int texture[32]{};// max 32 dds only
	unsigned int textureCount = 0;

	void BatchTexture(std::string texture);
};

struct DefaultRenderProperties {
	unsigned int VAO{};

	float shininess{};
	float metallic{};
	float roughness{};
	float ao{};
	float emission{};

	glm::mat4 entitySRT{};
	glm::vec4 Albedo{};
	glm::vec4 Specular{};
	glm::vec4 Diffuse{};
	glm::vec4 Ambient{};

	GLuint textureID{};
	GLuint NormalID{};
	GLuint RoughnessID{};
	GLuint MetallicID{};
	GLuint AoID{};
	GLuint EmissionID{};

	unsigned int drawCount{};

	//GLSLShader shader;

	GLenum drawType;

	bool isAnimatable{ false };
	int boneidx{ -1 };
};


// Each Shader has 1 ShaderProperties
struct ShaderProperties {
	std::string name;
					// Index			// Var Name   // Type Enum
	std::unordered_map<size_t, std::pair<std::string, size_t>> fragmentVariables;
	std::unordered_map<size_t, std::pair<std::string, size_t>> vertexVariables;

};


#endif // !GRAPHICSTRUCTANDCLASS_H
