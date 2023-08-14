/*!
/file    GLFW_Handler.cpp
/primary author Euan Lim (60%)
/secondary author Jake Lian (40%)
/date    17/09/2022

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and use GLEW to extract function 
pointers to OpenGL implementations.

*//*__________________________________________________________________________*/




/*                                                                   includes
----------------------------------------------------------------------------- */
#include "Precompiled.h"
#include "Handler_GLFW.h"
//#include "InputHandler.h"
//#include "Orion/Core/Log.h"
//#include "Orion/Renderer/Model.h"
//#include "Orion/IOManager/MouseHandler.h"
//#include "Orion/Events/Messaging.h"
//#include "Orion/Events/Events.h"
//#include <Orion/Events/KeyEvent.h>
//#include "Orion/Renderer/Camera.h"
//#include "Orion/Scene/SceneManager.h"
//#include "Orion/ECS/Systems/AudioSystem.h"

//extern bool fullscreen;
//extern bool function_mode;
bool gamescene_alt_tabbed = false;

//#include <glapp.h>
/*                                                   objects with file scope
----------------------------------------------------------------------------- */
// static data members declared in GLFW_Handler
//bool GLFW_Handler::keyHeld[Orion::Key::MaxKey]{};
//bool GLFW_Handler::keyPressed[Orion::Key::MaxKey]{};
GLint GLFW_Handler::width;
GLint GLFW_Handler::height;
GLdouble GLFW_Handler::fps;
GLdouble GLFW_Handler::delta_time;
std::string GLFW_Handler::title;
GLFWwindow* GLFW_Handler::ptr_window;
//Orion::Events::Observable crashTestObsevable;
//extern Orion::MouseHandler Mouse;
//extern Orion::CameraSystem::Camera Cam;
//extern Orion::Renderer::FrameBuffer ImguiWindow;
static bool audioRunState{};
GLboolean GLFW_Handler::keystateG = GL_FALSE;
GLboolean GLFW_Handler::keystateA = GL_FALSE;


/*!*****************************************************************************
\author
    Euan Lim
\brief
    Handler for Alt Tab
param [in] window
    GLFW window
param [in] focused
    alt tab or not
\return
    void
*******************************************************************************/

