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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class Application
{
public:

	// Initializing the application
	void Init();

	// Initialize the application
	void InitApp(const int& _width, const int& _height, const std::string& _title);

	void Update(float dt);

	void Draw(float dt);

	// Exit the application
	void Exit();

	// Create an application
	static Application* CreateApp();

	// Set fullscreen of the window application
	void Fullscreen(const bool& _enable, const int& _width, const int& _height);

	// Define the error callback of the windows
	static void CallbackError(int _error, char const* _description);

	// Callback when the window is resized and receives the new size of the windows in pixels
	static void CallbackFramebufferResize(GLFWwindow* _window, int _width, int _height);

	// Callback when the window gain or lose focus
	static void CallbackWindowFocus(GLFWwindow* _window, int _focused);

	// Callback when an item is dropped into the window application
	static void CallbackWindowDrop(GLFWwindow* _window, int _pathCount, const char* _paths[]);

	// Callback key state
	static void CallbackKeyState(GLFWwindow* _window, int _key, int _scanCode, int _action, int _mod);

	// Callback mouse scroll state
	static void CallbackMouseScrollState(GLFWwindow* _window, double _xOffset, double _yOffset);

	// Load configuration from file
	static bool LoadConfig(const std::string& _filename, const int& _width, const int& _height);

	// Set dimensions of the window
	void SetDimensions(const int& _width, const int& _height) { windowWidth = _width; windowHeight = _height; }

	// Get the application title
	const std::string& GetTitle() const { return title; }

	// Getter and setter for width and height
	static const int& GetWidth() { return windowWidth; }
	void SetWidth(const int& _width) { windowWidth = _width; }

	static const int& GetHeight() { return windowHeight; }
	void SetHeight(const int& _height) { windowHeight = _height; }

	// Check if the window is focused
	static const bool& IsWindowFocused() { return windowFocused; }

	// Get the window application
	static GLFWwindow* GetWindow() { return window; }

private:
	static GLFWwindow* window;

	std::string title;

	static int windowWidth;
	static int windowHeight;
	static bool windowFocused;
	static bool windowFullscreen;
	bool fullscreen = false;
};

#endif // !APPLICATION_H