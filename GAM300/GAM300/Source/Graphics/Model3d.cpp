#include "Precompiled.h"
#include "Model3d.h"
#include "Editor_Camera.h"


#include <algorithm>

//extern Editor_Camera E_Camera;

struct DDSHeader {
    char dwMagic[4];
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwHeight;
    uint32_t dwWidth;
    uint32_t dwPitchOrLinearSize;
    uint32_t dwDepth;
    uint32_t dwMipMapCount;
    uint32_t dwReserved1[11];
    struct {
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwFourCC;
        uint32_t dwRGBBitCount;
        uint32_t dwRBitMask;
        uint32_t dwGBitMask;
        uint32_t dwBBitMask;
        uint32_t dwABitMask;
    } ddspf;
    uint32_t dwCaps;
    uint32_t dwCaps2;
    uint32_t dwCaps3;
    uint32_t dwCaps4;
    uint32_t dwReserved2;
};


// should move somewhere else...
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

    DDSHeader ddsfile;
    
    /* get the surface desc */
    fread(&header, 124, 1, fp);

    unsigned int dwFlags = *(unsigned int*)&(header[28]); // Offset for the dwFlags field
    bool isSRGB = (dwFlags & 0x200) != 0; // Check if the DDPF_SRGB flag is set
    std::cout << "flag_1 " << std::hex << dwFlags << "\n";


    unsigned int sizerino = *(unsigned int*)&(header[0]);

    //unsigned long flag_1 = *(unsigned long*)&(header[4]);
    unsigned int height = *(unsigned int*)&(header[8]);
    unsigned int width = *(unsigned int*)&(header[12]);
    unsigned int linearSize = *(unsigned int*)&(header[16]);
    //unsigned long flag_3 = *(unsigned long*)&(header[20]);
    //unsigned int dwFlags = *(unsigned int*)&(header[28]);

    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    //unsigned long flag_2 = *(unsigned long*)&(header[76]);

    unsigned int fourCC = *(unsigned int*)&(header[80]);


    unsigned char* buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);
        
    //std::cout << "sizerino is " << sizerino << "\n";
    //char hex_string_1[20];
    //sprintf_s(hex_string_1, "%X", flag_1); //convert number to hex
    //char hex_string_2[20];
    //sprintf_s(hex_string_2, "%X", flag_2); //convert number to hex
    //char hex_string_3[20];
    //sprintf_s(hex_string_3, "%X", flag_3); //convert number to hex
    //std::cout << "flag_1 " << std::hex << flag_1 << "\n";
    //std::cout << "flag_2 " << std::hex << flag_2 << "\n";
    //std::cout << "flag_3 " << std::hex << flag_3 << "\n";
    //std::cout << "flag_2 " << flag_2 << "\n";
    //std::cout << "flag_3 " << flag_3 << "\n";

    //if (flag == 0x20) {
    //    std::cout << flag << "\n";
    //    std::cout << "here\n";
    //    // The DDPF_SRGB flag is set, indicating sRGB color space.
    //    // The texture uses sRGB color space.
    //}
    //else {
    //    std::cout << flag << "\n";

    //    std::cout << "not here\n";
    //    // The DDPF_SRGB flag is not set.
    //    // The texture uses linear RGB color space.
    //}

    unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC)
    {
    case FOURCC_DXT1:
        std::cout << "1\n";
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        std::cout << "2\n";

        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        std::cout << "3\n";

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
    std::cout << "block size is : " << blockSize << "\n";
    unsigned int offset = 0;
    
    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
      /*  glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, width, height,
            0, size, buffer + offset);*/
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Print whether the DDS texture is in sRGB or RGB color space
    if (isSRGB) {
        std::cout << "DDS texture is in sRGB color space." << std::endl;
    }
    else {
        std::cout << "DDS texture is in RGB color space." << std::endl;
    }

    return textureID;


}

