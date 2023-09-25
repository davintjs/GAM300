#include "Precompiled.h"
#include "GraphicsSystem.h"

#include "LapSupGraphics/Compiler.h"
#include "LapSupGraphics/Mesh.h"
#include "Model3d.h"
#include "Editor_Camera.h"
#include "../Core/FramerateController.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Editor/Editor.h"
#include "Editor/EditorHeaders.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"

#include "MeshManager.h"

//Temporary
//Model testmodel;

//Model LightSource;

Model AffectedByLight;
bool haveTexture = false;
Model Line;

std::map<std::string, InstanceProperties> properties;

bool SwappingColorSpace = false;
//Editor_Camera testCam;

//Editor_Camera E_Camera;
std::vector<Ray3D> Ray_Container;
bool gay = false;

// Naive Solution for now

std::vector <Materials> temp_MaterialContainer;

std::vector <glm::vec4> temp_AlbedoContainer;
std::vector <glm::vec4> temp_SpecularContainer;
std::vector <glm::vec4> temp_DiffuseContainer;
std::vector <glm::vec4> temp_AmbientContainer;
std::vector <float> temp_ShininessContainer;

trans_mats SRT_Buffers[50];
GLSLShader temp_instance_shader;
GLSLShader temp_debug_shader;
LightProperties Lighting_Source;
//bool isThereLight = false;

//void InstanceSetup(GLuint vaoid);
//void InstancePropertySetup(InstanceProperties& prop);

//std::vector<std::string> faces
//{
//	FileSystem::getPath("resources/textures/skybox/right.jpg"),
//	FileSystem::getPath("resources/textures/skybox/left.jpg"),
//	FileSystem::getPath("resources/textures/skybox/top.jpg"),
//	FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
//	FileSystem::getPath("resources/textures/skybox/front.jpg"),
//	FileSystem::getPath("resources/textures/skybox/back.jpg")
//};
GLuint Skybox_Tex;
Model SkyBox_Model;

GLSLShader HDR_Shader;
bool hdr = false;
float exposure = 0.1f;

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void HDR_Shader_init()
{
	//TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID("right"));
	std::vector<std::pair<GLenum, std::string>> shdr_files;
	// Vertex Shader
	shdr_files.emplace_back(std::make_pair(
		GL_VERTEX_SHADER,
		"GAM300/Source/Graphics/HDR.vert"));

	// Fragment Shader
	shdr_files.emplace_back(std::make_pair(
		GL_FRAGMENT_SHADER,
		"GAM300/Source/Graphics/HDR.frag"));

	std::cout << "HDR SHADER\n";
	HDR_Shader.CompileLinkValidate(shdr_files);
	std::cout << "\n\n";

	// if linking failed
	if (GL_FALSE == HDR_Shader.IsLinked()) {
		std::stringstream sstr;
		sstr << "Unable to compile/link/validate shader programs\n";
		sstr << HDR_Shader.GetLog() << "\n";
		std::cout << sstr.str();
		std::exit(EXIT_FAILURE);
	}
}



//unsigned int hdrFBO;
//unsigned int rboDepth;




unsigned int ReturnTextureIdx(std::string MeshName,GLuint id);