/*
void alt_tabby_magician(GLFWwindow* window, int focused)
{
	if (!focused) // alt tabbed out
	{
        audioRunState = Orion::Audio::audioRun;
		if (SCENEMANAGER->GetSceneName() == "Level1_Alpha" || SCENEMANAGER->GetSceneName() == "Level1_Alphatemp"
            || SCENEMANAGER->GetSceneName() == "Level2" || SCENEMANAGER->GetSceneName() == "Level2temp"
            || SCENEMANAGER->GetSceneName() == "Checkpoint" || SCENEMANAGER->GetSceneName() == "Checkpointtemp")
		{
            gamescene_alt_tabbed = true;
			//std::cout << "alt tabbed INSIDE THE GAME SCENE\n";
			////paused_toggle = true;
			//pause_state = true;
			//auto& gameobjects = SCENEMANAGER->GetGameObjectManager().GetGameObjects();
			//for (auto& g_Object : gameobjects)
			//{
			//	if (g_Object->GetTag() == "Pause" || g_Object->GetTag() == "Pause_Extra")
			//	{
			//		std::cout << "ininin\n";
			//		g_Object->isActive() = true;
			//	}
			//}
		}
		else
		{

            gamescene_alt_tabbed = false;

		}
		// The window gained input focus
        // kk code here
        // ORIONAUDIOMANAGER->PauseMusic();
        Orion::Audio::audioRun = false;
	}
	else // alt tabbed back in
	{
        Orion::Audio::audioRun = audioRunState;
        gamescene_alt_tabbed = false;
    }
}
*/
/*  _________________________________________________________________________ */
/*! init

@param GLint width
@param GLint height
Dimensions of window requested by program

@param std::string title_str
String printed to window's title bar

@return bool
true if OpenGL context and GLEW were successfully initialized.
false otherwise.

Uses GLFW to create OpenGL context. GLFW's initialization follows from here:
http://www.glfw.org/docs/latest/quick.html
a window of size width x height pixels
and its associated OpenGL context that matches a core profile that is
compatible with OpenGL 4.5 and doesn't support "old" OpenGL, has 32-bit RGBA,
double-buffered color buffer, 24-bit depth buffer and 8-bit stencil buffer
with each buffer of size width x height pixels
*/
bool GLFW_Handler::init(GLint w, GLint h, std::string t) {
  GLFW_Handler::width = w;
  GLFW_Handler::height = h;
  GLFW_Handler::title = t;

    
  // Part 1
  if (!glfwInit()) {
    std::cout << "GLFW init has failed - abort program!!!" << std::endl;
    return false;
  }

  // In case a GLFW function fails, an error is reported to callback function
  glfwSetErrorCallback(GLFW_Handler::error_cb);

  // Before asking GLFW to create an OpenGL context, we specify the minimum constraints
  // in that context:
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // window dimensions are static
  glfwWindowHint(GLFW_DECORATED, GL_FALSE);
  glfwWindowHint(GLFW_FOCUSED, GL_TRUE);

  //if (fullscreen) {
  //glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE); // window dimensions are static
  //}
  //else {
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_FALSE); // window dimensions are static
  //}
  // full screen mode code
  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  //GLFW_Handler::width = mode->width;
  //GLFW_Handler::height = mode->height;

  //GLFW_Handler::ptr_window = glfwCreateWindow(w, h, 
  //title.c_str(), glfwGetPrimaryMonitor(), NULL);
  // 
  //width = mode->width;
  //height = mode->height;
  // windowed mode
  GLFW_Handler::ptr_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

  //if (fullscreen)
  //{
  //    glfwSetWindowMonitor(GLFW_Handler::ptr_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
  //    glfwSetWindowAttrib(GLFW_Handler::ptr_window, GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  //   /* glm::vec2 viewp((float)mode->width, (float)mode->height);
  //    ImguiWindow.Viewport_Size(viewp);
  //    ImguiWindow.Create();*/
  //}
  //else
  //{
      glfwSetWindowMonitor(GLFW_Handler::ptr_window, nullptr, 0, 30, GLFW_Handler::width, GLFW_Handler::height, mode->refreshRate);
      glfwSetWindowAttrib(GLFW_Handler::ptr_window, GLFW_SCALE_TO_MONITOR, GL_FALSE);
      glfwSetWindowAttrib(GLFW_Handler::ptr_window, GLFW_DECORATED, GLFW_TRUE);
  //}

 

  //glfwSetWindowMonitor(ptr_window, glfwGetPrimaryMonitor(), 0, 0, width, height, mode->refreshRate);
 
  //glViewport(0, 0, mode->width, mode->height);

  if (!GLFW_Handler::ptr_window) {
    std::cerr << "GLFW unable to create OpenGL context - abort program\n";
    glfwTerminate();
    return false;
  }
  
  glfwMakeContextCurrent(GLFW_Handler::ptr_window);
  // Callback for various user inputs
  glfwSetFramebufferSizeCallback(GLFW_Handler::ptr_window, GLFW_Handler::fbsize_cb);
  glfwSetKeyCallback(GLFW_Handler::ptr_window, GLFW_Handler::key_cb);
  glfwSetMouseButtonCallback(GLFW_Handler::ptr_window, GLFW_Handler::mousebutton_cb);
  glfwSetCursorPosCallback(GLFW_Handler::ptr_window, GLFW_Handler::mousepos_cb);
  glfwSetScrollCallback(GLFW_Handler::ptr_window, GLFW_Handler::mousescroll_cb);

  // this is the default setting ...
  glfwSetInputMode(GLFW_Handler::ptr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
 
  // Part 2: Initialize entry points to OpenGL functions and extensions
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /*std::cerr << "Unable to initialize GLEW - error: "
      << glewGetErrorString(err) << " abort program" << std::endl;*/
    return false;
  }
  /*if (GLEW_VERSION_4_5) {
    std::cout << "Using glew version: " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Driver supports OpenGL 4.5\n" << std::endl;
  } else {
    std::cerr << "Driver doesn't support OpenGL 4.5 - abort program" << std::endl;
    return false;
  }*/



  if (!GLEW_VERSION_4_5) {
      //ORION_ENGINE_ERROR("Driver doesn't support OpenGL 4.5 - abort program");
      return true;
  }
  return true;
}


