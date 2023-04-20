/* !
/file    glslshader.h
/primary author Euan Lim (50%)
/secondary author Jake Lian (50%)
/date    06/11/2016

This file contains definitions of member functions of class GLShader.
*//*__________________________________________________________________________*/
#include <Precompiled.h>
#include "glslshader.h"

/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    Checks if file exist
\param[in] file_name
    name of the file to be checked
\return
    returns true if file exist, otherwise false
*************************************************************************/
GLboolean
GLSLShader::FileExists(std::string const& file_name) {
    std::ifstream infile(file_name); return infile.good();
}

/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    deletes shader program
\param[in] NA
\return NA
*************************************************************************/
void
GLSLShader::DeleteShaderProgram() {
    if (pgm_handle > 0) {
        glDeleteProgram(pgm_handle);
    }
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    Compiles the shader programs indicated in parameter
\param[in] vec
    vector of GLenum and string.
    GLenum indicates the shader to be used.
    string contains the path to the shader programs
\return NA
*************************************************************************/
GLboolean
GLSLShader::CompileLinkValidate(std::vector<std::pair<GLenum, std::string>> vec) {
    for (auto& elem : vec) {
        if (GL_FALSE == CompileShaderFromFile(elem.first, elem.second.c_str())) {
            return GL_FALSE;
        }
    }
    if (GL_FALSE == Link()) {
        return GL_FALSE;
    }
    if (GL_FALSE == Validate()) {
        return GL_FALSE;
    }
    PrintActiveAttribs();
    PrintActiveUniforms();

    return GL_TRUE;
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    compile shader program from file
\param[in] shader_type
    fragment or vertex shader
\param[in] file_name
    name of the file
\return GLboolean
    returns true if program compiles succssfully, otherwise false
*************************************************************************/
GLboolean
GLSLShader::CompileShaderFromFile(GLenum shader_type, const std::string& file_name) {
    // returns false if file dont exist, for better performance
    if (GL_FALSE == FileExists(file_name)) {
        log_string = "File not found";
        return GL_FALSE;
    }
    // returns false if program handle dont exist, for better performance
    if (pgm_handle <= 0) {
        pgm_handle = glCreateProgram();
        if (0 == pgm_handle) {
            log_string = "Cannot create program handle";
            return GL_FALSE;
        }
    }
    // returns false if shader file dont cannot be opened, for better performance
    std::ifstream shader_file(file_name, std::ifstream::in);
    if (!shader_file) {
        log_string = "Error opening file " + file_name;
        return GL_FALSE;
    }
    // compiles shader
    std::stringstream buffer;
    buffer << shader_file.rdbuf();
    shader_file.close();
    return CompileShaderFromString(shader_type, buffer.str());
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    compile shader program using strings, without a shader file
\param[in] shader_type
    fragment or vertex shader
\param[in] file_name
    string of agorithms
\return GLboolean
    returns true if program compiles succssfully, otherwise false
*************************************************************************/
GLboolean
GLSLShader::CompileShaderFromString(GLenum shader_type,
    const std::string& shader_src) {
    if (pgm_handle <= 0) {
        pgm_handle = glCreateProgram();
        if (0 == pgm_handle) {
            log_string = "Cannot create program handle";
            return GL_FALSE;
        }
    }

    GLuint shader_handle = 0;
    switch (shader_type) {
    case VERTEX_SHADER: shader_handle = glCreateShader(GL_VERTEX_SHADER); break;
    case FRAGMENT_SHADER: shader_handle = glCreateShader(GL_FRAGMENT_SHADER); break;
    case GEOMETRY_SHADER: shader_handle = glCreateShader(GL_GEOMETRY_SHADER); break;
    case TESS_CONTROL_SHADER: shader_handle = glCreateShader(GL_TESS_CONTROL_SHADER); break;
    case TESS_EVALUATION_SHADER: shader_handle = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
        //case COMPUTE_SHADER: shader_handle = glCreateShader(GL_COMPUTE_SHADER); break;
    default:
        log_string = "Incorrect shader type";
        return GL_FALSE;
    }

    // load shader source code into shader object
    GLchar const* shader_code[] = { shader_src.c_str() };
    glShaderSource(shader_handle, 1, shader_code, NULL);

    // compile the shader
    glCompileShader(shader_handle);

    // check compilation status
    GLint comp_result;
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &comp_result);
    if (GL_FALSE == comp_result) {
        log_string = "Vertex shader compilation failed\n";
        GLint log_len;
        glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            GLchar* log = new GLchar[log_len];
            GLsizei written_log_len;
            glGetShaderInfoLog(shader_handle, log_len, &written_log_len, log);
            log_string += log;
            delete[] log;
        }
        return GL_FALSE;
    }
    else { // attach the shader to the program object
        glAttachShader(pgm_handle, shader_handle);
        return GL_TRUE;
    }
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    compile shader program from file
\param[in] NA
\return GLboolean
    returns true if program links succssfully, otherwise false
*************************************************************************/
GLboolean GLSLShader::Link() {
    if (GL_TRUE == is_linked) {
        return GL_TRUE;
    }
    if (pgm_handle <= 0) {
        return GL_FALSE;
    }

    glLinkProgram(pgm_handle); // link the various compiled shaders

    // verify the link status
    GLint lnk_status;
    glGetProgramiv(pgm_handle, GL_LINK_STATUS, &lnk_status);
    if (GL_FALSE == lnk_status) {
        log_string = "Failed to link shader program\n";
        GLint log_len;
        glGetProgramiv(pgm_handle, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            GLchar* log_str = new GLchar[log_len];
            GLsizei written_log_len;
            glGetProgramInfoLog(pgm_handle, log_len, &written_log_len, log_str);
            log_string += log_str;
            delete[] log_str;
        }
        return GL_FALSE;
    }
    return is_linked = GL_TRUE;
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief 
    starts running the shader
\return NA
*************************************************************************/
void GLSLShader::Use() {
    if (pgm_handle > 0 && is_linked == GL_TRUE) {
        glUseProgram(pgm_handle);
    }
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    stops running the shader
\return NA
*************************************************************************/
void GLSLShader::UnUse() {
    glUseProgram(0);
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    stops running the shader
\return GLboolean
    returns true if program validates, otherwise false
*************************************************************************/
GLboolean GLSLShader::Validate() {
    //returns false if any of these are false
    if (pgm_handle <= 0 || is_linked == GL_FALSE) {
        return GL_FALSE;
    }

    glValidateProgram(pgm_handle);
    GLint status;
    glGetProgramiv(pgm_handle, GL_VALIDATE_STATUS, &status);
    //returns false if status of program is false
    if (GL_FALSE == status) {
        log_string = "Failed to validate shader program for current OpenGL context\n";
        GLint log_len;
        glGetProgramiv(pgm_handle, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            GLchar* log_str = new GLchar[log_len];
            GLsizei written_log_len;
            glGetProgramInfoLog(pgm_handle, log_len, &written_log_len, log_str);
            log_string += log_str;
            delete[] log_str;
        }
        return GL_FALSE;
    }

      return GL_TRUE;

}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    gets value of pgm_handle
\return GLboolean
    returns true if program validates, otherwise false
*************************************************************************/
GLuint GLSLShader::GetHandle() const {
    return pgm_handle;
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    check if shader is linked
\return GLboolean
    returns true if program is linked, otherwise false
*************************************************************************/
GLboolean GLSLShader::IsLinked() const {
    return is_linked;
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    gets logging status of shader
\return std::string
    logger information
*************************************************************************/
std::string GLSLShader::GetLog() const {
    return log_string;
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    prints out active attribute of shaders with orion logging system
\return NA
*************************************************************************/
void GLSLShader::PrintActiveAttribs() const {
#if 1
    GLint max_length, num_attribs;
    glGetProgramiv(pgm_handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);
    glGetProgramiv(pgm_handle, GL_ACTIVE_ATTRIBUTES, &num_attribs);
    GLchar* pname = new GLchar[max_length];
    std::stringstream sstr;
    sstr << "ACTIVE SHADER ATTRIBUTES:\n"
        << "\nIndex\t|\tName\n"
        << "----------------------------------------------------------------------\n";
    for (GLint i = 0; i < num_attribs; ++i) {
        GLsizei written;
        GLint size;
        GLenum type;
        glGetActiveAttrib(pgm_handle, i, max_length, &written, &size, &type, pname);
        GLint loc = glGetAttribLocation(pgm_handle, pname);
        sstr << i << ". " << loc << "\t\t" << pname << "\n";
    }
    sstr << "----------------------------------------------------------------------\n";
    //ORION_ENGINE_INFO(sstr.str());
    std::cout << sstr.str();
    delete[] pname;

#else
    GLint numAttribs;
    glGetProgramInterfaceiv(pgm_handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);
    GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
    std::cout << "Active attributes:" << std::endl;
    for (GLint i = 0; i < numAttribs; ++i) {
        GLint results[3];
        glGetProgramResourceiv(pgm_handle, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

        GLint nameBufSize = results[0] + 1;
        GLchar* pname = new GLchar[nameBufSize];
        glGetProgramResourceName(pgm_handle, GL_PROGRAM_INPUT, i, nameBufSize, NULL, pname);
        //   std::cout << results[2] << " " << pname << " " << getTypeString(results[1]) << std::endl;
        std::cout << results[2] << " " << pname << " " << results[1] << std::endl;
        delete[] pname;
    }
#endif
}
/*!***********************************************************************
\author
    Euan Lim & Jake Lian
\brief
    prints out active Uniforms of shaders with orion logging system
\return NA
*************************************************************************/
void GLSLShader::PrintActiveUniforms() const {
    GLint max_length;
    glGetProgramiv(pgm_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
    GLchar* pname = new GLchar[max_length];
    GLint num_uniforms;
    glGetProgramiv(pgm_handle, GL_ACTIVE_UNIFORMS, &num_uniforms);
    std::stringstream sstr;
    sstr << "ACTIVE SHADER UNIFORMS:\n"
        << "Location\t|\tName\n"
        << "----------------------------------------------------------------------\n";
    for (GLint i = 0; i < num_uniforms; ++i) {
        GLsizei written;
        GLint size;
        GLenum type;
        glGetActiveUniform(pgm_handle, i, max_length, &written, &size, &type, pname);
        GLint loc = glGetUniformLocation(pgm_handle, pname);
        sstr << i << ". " << loc << "\t\t" << pname << "\n";
    }
    sstr << "----------------------------------------------------------------------\n";
    //ORION_ENGINE_INFO(sstr.str());
    std::cout << sstr.str();
    delete[] pname;
}
