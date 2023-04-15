#pragma once

class Application
{
public:
	Application();
	~Application();

	void Init();
	void Run();
	void Exit();

	static Application* CreateApp();
};