void GraphicsSystem::Init()
{
	
	//glGenFramebuffers(1, &hdrFBO);
	//// create floating point color buffer
	//unsigned int colorBuffer;
	//glGenTextures(1, &colorBuffer);
	//glBindTexture(GL_TEXTURE_2D, colorBuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1600, 900, 0, GL_RGBA, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//// create depth buffer (renderbuffer)
	//glGenRenderbuffers(1, &rboDepth);
	//glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1600, 900);
	//// attach buffers
	//glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	std::cout << "Framebuffer not complete!" << std::endl;
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);





	Skybox_Tex = TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID("skybox_default_top"));


	SkyBox_Model.SkyBoxinit();
	SkyBox_Model.setup_skybox_shader();


	//TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID("right"));
	std::vector<std::pair<GLenum, std::string>> shdr_files;
	// Vertex Shader
	shdr_files.emplace_back(std::make_pair(
		GL_VERTEX_SHADER,
		"GAM300/Source/Graphics/InstancedRender.vert"));

	// Fragment Shader
	shdr_files.emplace_back(std::make_pair(
		GL_FRAGMENT_SHADER,
		"GAM300/Source/Graphics/InstancedRender.frag"));

	std::cout << "TEMP Instanced Render SHADER\n";
	temp_instance_shader.CompileLinkValidate(shdr_files);
	std::cout << "\n\n";

	// if linking failed
	if (GL_FALSE == temp_instance_shader.IsLinked()) {
		std::stringstream sstr;
		sstr << "Unable to compile/link/validate shader programs\n";
		sstr << temp_instance_shader.GetLog() << "\n";
		std::cout << sstr.str();
		std::exit(EXIT_FAILURE);
	}


	//debug shader
	std::vector<std::pair<GLenum, std::string>> debugshdr_files;
	// Vertex Shader
	debugshdr_files.emplace_back(std::make_pair(
		GL_VERTEX_SHADER,
		"GAM300/Source/Graphics/InstancedDebugRender.vert"));

	// Fragment Shader
	debugshdr_files.emplace_back(std::make_pair(
		GL_FRAGMENT_SHADER,
		"GAM300/Source/Graphics/InstancedDebugRender.frag"));

	std::cout << "TEMP debug Render SHADER\n";
	temp_debug_shader.CompileLinkValidate(debugshdr_files);
	std::cout << "\n\n";

	// if linking failed
	if (GL_FALSE == temp_debug_shader.IsLinked()) {
		std::stringstream sstr;
		sstr << "Unable to compile/link/validate debug shader programs\n";
		sstr << temp_debug_shader.GetLog() << "\n";
		std::cout << sstr.str();
		std::exit(EXIT_FAILURE);
	}



	//std::cout << "-- Graphics Init -- " << std::endl;

	//INIT GRAPHICS HERE
	
	HDR_Shader_init();


	glEnable(GL_EXT_texture_sRGB); // Unsure if this is required	

	// Euan RayCasting Testing
	Line.lineinit();
	
	// Setting up Positions
	//testmodel.position = glm::vec3(0.f, 0.f, -800.f);
	//LightSource.position = glm::vec3(0.f, 0.f, -300.f);
	//testmodel.position = glm::vec3(0.f, 0.f, -800.f);
	//LightSource.position = glm::vec3(0.f, 0.f, -300.f);
	AffectedByLight.position = glm::vec3(0.f, 0.f, -500.f);

	int index = 0;

	EditorCam.Init();
}

