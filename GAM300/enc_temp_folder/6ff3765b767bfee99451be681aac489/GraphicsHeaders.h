/*!***************************************************************************************
\file			GraphicsHeaders.h
\project
\author         Sean Ngo

\par			Course: GAM300
\date           10/10/2023

\brief
	This file contains the declaration of Graphics System that includes:
	1. 

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef GRAPHICSHEADERS_H
#define GRAPHICSHEADERS_H

#include <glm/gtc/type_ptr.hpp>

#include "Core/SystemInterface.h"
#include "GraphicStructsAndClass.h"
#include "BaseCamera.h"

#include "glslshader.h"
#include <filesystem>

#include "Utilities/GUID.h"

#include "Model3d.h"
//#include "glslshader.h"
#include "GBuffer.h"

#include "Scripting/ScriptFields.h"
#include "Scene/Object.h"

#define SHADER ShaderManager::Instance()
#define MYSKYBOX SkyboxManager::Instance()
#define COLOURPICKER ColourPicker::Instance()
#define DEBUGDRAW DebugDraw::Instance()
#define LIGHTING Lighting::Instance()
#define RENDERER Renderer::Instance()
#define MATERIALSYSTEM MaterialSystem::Instance()

class Ray3D;
class RaycastLine;
class SkyBox;

//// Map of all shader field types
//static std::unordered_map<std::string, size_t> shaderFieldTypeMap =
//{
//	{ "float",						GetFieldType::E<float>()},
//	{ "double",						GetFieldType::E<double>()},
//	{ "bool",						GetFieldType::E<bool>()},
//	{ "char",						GetFieldType::E<char>()},
//	{ "short",						GetFieldType::E<short>()},
//	{ "int",						GetFieldType::E<int>()},
//	{ "int64",						GetFieldType::E<int64_t>()},
//	{ "uint16_t",					GetFieldType::E<uint16_t>()},
//	{ "uint32_t",					GetFieldType::E<uint32_t>()},
//	{ "uint32_t",					GetFieldType::E<uint32_t>()},
//	{ "char*",						GetFieldType::E<char*>()},
//	{ "vec2",						GetFieldType::E<Vector2>()},
//	{ "vec3",						GetFieldType::E<Vector3>()},
//	{ "vec4",						GetFieldType::E<Vector4>()}
//};

// Graphics Settings



// Graphic Functions
void renderQuad(unsigned int& _quadVAO, unsigned int& _quadVBO);
void renderQuadWireMesh(unsigned int& _quadVAO, unsigned int& _quadVBO);
bool bloom(unsigned int amount);

using InstanceContainer = std::unordered_map<GLuint, InstanceProperties>; // <vao, properties>
// Bean: A temp solution to access the shader
// enum SHADERTYPE
// {
// 	HDR,
// 	PBR,
// 	TIR,// Temporary Instance Render
// 	TDR,// Temporary Debug Instance Render
// 	SKYBOX,
// 	BASICLIGHT,
// 	AFFECTEDLIGHT,
// 	SHADOW,
// 	POINTSHADOW,
// 	UI_SCREEN,
// 	UI_WORLD,
// 	BLUR
// };

struct Shader {
	Shader(std::string _name, SHADERTYPE type) : name(_name), shadertype(type) {}
	std::string name;
	SHADERTYPE shadertype;
};

struct Material_instance : Object
{
					// Var name   // Data Storage
	//std::unordered_map<std::string, Field> variables;// Everything inside here is the variables

	

	Material_instance() 
	{
		shaderType = (int)SHADERTYPE::PBR;
		name = "Default Material";
		albedoColour = Vector4(1.f,1.f,1.f,1.f );
		metallicConstant = 1.f ;
		roughnessConstant = 1.f ;
		aoConstant = 1.f ;
		emissionConstant = 1.f ;

		albedoTexture =		DEFAULT_TEXTURE;
		normalMap =			DEFAULT_TEXTURE;
		metallicTexture =	DEFAULT_TEXTURE;
		roughnessTexture =	DEFAULT_TEXTURE;
		aoTexture =			DEFAULT_TEXTURE;
		emissionTexture =	DEFAULT_TEXTURE;
	}

	Material_instance(const Material_instance& other) 
	{ 
		// Copy each member variable from 'other' to 'this'
		shaderType = other.shaderType;
		name = other.name;
		albedoColour = other.albedoColour;
		metallicConstant = other.metallicConstant;
		roughnessConstant = other.roughnessConstant;
		aoConstant = other.aoConstant;
		emissionConstant = other.emissionConstant;

		albedoTexture = other.albedoTexture;
		normalMap = other.normalMap;
		metallicTexture = other.metallicTexture;
		roughnessTexture = other.roughnessTexture;
		aoTexture = other.aoTexture;
		emissionTexture = other.emissionTexture;
	}

	Material_instance& Duplicate_MaterialInstance(const Material_instance& other)
	{
		// Copy each member variable from 'other' to 'this'
		shaderType = other.shaderType;
		name = other.name + " - Copy";
		albedoColour = other.albedoColour;
		metallicConstant = other.metallicConstant;
		roughnessConstant = other.roughnessConstant;
		aoConstant = other.aoConstant;
		emissionConstant = other.emissionConstant;

		albedoTexture = other.albedoTexture;
		normalMap = other.normalMap;
		metallicTexture = other.metallicTexture;
		roughnessTexture = other.roughnessTexture;
		aoTexture = other.aoTexture;
		emissionTexture = other.emissionTexture;

		return *this;
	}

	int shaderType = (int)SHADERTYPE::PBR;
	Engine::GUID matInstanceName;

	//-------------------------
	//      PBR VARIABLES
	//-------------------------
	std::string		name;
	Vector4			albedoColour;// This is pretty much used in all types of shaders
	float			metallicConstant;
	float			roughnessConstant;
	float			aoConstant;
	float			emissionConstant;

	Engine::GUID	albedoTexture;
	Engine::GUID	normalMap;
	Engine::GUID	metallicTexture;
	Engine::GUID	roughnessTexture;
	Engine::GUID	aoTexture;
	Engine::GUID	emissionTexture;



	//PBR - not instanced;



	// Blinn Phong - Not in use

	//-------------------------
	//      Blinn Phong - Not in use
	//-------------------------

	/*
	glm::vec4		specular;
	glm::vec4		diffuse;
	glm::vec4		ambient;
	float			shininess;
	*/
	property_vtable();
};

