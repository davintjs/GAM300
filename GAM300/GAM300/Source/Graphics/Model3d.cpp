#include "Precompiled.h"
#include "Model3d.h"
#include "Camera.h"

extern Camera testCam;

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

    shader.CompileLinkValidate(shdr_files);
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
        glm::vec4(0.f, 0.f, -500.f, 1.f)
    );
    glm::mat4 SRT = translation_mat * rotation_mat * scaling_mat;
    


    shader.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shader.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(testCam.persp_projection));
    // Scuffed SRT
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shader.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(SRT));



    glBindVertexArray(vaoid);
    glDrawElements(prim, drawcount, GL_UNSIGNED_INT, nullptr);
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