/*  _________________________________________________________________________ */
/*! cleanup

@param none

@return none

For now, there are no resources allocated by the application program.
The only task is to have GLFW return resources back to the system and
gracefully terminate.
*/
void GLFW_Handler::cleanup() {
  // Part 1
  glfwTerminate();
}

/*  _________________________________________________________________________*/
/*! key_cb

@param GLFWwindow*
Handle to window that is receiving event

@param int
the keyboard key that was pressed or released

@parm int
Platform-specific scancode of the key

@parm int
GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
action will be GLFW_KEY_UNKNOWN if GLFW lacks a key token for it,
for example E-mail and Play keys.

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when keyboard buttons are pressed.
This function is called when keyboard buttons are pressed.
When the ESC key is pressed, the close flag of the window is set.

if button T M A are pressed, will trigger specific flags relating to tutorial 5 
*/

void GLFW_Handler::key_cb( [[maybe_unused]] GLFWwindow* pwin, int key, int , int action, int ) {
    // key state changes from released to pressed
    std::cout << "entered key_callback\n";
    if (GLFW_PRESS == action) {
        //std::cout << "GLFW HANDLER : PRESS\n";
        InputHandler::setKeystate_Press(key);
        //Orion::Events::KeyPressedEvent event = Orion::Events::KeyPressedEvent(static_cast<Orion::Key::KeyCode>(key), false);
        //crashTestObsevable.ProcessEvent(event);
       
    }
    else if (GLFW_REPEAT == action) {
        //std::cout << "GLFW HANDLER : REPEAT / HOLDING\n";

        //Orion::Events::KeyPressedEvent event = Orion::Events::KeyPressedEvent(static_cast<Orion::Key::KeyCode>(key), true);
        //crashTestObsevable.ProcessEvent(event);
        InputHandler::setKeystate_Hold(key);
        
    }

    else if (GLFW_RELEASE == action) {
        
        //std::cout << "GLFW HANDLER : RELEASE\n";

        InputHandler::setKeystate_Release(key);

        //Orion::Events::KeyReleasedEvent event = Orion::Events::KeyReleasedEvent(static_cast<Orion::Key::KeyCode>(key));
        //crashTestObsevable.ProcessEvent(event);

       

    }
}
/*  _________________________________________________________________________*/
/*! mousebutton_cb

@param GLFWwindow*
Handle to window that is receiving event

@param int
the mouse button that was pressed or released
GLFW_MOUSE_BUTTON_LEFT and GLFW_MOUSE_BUTTON_RIGHT specifying left and right
mouse buttons are most useful

@parm int
action is either GLFW_PRESS or GLFW_RELEASE

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when mouse buttons are pressed.
*/
void GLFW_Handler::mousebutton_cb(GLFWwindow*, int button, int action, int ) {
  switch (button) {
  case GLFW_MOUSE_BUTTON_LEFT:

//#ifdef _DEBUG
//    std::cout << "Left mouse button ";
//#endif
    break;
  case GLFW_MOUSE_BUTTON_RIGHT:
//#ifdef _DEBUG
//    std::cout << "Right mouse button ";
//#endif
    break;
  }
  switch (action) {
  case GLFW_PRESS:
     
//#ifdef _DEBUG
//    std::cout << "pressed!!!" << std::endl;
//#endif
    break;
  case GLFW_RELEASE:

      
//#ifdef _DEBUG
//    std::cout << "released!!!" << std::endl;
//#endif
    break;
  }
 
  /*
      if (button == GLFW_MOUSE_BUTTON_LEFT)
      {
        switch (action) {

        case GLFW_PRESS:
            Mouse.now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(Mouse.now - Mouse.last);
            //std::cout << "Time between click is : " << time_span.count() << "\n";
            if (time_span.count() <= 0.5)
            {
                Mouse.double_click = true;
                
            }
            Mouse.last = std::chrono::high_resolution_clock::now();
            

            Orion::Lclick = GL_TRUE;
           
            Mouse.l_click = true;
            break;

        case GLFW_RELEASE:

            Orion::Lclick = GL_FALSE;
            Mouse.l_click = false;

            break;

        }

      }

      if (button == GLFW_MOUSE_BUTTON_RIGHT)
      {
          switch (action) {
          case GLFW_PRESS:
              Orion::Rclick = GL_TRUE;
              Mouse.r_click = true;
              // Cam.Init();
              break;

          case GLFW_RELEASE:
              Orion::Rclick = GL_FALSE;
              Mouse.r_click = false;
              break;

          }
      }
 
      */
      /* }
  else
  {
      std::cout << "out\n";
  }*/
}

