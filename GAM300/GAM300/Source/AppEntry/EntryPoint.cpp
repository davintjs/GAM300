#include "Precompiled.h"
#include "EntryPoint.h"
#include "Core/EngineCore.h"
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
	MyEngineCore.Update();
	//app->Run();
	MyEngineCore.Exit();
	app->Exit();

	delete app;
}