property_begin_name(Material_instance, "Material_Instance") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(matInstanceName).Name("GUID").Flags(property::flags::DONTSHOW),
		property_var(name).Name("Material Name"),
		property_var(shaderType).Name("Shader"),
		property_var(albedoColour).Name("Albedo"),
		property_var(metallicConstant).Name("Metallic"),
		property_var(roughnessConstant).Name("Roughness"),
		property_var(aoConstant).Name("AmbientOcclusion"),
		property_var(emissionConstant).Name("Emission"),
		property_var(albedoTexture).Name("AlbedoTexture"),
		property_var(normalMap).Name("NormalMap"),
		property_var(metallicTexture).Name("MetallicTexture"),
		property_var(roughnessTexture).Name("RoughnessTexture"),
		property_var(aoTexture).Name("AoTexture"),
		property_var(emissionTexture).Name("EmissionTexture"),
} property_vend_h(Material_instance)



ENGINE_SYSTEM(MaterialSystem)
{
public:
	
	void Init();
	void Update(float dt);
	void Exit();

	// Adding into material instance

	// Removing from material instance
	void createPBR_Instanced();

	void createPBR_NonInstanced();

	void AddMaterial(const Material_instance & new_mat);

	Material_instance& DuplicateMaterial(const Material_instance & instance);

	Material_instance& NewMaterialInstance(std::string _name = "Default Material");

	// Deleting a Material Instance
	void deleteInstance(Material_instance & matInstance);
	
	
	std::unordered_map< SHADERTYPE, std::vector<Material_instance> >_material;// Everything inside here is the variables

	std::vector<Shader>available_shaders;

private:

	//std::unordered_map< SHADERTYPE, std::vector<Material_instance> >_material;// Everything inside here is the variables
};


ENGINE_SYSTEM(ShaderManager)
{
public:
	void Init();
	void Update(float dt);
	void Exit();

	// All shaders will be loaded using this function and passed into shaders container
	void ShaderCompiler(const std::string & _name, const std::string& _vertPath, 
		const std::string& _fragPath, const std::string & _geometryPath = "");

	GLSLShader& GetShader(const SHADERTYPE& _type) { return shaders[static_cast<int>(_type)]; }

	void CreateShaderProperties(const std::string& _frag, const std::string& _vert);
	void ParseShaderFile(const std::string& _filename, bool _frag);


	std::unordered_map <SHADERTYPE, std::vector<Material_instance>> MaterialS;

private:
	std::vector<GLSLShader> shaders;
	ShaderProperties tempPBR_Properties; // This is temporary, eventually will move into the shaderProperties below
	std::vector<ShaderProperties> shaderProperties;
};

SINGLETON(SkyboxManager)
{
public:
	void Init();

	// Initialize the skybox of the engine
	void CreateSkybox(const std::filesystem::path& _name);

	void Draw(BaseCamera& _camera);

private:
	SkyBox skyBoxModel;
	GLuint skyboxTex;
};

