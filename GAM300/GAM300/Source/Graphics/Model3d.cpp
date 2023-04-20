#include "Precompiled.h"
#include "Model3d.h"


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
        "Source/LapSupGraphics/abnb2.vert"));
    //"Assets/Shaders/OrionVertShader.vert"));

// Fragment Shader
    shdr_files.emplace_back(std::make_pair(
        GL_FRAGMENT_SHADER,
        "Source/LapSupGraphics/abnb2.frag"));
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
    shader.Use();
    glBindVertexArray(vaoid);
    glDrawElements(prim, drawcount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    shader.UnUse();
}