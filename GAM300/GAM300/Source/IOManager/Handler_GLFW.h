/*!***************************************************************************************
\file			Handler_GLFW.h
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the GLFW Handler
	1. Handler for error, keyboard , mouse , window resize
	(Taken from Year 1 Professor Prasanna's Graphics Class)

All content ? 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLHELPER_H
#define GLHELPER_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <string>
#pragma warning(push, 0)
#include "GL/glew.h"
//#include "GLFW/glfw3.h"
#include "../../glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
//#include "glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
#include <glm/gtc/type_ptr.hpp> // for glm functions
#include "InputHandler.h"
#pragma warning(pop)
//#include "Orion/Core/KeyCodes.h"
//#include"include/GLFW/glfw3.h"

//#include <glapp.h>
extern bool gamescene_alt_tabbed;
void alt_tabby_magician(GLFWwindow* window, int focused);
/*  _________________________________________________________________________ */
struct GLFW_Handler
  /*! GLHelper structure to encapsulate initialization stuff ...
  */
{
  static bool init(GLint w, GLint h, std::string t);
  static void cleanup();

  // callbacks ...
  // error callback
  static void error_cb(int error, char const* description);
  // framebuffer callback
  static void fbsize_cb(GLFWwindow *ptr_win, int width, int height);
  // I/O callbacks ...
  //keyboard callback
  static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
  //mouse button callback
  static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
  //mouse scroll callback
  static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
  //mouse pos callback
  static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);
  static void update_time(double fpsCalcInt = 1.0);


  
  //variables
  static GLint width, height;
  static GLdouble fps;
  static GLdouble delta_time; // time taken to complete most recent game loop
  static std::string title;
  static GLFWwindow *ptr_window;

  static GLboolean keystateG;
  static GLboolean keystateA;
  // Task 2
  //static void print_specs();
};



#endif /* GLHELPER_H */
