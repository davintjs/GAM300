/*!***************************************************************************************
\file			EntryPoint.cpp
\project
\author         Everyone

\par			Course: GAM300
\date           28/08/2023

\brief
	This file contains the definitions of the following:
	1. main

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "EntryPoint.h"
#include "Core/EngineCore.h"
#include "Core/FramerateController.h"
#define _CRTDBG_MAP_ALLOC

int main(int, char**) {

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	auto app = Application::CreateApp();
	if (!app)
	{
		std::cerr << "Failed to create application" << std::endl;
		exit(EXIT_FAILURE);
	}

	app->Init();
	MyEngineCore.Init();
	MyFrameRateController.Init(1/60.f);

	float dt = 0;
	while (!glfwWindowShouldClose(GLFW_Handler::ptr_window))
	{
		MyFrameRateController.Start();
		MyEngineCore.Update(dt);
		dt = MyFrameRateController.End();
	}

	MyEngineCore.Exit();
	app->Exit();

	delete app;
}