void Model::init(AssimpLoader* geom) {
    // inside _vertices
    /*glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 tex;
    glm::vec4 color;*/
    _geom = geom;

    GLuint VAO; 
    GLuint VBO;
    GLuint EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    //std::cout << "box_wf vao is :" << VAO << "\n";

    // test vertex colors
    //for (size_t i = 0; i < _geom->_vertices.size(); i++)
    //{
    //    _geom->_vertices[i].color = glm::vec4(1.f, 0.f, 0.f, 1.f);
    //}

    // bind vbo and pass vertice data into the buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, _geom->_vertices.size() * sizeof(Vertex), &_geom->_vertices[0], GL_STATIC_DRAW);

    // set the vertex attributes to tell vert shader pos, uv, normal etc.
    // note offsetof uses the struct Vertex as first argument, and member name as second.
    // it returns the offset to the member name :)
    glEnableVertexAttribArray(0); //pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2); //tangent
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(3); // tex (uv coords)
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex));

    glEnableVertexAttribArray(4); // color
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    

    // bind indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _geom->_indices.size() * sizeof(int32_t), &_geom->_indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind vao
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo

    vaoid = VAO;
    vboid = VBO;
    prim = GL_TRIANGLES;
    drawcount = _geom->_indices.size(); // number of slices 

    // load default texture, todo
    texturebuffer = loadDDS("Assets/Models/Skull_textured/TD_Checker_Base_Color.dds");

   /* texturebuffer = loadDDS("Assets/Models/Skull_textured/TD_Checker_Mixed_AO.dds");
    texturebuffer = loadDDS("Assets/Models/Skull_textured/TD_Checker_Roughness.dds");

    texturebuffer = loadDDS("Assets/Models/Skull_textured/TD_Checker_Normal_OpenGL.dds");
    texturebuffer = loadDDS("Assets/Models/Skull_textured/sample_1280×853.dds");*/



    setup_shader();

    debugAABB_init();
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
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturebuffer);
    glUniform1i(glGetUniformLocation(shader.GetHandle(), "myTextureSampler"), 0);


    glBindVertexArray(vaoid);
    glDrawElements(prim, drawcount, GL_UNSIGNED_INT, nullptr);

    // unbind and free stuff
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader.UnUse();

    debugAABB_draw(SRT);
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

    glGenBuffers(1, &texturebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
    glBindTexture(GL_TEXTURE_2D, texturebuffer);
    glUniform1i(glGetUniformLocation(shader.GetHandle(), "myTextureSampler"), 0);


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
    glBindTexture(GL_TEXTURE_2D, texturebuffer);
    glUniform1i(glGetUniformLocation(shader.GetHandle(), "myTextureSampler"), 0);


    glBindVertexArray(vaoid);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    shader.UnUse();

}


