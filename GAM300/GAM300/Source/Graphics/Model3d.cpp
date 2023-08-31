#include "Precompiled.h"
#include "Model3d.h"
#include "Editor_Camera.h"


#include <algorithm>

//extern Editor_Camera E_Camera;



//test

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char* imagepath) {

    unsigned char header[124];

    FILE* fp;

    /* try to open the file */
    errno_t err = fopen_s(&fp, imagepath, "rb");
    if (fp == NULL) {
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
        return 0;
    }

    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        fclose(fp);
        return 0;
    }

    /* get the surface desc */
    fread(&header, 124, 1, fp);

    unsigned int height = *(unsigned int*)&(header[8]);
    unsigned int width = *(unsigned int*)&(header[12]);
    unsigned int linearSize = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC = *(unsigned int*)&(header[80]);


    unsigned char* buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);

    unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        free(buffer);
        return 0;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
            0, size, buffer + offset);

        offset += size;
        //width /= 2;
        //height /= 2;
        width = std::max((int)width / 2, 1);
        height = std::max((int)height / 2, 1);

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if (width < 1) width = 1;
        if (height < 1) height = 1;

    }

    free(buffer);

    return textureID;


}

void Model::init(AssimpLoader* geom) {
    // inside _vertices
    /*glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 tex;
    glm::vec4 color;*/

    GLuint VAO; 
    GLuint VBO;
    GLuint EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    //std::cout << "box_wf vao is :" << VAO << "\n";
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, geom->_vertices.size() * sizeof(Vertex), &geom->_vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // test texture loading
    //std::cout << texturebuffer << "first \n";
    /*GLuint*/ texturebuffer = loadDDS("Assets/Models/Skull_textured/TD_Checker_Base_Color.dds");
    //std::cout << texturebuffer << "second \n";
    //test col link
    //for (size_t i = 0; i < geom->_vertices.size(); i++)
    //{
    //    geom->_vertices[i].color = glm::vec4(1.f, 0.f, 0.f, 1.f);
    //}

    //GLuint texturebuffer;
    glGenBuffers(1, &texturebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
    glBufferData(GL_ARRAY_BUFFER, geom->_vertices.size() * sizeof(Vertex), &geom->_vertices[0].tex, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, geom->_vertices.size() * sizeof(Vertex), &geom->_vertices[0].color, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    //

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, geom->_indices.size() * sizeof(int32_t), &geom->_indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
    vaoid = VAO;
    vboid = VBO;
    prim = GL_TRIANGLES;
    drawcount = geom->_indices.size(); // number of slices 
    setup_shader();
}




void Model::setup_shader() {
    std::vector<std::pair<GLenum, std::string>> shdr_files;
    // Vertex Shader
    shdr_files.emplace_back(std::make_pair(
        GL_VERTEX_SHADER,
        "GAM300/Source/LapSupGraphics/abnb2.vert"));
    //"Assets/Shaders/OrionVertShader.vert"));

// Fragment Shader
    shdr_files.emplace_back(std::make_pair(
        GL_FRAGMENT_SHADER,
        "GAM300/Source/LapSupGraphics/abnb2.frag"));
    //"Assets/Shaders/OrionFragShader.frag"));

    std::cout << "abnb2 SHADER\n";
    shader.CompileLinkValidate(shdr_files);
    std::cout << "\n\n";

    // if linking failed
    if (GL_FALSE == shader.IsLinked()) {
        std::stringstream sstr;
        sstr << "Unable to compile/link/validate shader programs\n";
        sstr << shader.GetLog() << "\n";
        //ORION_ENGINE_ERROR(sstr.str());
        std::cout << sstr.str();
        std::exit(EXIT_FAILURE);
    }
}

void Model::draw() {
    glEnable(GL_DEPTH_TEST); // might be sus to place this here

    glm::mat4 scaling_mat(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)

    );

    glm::mat4 rotation_mat(
        glm::vec4(cos(90.f), 0.f, -sin(90.f), 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(sin(90.f), 0.f, cos(90.f), 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );
    //glm::mat3 translation_mat = glm::mat3(1.f);

    glm::mat4 translation_mat(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(position, 1.f)
    );
    glm::mat4 SRT = translation_mat * rotation_mat * scaling_mat;
    

    shader.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shader.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getPerspMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shader.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getViewMatrix()));

    // Scuffed SRT
    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shader.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    // test
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturebuffer);
    glUniform1i(glGetUniformLocation(shader.GetHandle(), "myTextureSampler"), 0);


    glBindVertexArray(vaoid);
    glDrawElements(prim, drawcount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    shader.UnUse();
}


void Model::cubeinit()
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    // first, configure the cube's VAO (and VBO)
    //unsigned int VBO, cubeVAO;
    
    glGenVertexArrays(1, &vaoid);
    glGenBuffers(1, &vboid);

    glBindBuffer(GL_ARRAY_BUFFER, vboid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(vaoid);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    //unsigned int lightCubeVAO;
    //glGenVertexArrays(1, &lightCubeVAO);
    //glBindVertexArray(lightCubeVAO);

    //// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    //glBindBuffer(GL_ARRAY_BUFFER, vboid);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
}

void Model::setup_lightshader()
{
    std::vector<std::pair<GLenum, std::string>> shdr_files;
    // Vertex Shader
    shdr_files.emplace_back(std::make_pair(
        GL_VERTEX_SHADER,
        "GAM300/Source/Graphics/BasicLighting.vert"));
    //"Assets/Shaders/OrionVertShader.vert"));

// Fragment Shader
    shdr_files.emplace_back(std::make_pair(
        GL_FRAGMENT_SHADER,
        "GAM300/Source/Graphics/BasicLighting.frag"));
    //"Assets/Shaders/OrionFragShader.frag"));

    std::cout << "BasicLighting SHADER\n";
    shader.CompileLinkValidate(shdr_files);
    std::cout << "\n\n";

    // if linking failed
    if (GL_FALSE == shader.IsLinked()) {
        std::stringstream sstr;
        sstr << "Unable to compile/link/validate shader programs\n";
        sstr << shader.GetLog() << "\n";
        //ORION_ENGINE_ERROR(sstr.str());
        std::cout << sstr.str();
        std::exit(EXIT_FAILURE);
    }
}

void Model::setup_affectedShader()
{
    std::vector<std::pair<GLenum, std::string>> shdr_files;
    // Vertex Shader
    shdr_files.emplace_back(std::make_pair(
        GL_VERTEX_SHADER,
        "GAM300/Source/Graphics/LightAffected.vert"));
    //"Assets/Shaders/OrionVertShader.vert"));

// Fragment Shader
    shdr_files.emplace_back(std::make_pair(
        GL_FRAGMENT_SHADER,
        "GAM300/Source/Graphics/LightAffected.frag"));
    //"Assets/Shaders/OrionFragShader.frag"));

    std::cout << "LightAffected SHADER\n";
    shader.CompileLinkValidate(shdr_files);
    std::cout << "\n\n";

    // if linking failed
    if (GL_FALSE == shader.IsLinked()) {
        std::stringstream sstr;
        sstr << "Unable to compile/link/validate shader programs\n";
        sstr << shader.GetLog() << "\n";
        //ORION_ENGINE_ERROR(sstr.str());
        std::cout << sstr.str();
        std::exit(EXIT_FAILURE);
    }
}

void Model::lightSource_draw()
{
    glEnable(GL_DEPTH_TEST); // might be sus to place this here

    glm::mat4 scaling_mat(
        glm::vec4(100.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 100.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 100.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );

    glm::mat4 rotation_mat(
        glm::vec4(cos(90.f), 0.f, -sin(90.f), 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(sin(90.f), 0.f, cos(90.f), 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );
    //glm::mat3 translation_mat = glm::mat3(1.f);

    glm::mat4 translation_mat(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(position, 1.f)
    );
    glm::mat4 SRT = translation_mat * rotation_mat * scaling_mat;


    shader.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shader.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getPerspMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shader.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getViewMatrix()));

    // Scuffed SRT

    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shader.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    //// test
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, texturebuffer);
    //glUniform1i(glGetUniformLocation(shader.GetHandle(), "myTextureSampler"), 0);


    glBindVertexArray(vaoid);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    shader.UnUse();

}

void Model::affectedByLight_draw(glm::vec3 lightPos)
{

    glEnable(GL_DEPTH_TEST); // might be sus to place this here

    glm::mat4 scaling_mat(
        glm::vec4(100.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 100.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 100.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)

    );

    glm::mat4 rotation_mat(
        glm::vec4(cos(90.f), 0.f, -sin(90.f), 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(sin(90.f), 0.f, cos(90.f), 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );
    //glm::mat3 translation_mat = glm::mat3(1.f);

    glm::mat4 translation_mat(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(position, 1.f)
    );
    glm::mat4 SRT = translation_mat * rotation_mat * scaling_mat;


    shader.Use();

    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shader.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getPerspMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shader.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getViewMatrix()));

    // Scuffed SRT
    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shader.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    // Setting Up Light Thing here

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

    GLint uniform_var_loc4 =
        glGetUniformLocation(this->shader.GetHandle(),
            "lightColor");
    glUniform3fv(uniform_var_loc4, 1, 
        glm::value_ptr(lightColor));

    GLint uniform_var_loc5 =
        glGetUniformLocation(this->shader.GetHandle(),
            "objectColor");
    glUniform3fv(uniform_var_loc5, 1,
        glm::value_ptr(objectColor));

    GLint uniform_var_loc6 =
        glGetUniformLocation(this->shader.GetHandle(),
            "lightPos");
    glUniform3fv(uniform_var_loc6, 1,
        glm::value_ptr(lightPos));

    GLint uniform_var_loc7 =
        glGetUniformLocation(this->shader.GetHandle(),
            "camPos");
    glUniform3fv(uniform_var_loc7, 1,
        glm::value_ptr(EditorCam.GetCameraPosition()));



    //// test
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, texturebuffer);
    //glUniform1i(glGetUniformLocation(shader.GetHandle(), "myTextureSampler"), 0);


    glBindVertexArray(vaoid);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    shader.UnUse();

}