void GraphicsSystem::Update(float dt)
{
	for (auto& [name, prop] : properties) {
		std::fill_n(prop.textureIndex, EnitityInstanceLimit, glm::vec2(0.f));
		std::fill_n(prop.texture, 32, 0.f);
	}
	//std::cout << "-- Graphics Update -- " << std::endl;
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	
	Ray3D temp;
	bool checkForSelection = Raycasting(temp);
	
	float intersected = FLT_MAX;
	float temp_intersect;
	
	// Temporary Material thing
	//temp_MaterialContainer[3].Albedo = glm::vec4{ 1.f,1.f,1.f,1.f };
	temp_DiffuseContainer[3] = glm::vec4{ 1.0f, 0.5f, 0.31f,1.f };
	temp_SpecularContainer[3] = glm::vec4{ 0.5f, 0.5f, 0.5f,1.f };
	temp_AmbientContainer[3] = glm::vec4{ 1.0f, 0.5f, 0.31f,1.f };
	temp_ShininessContainer[3] = 32.f;


	temp_AlbedoContainer[3].r = static_cast<float>(sin(glfwGetTime() * 2.0));
	temp_AlbedoContainer[3].g = static_cast<float>(sin(glfwGetTime() * 0.7));
	temp_AlbedoContainer[3].b = static_cast<float>(sin(glfwGetTime() * 1.3));


	// Temporary Light stuff
	bool haveLight = false;
	for (LightSource& lightSource : currentScene.GetArray<LightSource>())
	{
		haveLight = true;
		Entity& entity{ currentScene.Get<Entity>(lightSource) };
		Transform& transform = currentScene.Get<Transform>(entity);

		Lighting_Source.lightpos = transform.translation;
		Lighting_Source.lightColor = lightSource.lightingColor;
	}
	if (!haveLight)
	{
		Lighting_Source.lightColor = glm::vec3(0.f, 0.f, 0.f);
	}

	// Update Loop
	int i = 0;
	if (InputHandler::isKeyButtonPressed(GLFW_KEY_P))
	{
		gay = !gay;
	}
	for (MeshRenderer& renderer : currentScene.GetArray<MeshRenderer>())
	{
		Mesh* t_Mesh = MeshManager.DereferencingMesh(renderer.MeshName);
		
		if (t_Mesh == nullptr)
		{
			continue;
		}

		int index = t_Mesh->index;
		
		Entity& entity = currentScene.Get<Entity>(renderer);
		Transform& transform = currentScene.Get<Transform>(entity);
		//InstanceProperties* currentProp = &properties[renderer.MeshName];

		GLuint textureID = 0;
		GLuint normalMapID = 0;
		//std::string textureGUID = AssetManager::Instance().GetAssetGUID(renderer.AlbedoTexture); // problem eh
		// use bool to see if texture exist instead...
		if (renderer.AlbedoTexture != "") {
			textureID = 
				TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID(renderer.AlbedoTexture));
		}
		if (renderer.NormalMap != "") {
			normalMapID =
				TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID(renderer.NormalMap));
		}
		float texidx = float(ReturnTextureIdx(renderer.MeshName, textureID));
		float normidx = float(ReturnTextureIdx(renderer.MeshName, normalMapID));
		// button here change norm idx to 33
		if (gay)
		{
			normidx = 33;
		}
		//std::cout << normidx << "\n";
		properties[renderer.MeshName].textureIndex[properties[renderer.MeshName].iter] = glm::vec2(texidx, normidx);

		renderer.mr_Albedo = temp_AlbedoContainer[3];
		renderer.mr_Ambient = temp_AmbientContainer[3];
		renderer.mr_Diffuse = temp_DiffuseContainer[3];
		renderer.mr_Shininess = temp_ShininessContainer[3];
		renderer.mr_Specular = temp_SpecularContainer[3];
		
		properties[renderer.MeshName].Albedo[properties[renderer.MeshName].iter] = renderer.mr_Albedo;
		properties[renderer.MeshName].Ambient[properties[renderer.MeshName].iter] = renderer.mr_Ambient;
		properties[renderer.MeshName].Diffuse[properties[renderer.MeshName].iter] = renderer.mr_Diffuse;
		properties[renderer.MeshName].Specular[properties[renderer.MeshName].iter] = renderer.mr_Specular;
		properties[renderer.MeshName].Shininess[properties[renderer.MeshName].iter] = renderer.mr_Shininess;
		properties[renderer.MeshName].entitySRT[properties[renderer.MeshName].iter] = transform.GetWorldMatrix();


		++(properties[renderer.MeshName].iter);
		char maxcount = 32;
		// newstring
		for (char namecount = 0; namecount < maxcount; ++namecount) {
			std::string newName = renderer.MeshName;
			
			newName += ('1' + namecount);
			
			if (properties.find(newName) == properties.end()) {
				break;
			}

			/*if (currentScene.HasComponent<Texture>(entity)) {
				for (int j = 0; j <= properties[newName].textureCount; ++j) {
					if (properties[newName].texture[j] == texID) {
						haveTexture = true;
					}
				}
				if (!haveTexture) {
					if (properties[newName].textureCount < 32) {
						properties[newName].texture[properties[newName].textureCount++] = texID;
					}
				}
			}*/
			/*for (int j = 0; j <= properties[newName].textureCount; ++j) {
				if (properties[newName].texture[j] == texID) {
					haveTexture = true;
				}
			}
			if (!haveTexture) {
				if (properties[newName].textureCount < 32) {
					properties[newName].texture[properties[newName].textureCount++] = texID;
				}
			}*/

			// properties[newName].entitySRT[properties[newName].iter++] = transform.GetWorldMatrix();
			//std::cout << newName << "\n";

			properties[newName].entitySRT[properties[newName].iter] = transform.GetWorldMatrix();
			properties[newName].Albedo[properties[newName].iter] = renderer.mr_Albedo;
			properties[newName].Ambient[properties[newName].iter] = renderer.mr_Ambient;
			properties[newName].Diffuse[properties[newName].iter] = renderer.mr_Diffuse;
			properties[newName].Specular[properties[newName].iter] = renderer.mr_Specular;
			properties[newName].Shininess[properties[newName].iter] = renderer.mr_Shininess;
			++(properties[newName].iter);
		}
		++i;

		// I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
		if (checkForSelection)
		{
			//glm::mat4 translation_mat(
			//	glm::vec4(1.f, 0.f, 0.f, 0.f),
			//	glm::vec4(0.f, 1.f, 0.f, 0.f),
			//	glm::vec4(0.f, 0.f, 1.f, 0.f),
			//	glm::vec4(transform.translation, 1.f)
			//);
			//glm::mat4 rotation_mat = glm::toMat4(glm::quat(transform.rotation));

			glm::mat4 transMatrix = transform.GetWorldMatrix();

			//glm::mat4 noscale = translation_mat * rotation_mat;

			glm::vec3 translation;
			glm::quat rot;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::vec3 scale;
			glm::decompose(transMatrix, scale, rot, translation, skew, perspective);

			glm::vec3 mins = scale * MeshManager.DereferencingMesh(renderer.MeshName)->vertices_min;
			glm::vec3 maxs = scale * MeshManager.DereferencingMesh(renderer.MeshName)->vertices_max;
			glm::mat4 rotMat = glm::toMat4(rot);

			if (testRayOBB(temp.origin, temp.direction, mins, maxs,
				glm::translate(glm::mat4(1.0f), translation) * rotMat, temp_intersect))
			{
				if (temp_intersect < intersected)
				{
					SelectedEntityEvent SelectingEntity(&entity);
					EVENTS.Publish(&SelectingEntity);
					intersected = temp_intersect;
				}
			}
		}
		
	}


	// I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
	if (intersected == FLT_MAX && checkForSelection) 
	{// This means that u double clicked, wanted to select something, but THERE ISNT ANYTHING
		SelectedEntityEvent selectedEvent{ 0};
		EVENTS.Publish(&selectedEvent);
	}



	//Currently Putting in Camera Update loop here


	// Bean: For binding framebuffer
	//EditorCam.getFramebuffer().bind();

	EditorCam.Update(dt);
	// Dont delete this -> To run on lab computers
	
	/*GLint maxVertexAttribs;
	glGetProgramiv(temp_instance_shader.GetHandle(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxVertexAttribs);
	std::cout << "max vertex attribs :" << maxVertexAttribs << "\n";*/
	



	// DONT DELETE THIS - EUAN need to check if like got padding or anything cause it wil break the instancing
	
	/*
	std::cout << "size of material struct is : " << sizeof(Materials) << "\n";

	Materials materialsArray[3]; // Create an array of 3 Materials
	// Calculate the size of the array
	size_t sizeOfArray = sizeof(materialsArray);

	std::cout << "Size of Materials array: " << sizeOfArray << " bytes" << std::endl;
	*/
		
	

	// Using Mesh Manager
	/*
	// instanced bind
	glBindBuffer(GL_ARRAY_BUFFER, MeshManager.mContainer.find("Cube")->second.SRT_Buffer_Index[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &entitySRT[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (auto mesh = MeshManager.mContainer.begin(); mesh != MeshManager.mContainer.end(); mesh++)
	{

		// Looping through submeshes
		for (int k = 0; k < mesh->second.SRT_Buffer_Index.size(); ++k)
		{
			glBindBuffer(GL_ARRAY_BUFFER, mesh->second.SRT_Buffer_Index[k]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &SRT_Buffers[mesh->second.index].transformation_mat[0]);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			Draw_Meshes(mesh->second.Vaoids[k], SRT_Buffers[mesh->second.index].index + 1, mesh->second.Drawcounts[k], mesh->second.prim,Lighting_Source);
		}
		SRT_Buffers[mesh->second.index].index = 0;
	}
	*/

	glViewport(0, 0, 1600, 900);
	glBindFramebuffer(GL_FRAMEBUFFER, EditorCam.getFramebuffer().hdrFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);

	Draw(); // call draw after update

	EditorCam.getFramebuffer().unbind();
	
	EditorCam.getFramebuffer().bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.5f, 0.5f, 1.f);

	// Bean: For unbinding framebuffer
	
	HDR_Shader.Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, EditorCam.getFramebuffer().colorBuffer);



	//if (InputHandler::isKeyButtonPressed(GLFW_KEY_1))
	//{
	//	hdr = !hdr;
	//}
	//if (InputHandler::isKeyButtonPressed(GLFW_KEY_9))
	//{
	//	if (exposure == 0.1)
	//	{
	//		exposure = 5.0;
	//	}
	//	else
	//	{
	//		exposure =  0.1;
	//	}
	//}

	//GLint uniform1 =
	//	glGetUniformLocation(temp_instance_shader.GetHandle(), "hdr");

	//glUniform1i(uniform1, hdr);

	//GLint uniform2 =
	//	glGetUniformLocation(temp_instance_shader.GetHandle(), "exposure");

	//glUniform1f(uniform2, exposure);

	renderQuad();
	EditorCam.getFramebuffer().unbind();

	//std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	//glDisable(GL_FRAMEBUFFER_SRGB);

}