// Should plan on when you want to deserialize each time -> every start up? or every click command or something
// But right now just 1 model only
//void Model::DeserializeGeoms(const std::string filePath)
//{
//    Geom tempGeom;
//    std::ifstream ifs(filePath, std::ios::binary);
//
//    ifs.read(reinterpret_cast<char*>(&tempGeom.mPosCompressionScale), sizeof(tempGeom.mPosCompressionScale));
//    ifs.read(reinterpret_cast<char*>(&tempGeom.mTexCompressionScale), sizeof(tempGeom.mTexCompressionScale));
//
//    size_t meshSize;
//    ifs.read(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));
//
//    for (int i = 0; i < meshSize; ++i)
//    {
//        Mesh tempMesh;
//
//        size_t vertSize;
//        ifs.read(reinterpret_cast<char*>(&vertSize), sizeof(vertSize));
//        ifs.read(reinterpret_cast<char*>(&tempMesh._vertices[0]), vertSize * sizeof(Vertex));
//
//        size_t indSize;
//        ifs.read(reinterpret_cast<char*>(&indSize), sizeof(indSize));
//        ifs.read(reinterpret_cast<char*>(&tempMesh._indices[0]), indSize * sizeof(unsigned int));
//
//        ifs.read(reinterpret_cast<char*>(&tempMesh.materialIndex), sizeof(tempMesh.materialIndex));
//        ifs.read(reinterpret_cast<char*>(&tempMesh.mPosCompressionOffset), sizeof(glm::vec3));
//        ifs.read(reinterpret_cast<char*>(&tempMesh.mTexCompressionOffset), sizeof(glm::vec2));
//
//        tempGeom.mMeshes.push_back(tempMesh);
//    }
//
//    size_t matSize;
//    ifs.read(reinterpret_cast<char*>(&matSize), sizeof(matSize));
//
//    for (int j = 0; j < matSize; ++j)
//    {
//        Material tempMat;
//
//        ifs.read(reinterpret_cast<char*>(&tempMat.Specular), sizeof(aiColor4D));
//        ifs.read(reinterpret_cast<char*>(&tempMat.Diffuse), sizeof(aiColor4D));
//        ifs.read(reinterpret_cast<char*>(&tempMat.Ambient), sizeof(aiColor4D));
//
//        size_t texSize;
//        ifs.read(reinterpret_cast<char*>(&texSize), sizeof(texSize));
//        if (texSize > 0)
//        {
//            ifs.read(reinterpret_cast<char*>(&tempMat.textures[0]), texSize * sizeof(Texture));
//        }
//
//        tempGeom._materials.push_back(tempMat);
//    }
//
//    this->totalGeoms.push_back(tempGeom); // Add this geom into our vector of geoms
//
//    ifs.close();
//}


void Model::debugAABB_init() // vao & shader
{
    // find min max points for each axis
    glm::vec3 minpt = _geom->_vertices[0].pos, maxpt = _geom->_vertices[0].pos;

    for (size_t i = 0; i < _geom->_vertices.size(); i++)
    {
        minpt.x = std::min(minpt.x, _geom->_vertices[i].pos.x);
        minpt.y = std::min(minpt.y, _geom->_vertices[i].pos.y);
        minpt.z = std::min(minpt.z, _geom->_vertices[i].pos.z);

        maxpt.x = std::max(maxpt.x, _geom->_vertices[i].pos.x);
        maxpt.y = std::max(maxpt.y, _geom->_vertices[i].pos.y);
        maxpt.z = std::max(maxpt.z, _geom->_vertices[i].pos.z);
    }

    pntAABB[0] = minpt;
    pntAABB[1] = glm::vec4(minpt.x, minpt.y, maxpt.z, 1.f);
    pntAABB[2] = glm::vec4(minpt.x, maxpt.y, maxpt.z, 1.f);
    pntAABB[3] = glm::vec4(minpt.x, maxpt.y, minpt.z, 1.f);

    pntAABB[4] = maxpt;
    pntAABB[5] = glm::vec4(maxpt.x, maxpt.y, minpt.z, 1.f);
    pntAABB[6] = glm::vec4(maxpt.x, minpt.y, minpt.z, 1.f);
    pntAABB[7] = glm::vec4(maxpt.x, minpt.y, maxpt.z, 1.f);
    
    int indice = 0;

    idxAABB.push_back(glm::ivec2(indice, indice + 1));
    idxAABB.push_back(glm::ivec2(indice + 1, indice + 2));
    idxAABB.push_back(glm::ivec2(indice + 2, indice + 3));
    idxAABB.push_back(glm::ivec2(indice + 3, indice));

    idxAABB.push_back(glm::ivec2(indice + 4, indice + 5));
    idxAABB.push_back(glm::ivec2(indice + 5, indice + 6));
    idxAABB.push_back(glm::ivec2(indice + 6, indice + 7));
    idxAABB.push_back(glm::ivec2(indice + 7, indice + 4));

    idxAABB.push_back(glm::ivec2(indice + 7, indice + 1));
    idxAABB.push_back(glm::ivec2(indice + 4, indice + 2));
    idxAABB.push_back(glm::ivec2(indice + 5, indice + 3));
    idxAABB.push_back(glm::ivec2(indice + 6, indice));


    // setup vao
    GLuint Pbuff; // point buffer
    GLuint Ibuff; // indice buffer

    glGenVertexArrays(1, &vaoidAABB);
    glGenBuffers(1, &Pbuff);
    glGenBuffers(1, &Ibuff);

    glBindVertexArray(vaoidAABB);
    glBindBuffer(GL_ARRAY_BUFFER, Pbuff);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 8, &pntAABB[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ibuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 2 * idxAABB.size(),
        &idxAABB[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind vao
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo


    // setup shader
    std::vector<std::pair<GLenum, std::string>> shdr_files;
    // Vertex Shader
    shdr_files.emplace_back(std::make_pair(
        GL_VERTEX_SHADER,
        "GAM300/Source/LapSupGraphics/abnb2.vert"));

    // Fragment Shader
    shdr_files.emplace_back(std::make_pair(
        GL_FRAGMENT_SHADER,
        "GAM300/Source/LapSupGraphics/debugAABB.frag"));

    std::cout << "DEBUG AABB SHADER\n";
    shaderAABB.CompileLinkValidate(shdr_files);
    std::cout << "\n\n";

    // if linking failed
    if (GL_FALSE == shaderAABB.IsLinked()) {
        std::stringstream sstr;
        sstr << "Unable to compile/link/validate shader programs\n";
        sstr << shaderAABB.GetLog() << "\n";
        std::cout << sstr.str();
        std::exit(EXIT_FAILURE);
    }
}

void Model::debugAABB_draw(glm::mat4 & SRT)
{
    shaderAABB.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getPerspMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getViewMatrix()));

    // Scuffed SRT
    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    glBindVertexArray(vaoidAABB);
    glDrawElements(GL_LINES, 2 * idxAABB.size(), GL_UNSIGNED_INT, 0);

    // unbind and free stuff
    glBindVertexArray(0);
    shaderAABB.UnUse();
}