/*  _________________________________________________________________________*/
/*! mousepos_cb

@param GLFWwindow*
Handle to window that is receiving event

@param double
new cursor x-coordinate, relative to the left edge of the client area

@param double
new cursor y-coordinate, relative to the top edge of the client area

@return none

This functions receives the cursor position, measured in screen coordinates but
relative to the top-left corner of the window client area.
*/
void GLFW_Handler::mousepos_cb(GLFWwindow*, [[maybe_unused]] double xpos, [[maybe_unused]] double ypos) {
#ifdef _DEBUG
    //std::cout << "Mouse cursor position: (" << xpos << ", " << ypos << ")" << std::endl;

#endif
    /*double x, y;
    x = (xpos - (GLFW_Handler::width / 2.0));
    y = 900 - ypos;

    y -= (GLFW_Handler::height / 2.0);
    	Mouse.set_window_pos(x, y);*/
    /* std::cout << "GLFW_Handler x : " << xpos << "\n";
    std::cout << "GLFW_Handler y : " << ypos << "\n";*/

    //Mouse.set_true_pos((float)xpos, (float)ypos);
    //Orion::mousePos_Y = ypos;
    

}

/*
void GLFW_Handler::SetKeyPressed(Orion::Key::KeyCode keycode, bool pressed)
{
    keyPressed[static_cast<int32_t>(keycode)] = pressed;
}

void GLFW_Handler::SetKeyHeld(Orion::Key::KeyCode keycode, bool held)
{
    keyHeld[static_cast<int32_t>(keycode)] = held;
}

bool GLFW_Handler::IsKeyPressed(Orion::Key::KeyCode keycode)
{
    return keyPressed[static_cast<int32_t>(keycode)];
}

bool GLFW_Handler::IsKeyDown(Orion::Key::KeyCode keycode)
{
    return keyHeld[static_cast<int32_t>(keycode)];
}
bool GLFW_Handler::IsKeyRelease(Orion::Key::KeyCode keycode)
{
    auto state = glfwGetKey(ptr_window, static_cast<int32_t>(keycode));
    return state == GLFW_RELEASE;
}
*/


/*  _________________________________________________________________________*/
/*! mousescroll_cb

@param GLFWwindow*
Handle to window that is receiving event

@param double
Scroll offset along X-axis

@param double
Scroll offset along Y-axis

@return none

This function is called when the user scrolls, whether with a mouse wheel or
touchpad gesture. Although the function receives 2D scroll offsets, a simple
mouse scroll wheel, being vertical, provides offsets only along the Y-axis.
*/
void GLFW_Handler::mousescroll_cb(GLFWwindow *, [[maybe_unused]] double x, [[maybe_unused]] double y) {
    //UNREFERENCED_PARAMETER(x);
#ifdef _DEBUG
  /*std::cout << "Mouse scroll wheel offset: ("
    << xoffset << ", " << yoffset << ")" << std::endl;*/
#endif
   /* if (y > 0 && Mouse.within_window)
    {
        Cam.Move_Front();
    }
    else if ( Mouse.within_window)
    {
        Cam.Move_Back();
    }*/
}

/*  _________________________________________________________________________ */
/*! error_cb

@param int
GLFW error code

@parm char const*
Human-readable description of the code

@return none

The error callback receives a human-readable description of the error and
(when possible) its cause.
*/
void GLFW_Handler::error_cb(int , [[maybe_unused]]  char const* description) {
    //UNREFERENCED_PARAMETER(description);
#ifdef _DEBUG
  std::stringstream sstr;
  sstr << "GLFW error: " << description << std::endl;
  //ORION_ENGINE_ERROR(sstr.str());
#endif
}

