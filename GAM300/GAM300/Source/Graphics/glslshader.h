/* !
@file    glslshader.h
/primary author Euan Lim (50%)
/secondary author Jake Lian (50%)
@date    09/28/2023

For this project, authors have removed unnecessary functions..

This file contains the declaration of class GLSLShader that encapsulates the
functionality required to load shader source; compile shader source; link
shader objects into a program object, validate program object; log msesages
from compiling linking, and validation steps; install program object in
appropriate shaders; and pass uniform variables from the client to the
program object.

*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLSLSHADER_H
#define GLSLSHADER_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>

/*  _________________________________________________________________________ */
class GLSLShader
  /*! GLSLShader class.
  */
{
public:
  //~GLSLShader() { DeleteShaderProgram(); std::cout << "Deleted Shader Program" << std::endl; }
  // default ctor required to initialize GLSLShader object to safe state
  GLSLShader() : pgm_handle(0), is_linked(GL_FALSE) { /* empty by design */ }

  // This function not only compiles individual shader sources but links
  // multiple shader objects to create an exectuable shader program.
  // For each shader source, the function requires the full path to the file 
  // (containing shader source) and the type of shader program (vertex,
  // fragment, geometry, a tessellation type). This information is combined
  // as an std::pair object and the multiple pairs are supplied in a
  // std::vector object.
  // For each shader file, the function implements the six steps described in T
  // CompileShaderFromFile(). After the shader objects are created, a call to
  // Link() will create a shader executable program. This is followed by a call
  // to Validate() ensuring the program can execute in the current OpenGL state.
  GLboolean CompileLinkValidate(std::vector<std::pair<GLenum, std::string>>);

  // This function does the following:
  // 1) Create a shader program object if one doesn't exist
  // 2) Using first parameter, create a shader object
  // 3) Load shader source code from file specified by second parameter to
  //    shader object
  // 4) Compile shader source by calling CompileShaderFromString
  // 5) Check compilation status and log any messages to data member 
  //    "log_string"
  // 6) If compilation is successful, attach this shader object to previously
  //    created shader program  object
  GLboolean CompileShaderFromFile(GLenum shader_type, std::string const& file_name);

  // This function does the following:
  // 1) Create a shader program object if one doesn't exist
  // 2) Using first parameter, create a shader object
  // 3) Load the shader code from 2nd parameter to shader object
  // 4) Compile the shader source
  // 5) Check compilation status and log any messages to data member "log_string"
  // 6) If compilation is successful, attach this shader object to previously
  //    created shader program object ...
  GLboolean CompileShaderFromString(GLenum shader_type, std::string const& shader_src);

  // Link shader objects attached to handle pgm_handle. This member function
  // will also verify the status of the link operation (successful or not?).
  // If the shader objects did not link into a program object, then the
  // member function must retrieve and write the program object's information
  // log to data member log_string. 
  GLboolean Link();

  // Install the shader program object whose handle is encapsulated
  // by member pgm_handle
  void Use();

  // De-install previously installed shader program object using Use().
  // More correctly, after the execution of this member function, the
  // current rendering state is referring to an invalid program object.
  void UnUse();

  // check whether the executable shader program object can execute given the
  // current OpenGL state ...
  // See the glValidateProgram() reference page for more information
  // The function returns true if validatation succeeded 
  GLboolean Validate();

  // return the handle to the shader program object
  GLuint GetHandle() const;

  // have the different object code linked into a shader program?
  GLboolean IsLinked() const;

  // return logged information from the GLSL compiler and linker and
  // validation information obtained after calling Validate() ...
  std::string GetLog() const;

  // as the name implies, this function deletes a program object
  void DeleteShaderProgram();

  // display the list of active vertex attributes used by vertex shader
  void PrintActiveAttribs() const;

  // display the list of active uniform variables
  void PrintActiveUniforms() const;

private:
  enum ShaderType {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
    TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER,
    TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
    // ignore compute shader for now because it is not connected to
    // the graphics pipe
    // COMPUTE_SHADER = GL_COMPUTE_SHADER
  };

  GLuint pgm_handle = 0;  // handle to linked shader program object
  GLboolean is_linked = GL_FALSE; // has the program successfully linked?
  std::string log_string; // log for OpenGL compiler and linker messages

private:
  // return true if file (given in relative path) exists, false otherwise
  GLboolean FileExists(std::string const& file_name);
};

#endif /* GLSLSHADER_H */
