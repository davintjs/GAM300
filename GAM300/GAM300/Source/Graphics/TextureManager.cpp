/**************************************************************************************/
/*!
//    \file			TextureManager.cpp
//    \author(s) 	Euphrasia Theophelia Tan Ee Mun
//                 	Euan Lim Yiren
//
//    \date   	    15th September 2023
//    \brief		This file contains the functions that are used to create textures 
//                  from a dds file and retrieve textures from a GUID.
//
//    \Percentage   Theophelia 80%
//                  Euan 20%
//
//    Copyright (C) 2022 DigiPen Institute of Technology.
//    Reproduction or disclosure of this file or its contents without the
//    prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /**************************************************************************************/

#include "Precompiled.h"
#include "TextureManager.h"

void Texture_Manager::Init()
{
    std::cout << "TEXTURE MANAGER INIT\n";
	;
}


void Texture_Manager::Update(float dt)
{
	;
}

void Texture_Manager::AddTexture(char const* Filename, std::string GUID)
{
    GLuint temp{};

    // check if from skybox
    std::string searchWord = "skybox";
    const char* found = strstr(Filename, searchWord.c_str());

    if (found != nullptr) {
        // if skybox
        
        // if filename_top, we create skybox texture as the other .dds should have been loaded in
        searchWord = "_top";
        const char* found = strstr(Filename, searchWord.c_str());

        if (found != nullptr) {
        
            size_t length = found - Filename;
            char* subString = new char[length + 1];

            strncpy(subString, Filename, length);
            subString[length] = '\0';

            temp = CreateSkyboxTexture(subString);
            E_ASSERT(temp, "Skybox texture creation failed. Check if all textures necessary for skybox creation are named correctly.");

            delete[] subString;

            mTextureContainer.emplace(GUID, std::pair(Filename, temp));
        }
    }
    else {
        // if not skybox
        temp = CreateTexture(Filename);
        E_ASSERT(temp, "Texture creation failed.");
        mTextureContainer.emplace(GUID, std::pair(Filename, temp));
    }

}

/// Filename can be KTX or DDS files
GLuint Texture_Manager::CreateTexture(char const* Filename)
{
    gli::texture Texture = gli::load(Filename);
    if (Texture.empty())
        return 0;

    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
    GLenum Target = GL.translate(Texture.target());

    GLuint TextureName = 0; 
    glGenTextures(1, &TextureName);
    glBindTexture(Target, TextureName);
    glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

    glm::tvec3<GLsizei> const Extent(Texture.extent());
    GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

    switch (Texture.target())
    {
    case gli::TARGET_1D:
        glTexStorage1D(
            Target, static_cast<GLint>(Texture.levels()), Format.Internal, Extent.x);
        break;
    case gli::TARGET_1D_ARRAY:
    case gli::TARGET_2D:
    case gli::TARGET_CUBE:
        glTexStorage2D(
            Target, static_cast<GLint>(Texture.levels()), Format.Internal,
            Extent.x, Texture.target() == gli::TARGET_2D ? Extent.y : FaceTotal);
        break;
    case gli::TARGET_2D_ARRAY:
    case gli::TARGET_3D:
    case gli::TARGET_CUBE_ARRAY:
        glTexStorage3D(
            Target, static_cast<GLint>(Texture.levels()), Format.Internal,
            Extent.x, Extent.y,
            Texture.target() == gli::TARGET_3D ? Extent.z : FaceTotal);
        break;
    default:
        assert(0);
        break;
    }

    for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
        for (std::size_t Face = 0; Face < Texture.faces(); ++Face)
            for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
            {
                GLsizei const LayerGL = static_cast<GLsizei>(Layer);
                glm::tvec3<GLsizei> Extent(Texture.extent(Level));
                Target = gli::is_target_cube(Texture.target())
                    ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
                    : Target;

                switch (Texture.target())
                {
                case gli::TARGET_1D:
                    if (gli::is_compressed(Texture.format()))
                        glCompressedTexSubImage1D(
                            Target, static_cast<GLint>(Level), 0, Extent.x,
                            Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                            Texture.data(Layer, Face, Level));
                    else
                        glTexSubImage1D(
                            Target, static_cast<GLint>(Level), 0, Extent.x,
                            Format.External, Format.Type,
                            Texture.data(Layer, Face, Level));
                    break;
                case gli::TARGET_1D_ARRAY:
                case gli::TARGET_2D:
                case gli::TARGET_CUBE:
                    if (gli::is_compressed(Texture.format()))
                        glCompressedTexSubImage2D(
                            Target, static_cast<GLint>(Level),
                            0, 0,
                            Extent.x,
                            Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                            Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                            Texture.data(Layer, Face, Level));
                    else
                        glTexSubImage2D(
                            Target, static_cast<GLint>(Level),
                            0, 0,
                            Extent.x,
                            Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                            Format.External, Format.Type,
                            Texture.data(Layer, Face, Level));
                    break;
                case gli::TARGET_2D_ARRAY:
                case gli::TARGET_3D:
                case gli::TARGET_CUBE_ARRAY:

                    if (gli::is_compressed(Texture.format()))
                        glCompressedTexSubImage3D(
                            Target, static_cast<GLint>(Level),
                            0, 0, 0,
                            Extent.x, Extent.y,
                            Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                            Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                            Texture.data(Layer, Face, Level));
                    else
                        glTexSubImage3D(
                            Target, static_cast<GLint>(Level),
                            0, 0, 0,
                            Extent.x, Extent.y,
                            Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                            Format.External, Format.Type,
                            Texture.data(Layer, Face, Level));
                    break;
                default: assert(0); break;
                }
            }
    return TextureName;
}

GLuint Texture_Manager::CreateSkyboxTexture(char const* Filename)
{
    /*std::cout << "filename is : " << Filename << "\n";*/
    std::string FilenameStr(Filename);

    std::string left = FilenameStr + "_left.dds";
    std::string back = FilenameStr + "_back.dds";
    std::string front = FilenameStr + "_front.dds";
    std::string right = FilenameStr + "_right.dds";
    std::string top = FilenameStr + "_top.dds";
    std::string bottom = FilenameStr + "_bottom.dds";

    std::vector<std::string> faces
    {
        right,left,top,bottom,front,back
    };

    GLuint Skybox_Tex = 0;
    glGenTextures(1, &Skybox_Tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Skybox_Tex);

    int width, height, nrChannels;
    unsigned int err = 0;

    for (size_t i = 0; i < faces.size(); i++)
    {
        gli::texture Texture = gli::load(faces[i]);

        glCompressedTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
            Texture.extent().x,
            Texture.extent().y,
            0,
            GLsizei(Texture.size()),
            Texture.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return Skybox_Tex;
}

GLuint Texture_Manager::GetTexture(std::string GUID)
{
    if ((mTextureContainer.find(GUID) != mTextureContainer.end())) {
        return mTextureContainer.find(GUID)->second.second;
    }

    return UINT_MAX;
}

void Texture_Manager::Exit()
{
	mTextureContainer.clear();
}