SINGLETON(ColourPicker)
{
public:
	void Init();

	// Initialize the skybox of the engine

	void ColorPickingUI(BaseCamera & _camera);

	void Draw(glm::mat4 _projection, glm::mat4 _view, glm::mat4 _srt, GLSLShader& _shader);

private:

	// Colour Picking
	unsigned int colorPickFBO;
	unsigned int colorPickTex;

};


ENGINE_EDITOR_SYSTEM(DebugDraw)
{
public:
	void Init();
	void Update(float dt);
	void Exit();

	void SetupSegment3D();
	
	void Draw();

	void DrawSegment3D(const Segment3D& _segment3D, const glm::vec4& _color);
	void DrawSegment3D(const glm::vec3& _point1, const glm::vec3& _point2, const glm::vec4& _color);

	void DrawRay();

private:
	InstanceContainer* properties;
	std::vector<Ray3D> rayContainer;
	RaycastLine* raycastLine;
	bool enableRay = true;
};

ENGINE_SYSTEM(Lighting)
{
public:
	void Init();
	void Update(float dt);
	void Exit();

	LightProperties& GetLight() { return lightingSource; }

	std::vector<LightProperties>& GetPointLights() { return pointLightSources; }
	std::vector<LightProperties>& GetDirectionLights() { return directionLightSources; }
	std::vector<LightProperties>& GetSpotLights() { return spotLightSources; }

private:
	LightProperties lightingSource;
	std::vector<LightProperties> pointLightSources;
	std::vector<LightProperties> directionLightSources;
	std::vector<LightProperties> spotLightSources;
};

ENGINE_SYSTEM(Renderer)
{
public:
	void Init();
	void Update(float dt);
	void Exit();

	void SetupGrid(const int& _num);

	void Draw(BaseCamera& _camera);

	// Drawing UI onto screenspace
	void UIDraw_2D(BaseCamera& _camera);

	// Drawing UI onto worldspace
	void UIDraw_3D(BaseCamera& _camera);

	// Drawing Screenspace UI onto worldspace
	void UIDraw_2DWorldSpace(BaseCamera & _camera);


	void DrawMeshes(const GLuint& _vaoid, const unsigned int& _instanceCount,
		//const unsigned int& _primCount, GLenum _primType, const LightProperties& _lightSource, SHADERTYPE shaderType);
		const unsigned int& _primCount, GLenum _primType, const LightProperties& _lightSource, BaseCamera & _camera, SHADERTYPE shaderType);
	//glm::vec4 Albe, glm::vec4 Spec, glm::vec4 Diff, glm::vec4 Ambi, float Shin);
	//Materials Mat);

	void DrawGrid(const GLuint & _vaoid, const unsigned int& _instanceCount);

	void DrawDebug(const GLuint & _vaoid, const unsigned int& _instanceCount);

	void DrawDepthDirectional();

	void DrawDepthSpot();

	void DrawDepthPoint();

	bool Culling();

	void Forward();

	void Deferred();
	
	unsigned int ReturnTextureIdx(InstanceProperties& prop, const GLuint & _id);
	//unsigned int ReturnTextureIdx(const std::string & _meshName, const GLuint & _id);

	InstanceContainer& GetInstanceProperties() { return instanceProperties; }
	std::vector<InstanceContainer>& GetInstanceContainer() { return instanceContainers; }
	std::vector<DefaultRenderProperties>& GetDefaultProperties() { return defaultProperties; }

	float& GetExposure() { return exposure; }

	bool& IsHDR() { return hdr; }

	bool& enableShadows() { return renderShadow; };

	unsigned int& GetBloomCount() { return bloomCount; };

	float& GetBloomThreshold() { return bloomThreshold; };

	bool& enableBloom() { return enablebloom; };

	float& getAmbient() { return ambient; };

	gBuffer m_gBuffer;
private:
	std::unordered_map<Engine::GUID, InstanceProperties> properties;
	InstanceContainer instanceProperties; // <vao, properties>
	std::vector<InstanceContainer> instanceContainers; // subscript represents shadertype
	//InstanceContainer instanceContainers[size_t(SHADERTYPE::COUNT)]; // subscript represents shadertype
	std::vector<DefaultRenderProperties> defaultProperties;
	std::vector<std::vector<glm::mat4>*> finalBoneMatContainer;

	// Global Graphics Settings
	float exposure = 1.f;
	bool hdr = true;
	bool renderShadow = true;
	unsigned int bloomCount = 1;
	float bloomThreshold = 1.f;
	bool enablebloom;
	float ambient = 1.f;

};

//ENGINE_SYSTEM(ShadowRenderer)
//{
//public:
//	void Init();
//	void Update(float dt);
//	void Exit();
//
//};
//
//ENGINE_SYSTEM(UIRenderer)
//{
//public:
//	void Init();
//	void Update(float dt);
//	void Exit();
//
//};


void renderQuad();
#endif // !GRAPHICSHEADERS_H