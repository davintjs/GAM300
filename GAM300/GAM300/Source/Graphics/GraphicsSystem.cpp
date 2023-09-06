#include "Precompiled.h"
#include "GraphicsSystem.h"

#include "LapSupGraphics/Compiler.h"
#include "LapSupGraphics/Mesh.h"
#include "Model3d.h"
#include "Editor_Camera.h"
#include "../Core/FramerateController.h"

#include "Editor/Editor.h"
#include "Scene/SceneManager.h"


//Temporary
Model testmodel;

Model LightSource;

Model AffectedByLight;

unsigned int testBoxbuffer;
Model testBox;

unsigned int entityPositionsBuffer;
glm::mat4 entityPositions[100];
//Editor_Camera testCam;

//Editor_Camera E_Camera;

void InstanceSetup(GLuint vaoid);

void GraphicsSystem::Init()
{
	
	//std::cout << "-- Graphics Init -- " << std::endl;

	//INIT GRAPHICS HERE

	// Temporary Stuff, Things will be moved accordingly (hopefully)
	AssimpLoader assimp("Assets/Models/Skull_textured/Skull_textured.geom.desc", "Assets/Models/Skull_textured/Skull_textured.geom");
	testmodel.init(&assimp);// The Shader is set up inside this init function
	testBox.instance_cubeinit();
	testBox.setup_instanced_shader();
	// Magic Testing
	LightSource.cubeinit();
	LightSource.setup_lightshader();
	
	AffectedByLight.cubeinit();
	AffectedByLight.setup_affectedShader();

	// Setting up Positions
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	testmodel.position = glm::vec3(0.f, 0.f, -800.f);
	LightSource.position = glm::vec3(0.f, 0.f, -300.f);
	AffectedByLight.position = glm::vec3(0.f, 0.f, -500.f);
	int index = 0;
	for (Entity& entity : currentScene.entities) {
		
		/*if (!currentScene.EntityIsActive(entity.denseIndex))
		{
			continue;
		}*/
		//Transform& trans = currentScene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(entity.denseIndex);
		entityPositions[index] = glm::mat4(1.f);
		/*if (currentScene.singleComponentsArrays.GetArray<model>()) {

		}*/
		++index;
	}

	glGenBuffers(1, &entityPositionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, entityPositionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, currentScene.entities.size() * sizeof(glm::mat4), &entityPositions[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	InstanceSetup(testBox.vaoid);

	EditorCam.Init();
}

void GraphicsSystem::Update(float dt)
{
	//std::cout << "-- Graphics Update -- " << std::endl;
	
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	currentScene.singleComponentsArrays.GetArray<Transform>();
	

	int i = 0;
	for (Entity& entity : currentScene.entities)
	{
		
		Transform& trans = currentScene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(entity.denseIndex);
		
		glm::mat4 scaling_mat(
			glm::vec4(trans.scale.x, 0.f, 0.f, 0.f),
			glm::vec4(0.f, trans.scale.y, 0.f, 0.f),
			glm::vec4(0.f, 0.f, trans.scale.z, 0.f),
			glm::vec4(0.f, 0.f, 0.f, 1.f)

		);

		glm::mat4 rotation_mat(
			glm::vec4(cos(90.f), 0.f, -sin(90.f), 0.f),
			glm::vec4(0.f, 1.f, 0.f, 0.f),
			glm::vec4(sin(90.f), 0.f, cos(90.f), 0.f),
			glm::vec4(0.f, 0.f, 0.f, 1.f)
		);
		glm::mat4 translation_mat(
			glm::vec4(1.f, 0.f, 0.f, 0.f),
			glm::vec4(0.f, 1.f, 0.f, 0.f),
			glm::vec4(0.f, 0.f, 1.f, 0.f),
			glm::vec4(trans.translation, 1.f)
		);
		glm::mat4 SRT = translation_mat * rotation_mat * scaling_mat;
		entityPositions[i] = SRT;
	}

	//Currently Putting in Camera Update loop here

	// Bean: For binding framebuffer
	EditorCam.getFramebuffer().bind();

	EditorCam.Update((float)MyFrameRateController.getDt());

	//UPDATE OR RENDER GRAPHICS HERE

	//UPDATE

	//RENDER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glClearColor(0.f, 0.5f, 0.5f, 1.f);

	glEnable(GL_DEPTH_BUFFER);

	// This one is turbo scuffed i just putting here to test  light
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_UP))
	{
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_ALT))
		{
			LightSource.position.x -= 10.f;
		}
		else
		{
			LightSource.position.y += 10.f;

		}
	}
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT))
	{
		LightSource.position.z -= 10.f;
	}
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_DOWN))
	{
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_ALT))
		{
			LightSource.position.x += 10.f;
		}
		else
		{
			LightSource.position.y -= 10.f;

		}
	}
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_RIGHT))
	{
		LightSource.position.z += 10.f;
	}

	// instanced bind
	glBindBuffer(GL_ARRAY_BUFFER, entityPositionsBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (currentScene.entities.size()) * sizeof(glm::mat4), &entityPositions[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// instanced draw
	testBox.instanceDraw();

	testmodel.draw();

	LightSource.lightSource_draw();

	/*AffectedByLight.affectedByLight_draw(LightSource.position);*/

	// Bean: For unbinding framebuffer
	EditorCam.getFramebuffer().unbind();
}

void InstanceSetup(GLuint vaoid) {
	glBindVertexArray(vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, entityPositionsBuffer);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
}

void GraphicsSystem::Exit()
{
	//std::cout << "-- Graphics Exit -- " << std::endl;

	//CLEANUP GRAPHICS HERE
}