/*  _________________________________________________________________________ */
/*! fbsize_cb

@param GLFWwindow*
Handle to window that is being resized

@parm int
Width in pixels of new window size

@parm int
Height in pixels of new window size

@return none

This function is called when the window is resized - it receives the new size
of the window in pixels.
*/
void GLFW_Handler::fbsize_cb( [[maybe_unused]] GLFWwindow *pwin, int w, int h) {
    //UNREFERENCED_PARAMETER(pwin);
#ifdef _DEBUG
  std::cout << "fbsize_cb getting called!!!" << std::endl;
#endif
  // use the entire framebuffer as drawing region

  //glViewport(0, 0, w, h);
  
  // later, if working in 3D, we'll have to set the projection matrix here ...
}

/*  _________________________________________________________________________*/
/*! update_time

@param double
fps_calc_interval: the interval (in seconds) at which fps is to be
calculated

This function must be called once per game loop. It uses GLFW's time functions
to compute:
1. the interval in seconds between each frame
2. the frames per second every "fps_calc_interval" seconds
*/
void GLFW_Handler::update_time(double fps_calc_interval) {
  // get elapsed time (in seconds) between previous and current frames
  static double prev_time = glfwGetTime();
  double curr_time = glfwGetTime();
  delta_time = curr_time - prev_time;
  prev_time = curr_time;

  // fps calculations
  static double count = 0.0; // number of game loop iterations
  static double start_time = glfwGetTime();
  // get elapsed time since very beginning (in seconds) ...
  double elapsed_time = curr_time - start_time;

  ++count;

  // update fps at least every 10 seconds ...
  fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
  fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;
  if (elapsed_time > fps_calc_interval) {
    GLFW_Handler::fps = count / elapsed_time;
    start_time = curr_time;
    count = 0.0;
  }
}


/*  _________________________________________________________________________*/
/*! GLFW_Handler::print_specs()

@return void

This functions prints out information to the console, related to the GPU informations
and OpenGL context

*/
//void GLFW_Handler::print_specs() {
//    // GPU Vendor
//    GLubyte const* str_vendor = glGetString(GL_VENDOR);
//    std::cout << "GPU Vendor: " << str_vendor<< std::endl;
//
//    // GL Renderer
//    GLubyte const* str_render = glGetString(GL_RENDERER);
//    std::cout << "GL Renderer: " << str_render << std::endl;
//
//    // GL Version
//    GLubyte const* str_version = glGetString(GL_VERSION);
//    std::cout << "GL Version: " << str_version << std::endl;
//
//    // GL Shader Version
//    GLubyte const* shader_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
//    std::cout << "GL Shader Version: " << shader_version << std::endl; 
//    // GL Major Version
//    GLint major_version;
//    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
//    std::cout << "GL Major Version: " << major_version << std::endl;
//
//    // GL Minor Version
//    GLint minor_version;
//    glGetIntegerv(GL_MINOR_VERSION, &minor_version);
//    std::cout << "GL Minor Version: " << minor_version << std::endl;
//
//    // Current OpenGL Context -> double buffered?
//    GLint buffer_type;
//    glGetIntegerv(GL_DOUBLEBUFFER, &buffer_type);
//    if (buffer_type) {
//        std::cout << "Current OpenGL Context is double buffered" << std::endl;
//    }
//
//    // Maximum Vertex Count
//    GLint vertex_count;
//    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &vertex_count);
//    std::cout << "GL Maximum Vertex Count: " << vertex_count << std::endl;
//
//    // Maximum Indices Count
//    GLint indices_count;
//    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &indices_count);
//    std::cout << "GL Maximum Indices Count: " << indices_count << std::endl;\
//
//    // GL Maximum texture size
//    GLint tex_size;
//    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tex_size);
//    std::cout << "GL Maximum texture size: " << tex_size << std::endl;
//
//    // Maximum Viewport Dimensions
//    GLint viewport_dimensions[2];
//    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, viewport_dimensions);
//    std::cout << "Maximum Viewport Dimensions: " <<viewport_dimensions[0] << " x " << viewport_dimensions[1] << std::endl;
//
//}