void Model::lineinit()
{
    GLfloat vertices[] =
    {
        0.f,0.f,0.f
        ,1.f,1.f,1.f
    };

    GLushort elements[] =
    {
        0,1
    };
    unsigned int temp_vaoID;
    glGenVertexArrays(1, &temp_vaoID);
    glBindVertexArray(temp_vaoID);


    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint ibo_elements;
    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), &elements[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    vaoid = temp_vaoID;

    // setup shader
    std::vector<std::pair<GLenum, std::string>> shdr_files;
    // Vertex Shader
    shdr_files.emplace_back(std::make_pair(
        GL_VERTEX_SHADER,
        "GAM300/Source/LapSupGraphics/abnb2.vert"));

    // Fragment Shader
    shdr_files.emplace_back(std::make_pair(
        GL_FRAGMENT_SHADER,
        "GAM300/Source/LapSupGraphics/debugAABB.frag"));

    std::cout << "DEBUG AABB SHADER\n";
    shaderAABB.CompileLinkValidate(shdr_files);
    std::cout << "\n\n";

    // if linking failed
    if (GL_FALSE == shaderAABB.IsLinked()) {
        std::stringstream sstr;
        sstr << "Unable to compile/link/validate shader programs\n";
        sstr << shaderAABB.GetLog() << "\n";
        std::cout << sstr.str();
        std::exit(EXIT_FAILURE);
    }

}

void Model::debugline_draw(glm::mat4& SRT)
{
    shaderAABB.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getPerspMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.getViewMatrix()));

    // Scuffed SRT
    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    glBindVertexArray(vaoid);
    glDrawElements(GL_LINES,2, GL_UNSIGNED_SHORT, 0);

    // unbind and free stuff
    glBindVertexArray(0);
    shaderAABB.UnUse();
}