void GraphicsSystem::Draw_Meshes(GLuint vaoid, unsigned int instance_count,
	unsigned int prim_count, GLenum prim_type, LightProperties LightSource,
	glm::vec4 Albe, glm::vec4 Spec, glm::vec4 Diff, glm::vec4 Ambi, float Shin)
{
	
	
	

	//testBox.instanceDraw(EntityRenderLimit);

	// Should loop through the

	glEnable(GL_DEPTH_TEST); // might be sus to place this here

	temp_instance_shader.Use();
	// UNIFORM VARIABLES ----------------------------------------
	// Persp Projection
	GLint uniform1 =
		glGetUniformLocation(temp_instance_shader.GetHandle(), "persp_projection");
	GLint uniform2 =
		glGetUniformLocation(temp_instance_shader.GetHandle(), "View");
	GLint uniform3 =
		glGetUniformLocation(temp_instance_shader.GetHandle(), "lightColor");
	GLint uniform4 =
		glGetUniformLocation(temp_instance_shader.GetHandle(), "lightPos");
	GLint uniform5 =
		glGetUniformLocation(temp_instance_shader.GetHandle(), "camPos");







	// Scuffed SRT
	// srt not uniform
	/*GLint uniform3 =
		glGetUniformLocation(this->shader.GetHandle(), "SRT");*/

	glUniformMatrix4fv(uniform1, 1, GL_FALSE,
		glm::value_ptr(EditorCam.getPerspMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.getViewMatrix()));
	glUniform3fv(uniform3, 1,
		glm::value_ptr(LightSource.lightColor));
	//std::cout << "LightSource Light COlor" << LightSource.lightColor.x << "\n";
	glUniform3fv(uniform4, 1,
		glm::value_ptr(LightSource.lightpos));
	glUniform3fv(uniform5, 1,
		glm::value_ptr(EditorCam.GetCameraPosition()));




	glBindVertexArray(vaoid);
	glDrawElementsInstanced(GL_TRIANGLES, prim_count, GL_UNSIGNED_INT, 0, instance_count);
	glBindVertexArray(0);

	//glBindVertexArray(0);
//}
	temp_instance_shader.UnUse();
}


