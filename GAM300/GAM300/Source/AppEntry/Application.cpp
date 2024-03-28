/*!***************************************************************************************
\file			Application.cpp
\project
\author         Everyone

\par			Course: GAM300
\date           28/08/2023

\brief
	This file contains the definitions of the following:
	1. Application

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Core/EngineCore.h"
#include "Core/EventsManager.h"

#include "Application.h"

#include "IOManager/InputHandler.h"

GLFWwindow* Application::window;
int Application::windowWidth;
int Application::windowHeight;
bool Application::windowFocused;
bool Application::windowFullscreen;

void Application::Init()
{
	InitApp(1600, 900, "Bean Factory");
}

void Application::InitApp(const int& _width, const int& _height, const std::string& _title)
{
    windowWidth = _width;
    windowHeight = _height;
    windowFocused = true;
    windowFullscreen = false;
    title = _title;

    std::string config("Data\\config.json");
    LoadConfig(config, windowWidth, windowHeight);

    if (!glfwInit())
    {
        PRINT("GLFW init has failed - abort program!!!");
        abort();
    }

    // In case a GLFW function fails, an error is reported to callback function
    glfwSetErrorCallback(CallbackError);

    // Before asking GLFW to create an OpenGL context, we specify the minimum constraints
    // in that context:
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); // window dimensions are NOT static

    window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);
    glfwSetWindowMonitor(window, NULL, 100, 100, _width, _height, 0);

    if (!window)
    {
        std::cerr << "GLFW unable to create OpenGL context - abort program\n";
        glfwTerminate();
        abort();
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, CallbackFramebufferResize);
    glfwSetWindowFocusCallback(window, CallbackWindowFocus);
    glfwSetDropCallback(window, CallbackWindowDrop);

    glfwSetKeyCallback(window, CallbackKeyState);
    glfwSetScrollCallback(window, CallbackMouseScrollState);

    // this is the default setting ...
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Initialize entry points to OpenGL functions and extensions
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Unable to initialize GLEW - error: "
            << glewGetErrorString(err) << " abort program" << std::endl;
        abort();
    }
    if (GLEW_VERSION_4_5)
    {
        PRINT("Using glew version: ", glewGetString(GLEW_VERSION));
        PRINT("Driver supports OpenGL 4.5\n");
    }
    //else
    //{
    //    std::cerr << "Driver doesn't support OpenGL 4.5 - abort program" << std::endl;
    //    abort();
    //}

    // Initialise Viewport
    glViewport(0, 0, windowWidth, windowHeight);

    fullscreen = true;
    Fullscreen(fullscreen, 1600, 900);
}

void Application::Update(float dt)
{
    if (InputHandler::isKeyButtonPressed(GLFW_KEY_F11))
    {
        fullscreen = !fullscreen;
        Fullscreen(fullscreen, 1600, 900);
    }

    Draw(dt);
}

void Application::Draw(float dt)
{
    static float fps = 1.f / dt;
    static float timer = 0.f;
    if (timer > 1.f)
    {
        fps = 1.f / dt;
        timer = 0.f;
    }
    timer += dt;

	// Printing to Windows Title Bar
	std::stringstream sstr;
	sstr << std::fixed << std::setprecision(2) << title << " | FPS: " << fps
		<< " | Resolution: " << windowWidth << " by " << windowHeight;
	glfwSetWindowTitle(window, sstr.str().c_str());

    glfwSwapBuffers(window);
}

void Application::Exit()
{
	glfwTerminate();
}

Application* Application::CreateApp()
{
	return new Application;
}

void Application::Fullscreen(const bool& _enable, const int& _width, const int& _height)
{
    // Dont update if it is already in the selected window mode
    if (_enable == windowFullscreen)
        return;

    if (_enable != windowFullscreen)
    {
        windowFullscreen = _enable;
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        // If enabled, set window to fullscreen
        if (windowFullscreen)
        {
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else // Set to maximized
        {
            if(glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) // Was maximized
                glfwSetWindowMonitor(window, NULL, 0, 30, windowWidth, windowHeight, mode->refreshRate);
            else
                glfwSetWindowMonitor(window, NULL, (int)(0.1f * _height), (int)(0.1f * _height), _width, _height, mode->refreshRate);
        }
    }
}

void Application::CallbackError(int, char const* _description)
{
#if defined(DEBUG) | defined(_DEBUG)
    std::cerr << "GLFW error: " << _description << std::endl;
#endif
    (void)_description;
}

void Application::CallbackFramebufferResize(GLFWwindow*, int _width, int _height)
{
    //PRINT("Application Resizing...\n");

    windowWidth = _width;
    windowHeight = _height;
    glViewport(0, 0, _width, _height);
}

void Application::CallbackWindowFocus(GLFWwindow* _window, int _focused)
{
    PRINT("Application Focused: ", (_focused) ? "Yes" : "No", '\n');
    windowFocused = (bool)_focused;
}

void Application::CallbackWindowDrop(GLFWwindow*, int _pathCount, const char* _paths[])
{
    PRINT("Application Getting Files...\n");
    std::vector<std::filesystem::path> paths;
    for (int i = 0; i < _pathCount; ++i) {
        paths.push_back(std::filesystem::path(_paths[i]));
    }
    DropAssetsEvent e(_pathCount, paths.data());
    EVENTS.Publish(&e);
}

void Application::CallbackKeyState(GLFWwindow*, int _key, int, int _action, int)
{
    if (GLFW_PRESS == _action)
        InputHandler::setKeystate_Press(_key);
    else if (GLFW_REPEAT == _action)
        InputHandler::setKeystate_Hold(_key);
    else if (GLFW_RELEASE == _action)
        InputHandler::setKeystate_Release(_key);
}

void Application::CallbackMouseScrollState(GLFWwindow*, double, double _yOffset)
{
    InputHandler::setMouseScroll((int)_yOffset);
}

bool Application::LoadConfig(const std::string& _filename, const int& _width, const int& _height)
{
    return false;
}

// Set the cursor's mode
void Application::SetCursorMode(const bool& _shown)
{
    if (_shown)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Application::TryExit()
{
    glfwSetWindowShouldClose(window, true);
}