#include "Precompiled.h"
#include "GraphicsSystem.h"

#include "LapSupGraphics/Compiler.h"
#include "LapSupGraphics/Mesh.h"
#include "Model3d.h"
#include "Editor_Camera.h"
#include "../Core/FramerateController.h"

#include "Editor/Editor.h"

//Temporary
Model testmodel;

Model LightSource;

Model AffectedByLight;

//Editor_Camera testCam;

//Editor_Camera E_Camera;

void GraphicsSystem::Init()
{
	
	//std::cout << "-- Graphics Init -- " << std::endl;

	//INIT GRAPHICS HERE

	// Temporary Stuff, Things will be moved accordingly (hopefully)
	 
	//AssimpLoader assimp("Assets/Models/Skull_textured/Skull_textured.geom.desc", "Assets/Models/Skull_textured/Skull_textured.geom");


	//testmodel.init(&assimp);// The Shader is set up inside this init function
	testmodel.init();// The Shader is set up inside this init function

	// Magic Testing
	LightSource.cubeinit();
	LightSource.setup_lightshader();
	
	AffectedByLight.cubeinit();
	AffectedByLight.setup_affectedShader();

	// Setting up Positions

	testmodel.position = glm::vec3(0.f, 0.f, -800.f);
	LightSource.position = glm::vec3(0.f, 0.f, -300.f);
	AffectedByLight.position = glm::vec3(0.f, 0.f, -500.f);


	EditorCam.Init();
}

void GraphicsSystem::Update(float dt)
{
	//std::cout << "-- Graphics Update -- " << std::endl;
	
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



	testmodel.draw();

	LightSource.lightSource_draw();

	AffectedByLight.affectedByLight_draw(LightSource.position);

	// Bean: For unbinding framebuffer
	EditorCam.getFramebuffer().unbind();
}

void GraphicsSystem::Exit()
{
	//std::cout << "-- Graphics Exit -- " << std::endl;

	//CLEANUP GRAPHICS HERE
}