void GraphicsSystem::Draw() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.5f, 0.5f, 1.f);
	glEnable(GL_DEPTH_BUFFER);



	// Looping Properties
	for (auto& [name, prop] : properties)
	{
		
		/*for (size_t i = 0; i < 32; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texIndex[i]);
		}*/
		glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//glBindBuffer(GL_ARRAY_BUFFER, prop.entityMATbuffer);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(Materials), &(prop.entityMAT[0]));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Albedo[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, prop.SpecularBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Specular[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, prop.DiffuseBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Diffuse[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, prop.AmbientBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Ambient[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, prop.ShininessBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(float), &(prop.Shininess[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, prop.textureIndexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec2), &(prop.textureIndex[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//std::cout <<  " r" << prop.entityMAT[0].Albedo.r << "\n";
		//std::cout <<  " g" << prop.entityMAT[0].Albedo.g << "\n";
		//std::cout <<  " b" << prop.entityMAT[0].Albedo.b << "\n";
		//std::cout <<  " a" << prop.entityMAT[0].Albedo.a << "\n";
		
		//std::cout <<  " a" << temp_AlbedoContainer[3].r << "\n";
		for (int i = 0; i < 32; ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, prop.texture[i]);
		}
		Draw_Meshes(prop.VAO, prop.iter, prop.drawCount, GL_TRIANGLES, Lighting_Source, 
			temp_AlbedoContainer[3], temp_SpecularContainer[3], temp_DiffuseContainer[3], temp_AmbientContainer[3], temp_ShininessContainer[3]);
	
		// FOR DEBUG DRAW
		glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		Draw_Debug(prop.debugVAO, prop.iter);

		prop.iter = 0;
	}


	// This is to render the Rays
	if (Ray_Container.size() > 0)
	{

		for (int i = 0; i < Ray_Container.size(); ++i)
		{
			Ray3D ray = Ray_Container[i];

			//std::cout << "ray " << ray.origin.x << "\n";
			//std::cout << "ray direc" << ray.direction.x << "\n";

			glm::mat4 SRT
			{
				glm::vec4(ray.direction.x * 1000000.f, 0.f , 0.f , 0.f),
				glm::vec4(0.f, ray.direction.y * 1000000.f, 0.f , 0.f),
				glm::vec4(0.f , 0.f , ray.direction.z * 1000000.f , 0.f),
				glm::vec4(ray.origin.x, ray.origin.y, ray.origin.z,1.f)
			};
			//std::cout << "in here draw\n";
			Line.debugline_draw(SRT);

		}
	}

	glDepthFunc(GL_LEQUAL);
	SkyBox_Model.SkyBoxDraw(Skybox_Tex);
	glDepthFunc(GL_LESS);


}

