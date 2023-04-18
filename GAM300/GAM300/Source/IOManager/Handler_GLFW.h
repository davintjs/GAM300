/* !
/file    glhelper.h
/primary author Euan Lim (50%)
/secondary author Jake Lian (50%)
/date    16/06/2022

This file contains the declaration of namespace Helper that encapsulates the
functionality required to create an OpenGL context using GLFW; use GLEW
to load OpenGL extensions; initialize OpenGL state; and finally initialize
the OpenGL application by calling initalization functions associated with
objects participating in the application.

*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLHELPER_H
#define GLHELPER_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <string>
#pragma warning(push, 0)
#include "GL/glew.h"
#include "GLFW/glfw3.h"
//#include "glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
#include <glm/gtc/type_ptr.hpp> // for glm functions

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
  static void error_cb(int error, char const* description);
  static void fbsize_cb(GLFWwindow *ptr_win, int width, int height);
  // I/O callbacks ...
  static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
  static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
  static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
  static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);
  //static void SetKeyPressed(Orion::Key::KeyCode keycode, bool pressed);
  //static void SetKeyHeld(Orion::Key::KeyCode keycode, bool held);
  //static bool IsKeyPressed(Orion::Key::KeyCode keycode);
  //static bool IsKeyDown(Orion::Key::KeyCode keycode);
  //static bool IsKeyRelease(Orion::Key::KeyCode keycode);
  static void update_time(double fpsCalcInt = 1.0);


  
  //static bool keyPressed[Orion::Key::MaxKey];
  //static bool keyHeld[Orion::Key::MaxKey];

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
