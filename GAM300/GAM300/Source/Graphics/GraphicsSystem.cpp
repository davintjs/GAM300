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

//Editor_Camera testCam;

//Editor_Camera E_Camera;

void GraphicsSystem::Init()
{
	
	//std::cout << "-- Graphics Init -- " << std::endl;

	//INIT GRAPHICS HERE

	// Temporary Stuff, Things will be moved accordingly (hopefully)
	AssimpLoader assimp("Assets/Models/Skull_textured/Skull_textured.geom.desc", "Assets/Models/Skull_textured/Skull_textured.geom");
	testmodel.init(&assimp);
	EditorCam.Init();
}

void GraphicsSystem::Update()
{
	//std::cout << "-- Graphics Update -- " << std::endl;
	
	//Currently Putting in Camera Update loop here
	EditorCam.Update((float)MyFrameRateController.getDt());


	//UPDATE OR RENDER GRAPHICS HERE

	//UPDATE

	//RENDER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glClearColor(0.f, 0.5f, 0.5f, 1.f);

	glEnable(GL_DEPTH_BUFFER);
	testmodel.draw();

}

void GraphicsSystem::Exit()
{
	//std::cout << "-- Graphics Exit -- " << std::endl;

	//CLEANUP GRAPHICS HERE
}