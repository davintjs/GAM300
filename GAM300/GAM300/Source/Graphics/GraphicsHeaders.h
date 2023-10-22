/*!***************************************************************************************
\file			GraphicsHeaders.h
\project
\author         Sean Ngo

\par			Course: GAM300
\date           10/10/2023

\brief
	This file contains the declaration of Graphics System that includes:
	1. 

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef GRAPHICSHEADERS_H
#define GRAPHICSHEADERS_H

#include <glm/gtc/type_ptr.hpp>

#include "Core/SystemInterface.h"
#include "GraphicStructsAndClass.h"
#include "BaseCamera.h"

#include "glslshader.h"

#define SHADER ShaderManager::Instance()
#define MYSKYBOX SkyboxManager::Instance()
#define DEBUGDRAW DebugDraw::Instance()
#define LIGHTING Lighting::Instance()
#define RENDERER Renderer::Instance()

class Ray3D;
class RaycastLine;
class SkyBox;
void renderQuad(unsigned int& _quadVAO, unsigned int& _quadVBO);

// Bean: A temp solution to access the shader
enum SHADERTYPE
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
	UI_WORLD
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

	GLSLShader& GetShader(const SHADERTYPE& _type) { return shaders[_type]; }

private:
	std::vector<GLSLShader> shaders;
};

ENGINE_SYSTEM(SkyboxManager)
{
public:
	void Init();
	void Update(float dt);
	void Exit();

	// Initialize the skybox of the engine
	void CreateSkybox(const std::string& _name);

	void Draw(BaseCamera& _camera);

private:
	SkyBox* skyBoxModel;
	GLuint skyboxTex;
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
	std::map<std::string, InstanceProperties>* properties;
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

	void UIDraw_2D(BaseCamera& _camera);

	void UIDraw_3D(BaseCamera& _camera);

	void DrawMeshes(const GLuint& _vaoid, const unsigned int& _instanceCount,
		const unsigned int& _primCount, GLenum _primType, const LightProperties& _lightSource, BaseCamera & _camera);
	//glm::vec4 Albe, glm::vec4 Spec, glm::vec4 Diff, glm::vec4 Ambi, float Shin);
	//Materials Mat);

	void DrawGrid(const GLuint & _vaoid, const unsigned int& _instanceCount);

	void DrawDebug(const GLuint & _vaoid, const unsigned int& _instanceCount);

	void DrawDepth();

	bool Culling();

	void Forward();

	void Deferred();
	
	unsigned int ReturnTextureIdx(const std::string & _meshName, const GLuint & _id);

	std::map<std::string, InstanceProperties>& GetProperties() { return properties; }

	float& GetExposure() { return exposure; }

	bool& IsHDR() { return hdr; }

	bool RenderShadow;

private:
	std::map<std::string, InstanceProperties> properties;
	float exposure = 1.f;
	bool hdr = true;
};

#endif // !GRAPHICSHEADERS_H