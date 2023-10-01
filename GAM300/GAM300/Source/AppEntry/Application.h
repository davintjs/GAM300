/*!***************************************************************************************
\file			Application.h
\project
\author         Everyone

\par			Course: GAM300
\date           28/08/2023

\brief
	This file contains the declarations of the following:
	1. Application

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef APPLICATION_H
#define APPLICATION_H

class Application
{
public:
	// Constructor and destructor for the application
	Application();
	~Application();

	// Initializing the application
	void Init();

	// The update loop of the application
	void Run();

	// Exit the application
	void Exit();

	// Create an application
	static Application* CreateApp();
};

#endif // !APPLICATION_H