bool GraphicsSystem::Raycasting(Ray3D& _ray)
{
	// I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING

	if (!EditorScene::Instance().UsingGizmos() && !EditorCam.isMoving && InputHandler::isMouseButtonPressed_L())
	{
		// Bean: Click within the scene imgui window
		if (!EditorScene::Instance().WindowHovered())
			return false;

		_ray = EditorCam.Raycasting(EditorCam.GetMouseInNDC().x, EditorCam.GetMouseInNDC().y,
			EditorCam.getPerspMatrix(), EditorCam.getViewMatrix(), EditorCam.GetCameraPosition());
		Ray_Container.push_back(_ray);
		return true;
	}

	return false;
}

unsigned int ReturnTextureIdx(std::string MeshName, GLuint id) {
	if (!id) {
		return 33;
	}
	for (unsigned int iter = 0; iter < properties[MeshName].textureCount+1; ++iter) {
		if (properties[MeshName].texture[iter] == 0) {
			properties[MeshName].texture[iter] = id;
			properties[MeshName].textureCount++;
			return iter;
		}
		if (properties[MeshName].texture[iter] == id) {
			properties[MeshName].textureCount++;
			return iter;
		}
	}
	return 33;
}


void GraphicsSystem::Exit()
{
	//std::cout << "-- Graphics Exit -- " << std::endl;

	//CLEANUP GRAPHICS HERE
}

void GraphicsSystem::Draw_Debug(GLuint vaoid, unsigned int instance_count)
{
	temp_debug_shader.Use();
	// UNIFORM VARIABLES ----------------------------------------
	// Persp Projection
	GLint uniform1 =
		glGetUniformLocation(temp_debug_shader.GetHandle(), "persp_projection");
	GLint uniform2 =
		glGetUniformLocation(temp_debug_shader.GetHandle(), "View");
	glUniformMatrix4fv(uniform1, 1, GL_FALSE,
		glm::value_ptr(EditorCam.getPerspMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.getViewMatrix()));

	glBindVertexArray(vaoid);
	//glDrawElements(GL_LINES, 2 * 12, GL_UNSIGNED_INT, 0);
	glDrawElementsInstanced(GL_LINES, 2 * 12, GL_UNSIGNED_INT, 0, instance_count);

	// unbind and free stuff
	glBindVertexArray(0);
	temp_debug_shader.UnUse();
}