#include "Precompiled.h"
#include "MeshManager.h"
#include "GraphicsSystem.h"

extern trans_mats SRT_Buffers[50];
extern std::map<std::string, InstanceProperties> properties;
// extern InstanceProperties properties[EntityRenderLimit];
extern std::vector <Materials> temp_MaterialContainer;


void MESH_Manager::Init()
{
    // Create all the hardcoded meshes here : Cube , (Maybe circle)?
	CreateInstanceCube();

}

void MESH_Manager::Update(float dt)
{
	// Empty by design
}

void MESH_Manager::Exit()
{
	// Loop through the container, clear all the vaoid, vbo and draw count

	mContainer.clear();
}


void MESH_Manager::GetGeomFromFiles(const std::string& filePath, const std::string& fileName)
{
    GeomImported newGeom(std::move(DeserializeGeoms(filePath)));

    std::cout << "I have Materials : " << newGeom._materials.size() << 
        "from " << filePath << "\n";

    for (int i = 0; i < newGeom._materials.size(); ++i)
    {
        std::cout << "Ambience : " << newGeom._materials[i].Ambient.r << "\n";
        std::cout << "Ambience : " << newGeom._materials[i].Ambient.g << "\n";
        std::cout << "Ambience : " << newGeom._materials[i].Ambient.b << "\n";
        std::cout << "Ambience : " << newGeom._materials[i].Ambient.a << "\n";
        std::cout << "\n\n";
        std::cout << "Diffuse : " << newGeom._materials[i].Diffuse.r << "\n";
        std::cout << "Diffuse : " << newGeom._materials[i].Diffuse.g << "\n";
        std::cout << "Diffuse : " << newGeom._materials[i].Diffuse.b << "\n";
        std::cout << "Diffuse : " << newGeom._materials[i].Diffuse.a << "\n";
        std::cout << "\n\n";

        std::cout << "Specular : " << newGeom._materials[i].Specular.r << "\n";
        std::cout << "Specular : " << newGeom._materials[i].Specular.g << "\n";
        std::cout << "Specular : " << newGeom._materials[i].Specular.b << "\n";
        std::cout << "Specular : " << newGeom._materials[i].Specular.a << "\n";
        std::cout << "\n\n";
        Materials temporary;
        temporary.Albedo = glm::vec4(1.f, 1.f, 1.f, 1.f);

        temporary.Diffuse = glm::vec4(newGeom._materials[i].Diffuse.r, newGeom._materials[i].Diffuse.g,
            newGeom._materials[i].Diffuse.b, newGeom._materials[i].Diffuse.a);
        
        temporary.Specular = glm::vec4(newGeom._materials[i].Specular.r, newGeom._materials[i].Specular.g,
            newGeom._materials[i].Specular.b, newGeom._materials[i].Specular.a);

        temporary.Ambient = glm::vec4(newGeom._materials[i].Ambient.r, newGeom._materials[i].Ambient.g,
            newGeom._materials[i].Ambient.b, newGeom._materials[i].Ambient.a);

        temp_MaterialContainer.push_back(temporary);
    }

    Mesh newMesh;
    newMesh.index = mContainer.size();
    glm::vec3 min(FLT_MAX);
    glm::vec3 max(FLT_MIN);
    for (int i = 0; i < newGeom.mMeshes.size(); ++i)
    {
        std::cout << "ouchie\n";
        for (int k = 0; k < newGeom.mMeshes[i]._vertices.size(); ++k)
        {
            glm::vec3 pos = newGeom.mMeshes[i]._vertices[k].pos;

            min.x = std::min(pos.x, min.x);
            min.y = std::min(pos.y, min.y);
            min.z = std::min(pos.z, min.z);

            max.x = std::min(pos.x, max.x);
            max.y = std::min(pos.y, max.y);
            max.z = std::min(pos.z, max.z);
        }
        /*totalvertices += totalGeoms[0].mMeshes[i]._vertices.size();
        totalindices += totalGeoms[0].mMeshes[i]._indices.size();
        std::cout << "total vertices count: " << totalvertices << "\n";
        std::cout << "total indices count: " << totalindices << "\n";*/


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

        glBufferData(GL_ARRAY_BUFFER, newGeom.mMeshes[i]._vertices.size() * sizeof(Vertex), &newGeom.mMeshes[i]._vertices[0], GL_STATIC_DRAW);

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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, newGeom.mMeshes[i]._indices.size() * sizeof(unsigned int), &newGeom.mMeshes[i]._indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0); // unbind vao
        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo

        InstanceProperties tempProp;
        tempProp.VAO = VAO;
        tempProp.drawCount = newGeom.mMeshes[i]._indices.size();
        std::string newName = fileName;
        std::map<std::string, InstanceProperties>::iterator it;
        it = properties.find(newName);
        while (it != properties.end()) {
            newName += ('0' + i);
            it = properties.find(newName);
        }
        properties.emplace(std::pair<std::string, InstanceProperties>(newName, tempProp));

        newMesh.prim = GL_TRIANGLES;
        newMesh.Vaoids.push_back(VAO);
        newMesh.Vboids.push_back(VBO);
        newMesh.Drawcounts.push_back(newGeom.mMeshes[i]._indices.size());

        newMesh.SRT_Buffer_Index.push_back(InstanceSetup(properties[newName]));
    }
    newMesh.vertices_min = min;
    newMesh.vertices_max = max;

    mContainer.emplace(fileName, newMesh);
}

//Mesh& DereferencingMesh(std::string mesh_Name);// Either Geom or Vaoid





// PRIVATE FUNCTIONS

GeomImported MESH_Manager::DeserializeGeoms(const std::string filePath)
{
    GeomImported tempGeom;
    std::ifstream ifs(filePath, std::ios::binary);

    //ifs.read(reinterpret_cast<char*>(&tempGeom.mPosCompressionScale), sizeof(tempGeom.mPosCompressionScale));
    //ifs.read(reinterpret_cast<char*>(&tempGeom.mTexCompressionScale), sizeof(tempGeom.mTexCompressionScale));

    size_t meshSize;
    ifs.read(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));

    for (int i = 0; i < meshSize; ++i)
    {
        Geom_Mesh tempMesh;

        size_t vertSize;
        ifs.read(reinterpret_cast<char*>(&vertSize), sizeof(vertSize));
        tempMesh._vertices.resize(vertSize);
        ifs.read(reinterpret_cast<char*>(&tempMesh._vertices[0]), vertSize * sizeof(Vertex));

        size_t indSize;
        ifs.read(reinterpret_cast<char*>(&indSize), sizeof(indSize));
        tempMesh._indices.resize(indSize);
        ifs.read(reinterpret_cast<char*>(&tempMesh._indices[0]), indSize * sizeof(unsigned int));

        ifs.read(reinterpret_cast<char*>(&tempMesh.materialIndex), sizeof(tempMesh.materialIndex));
        //ifs.read(reinterpret_cast<char*>(&tempMesh.mPosCompressionOffset), sizeof(glm::vec3));
        //ifs.read(reinterpret_cast<char*>(&tempMesh.mTexCompressionOffset), sizeof(glm::vec2));

        tempGeom.mMeshes.push_back(tempMesh);
    }

    size_t matSize;
    ifs.read(reinterpret_cast<char*>(&matSize), sizeof(matSize));

    for (int j = 0; j < matSize; ++j)
    {
        Material tempMat;

        ifs.read(reinterpret_cast<char*>(&tempMat.Specular), sizeof(aiColor4D));
        ifs.read(reinterpret_cast<char*>(&tempMat.Diffuse), sizeof(aiColor4D));
        ifs.read(reinterpret_cast<char*>(&tempMat.Ambient), sizeof(aiColor4D));

        size_t texSize;
        ifs.read(reinterpret_cast<char*>(&texSize), sizeof(texSize));
        if (texSize > 0)
        {
            tempMat.textures.resize(texSize);
            ifs.read(reinterpret_cast<char*>(&tempMat.textures[0]), texSize * sizeof(Texture));
        }

        tempGeom._materials.push_back(tempMat);
    }

    

    ifs.close();
    return tempGeom;
}

void MESH_Manager::CreateInstanceCube()
{
    Mesh newMesh;
    newMesh.index = mContainer.size();

    // positions            // Normals              // Tangents             // Texture Coords   // Colors
    //float vertices[] = {
    //   -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         1.0f, 0.0f, 0.0f, 1.0f, // Vertex 0
    //    1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 0.0f, 1.0f, // Vertex 1
    //    1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 2
    //   -1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         1.0f, 1.0f, 0.0f, 1.0f, // Vertex 3
    //   -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 4
    //    1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 1.0f, 1.0f, // Vertex 5
    //    1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         1.0f, 0.0f, 1.0f, 1.0f, // Vertex 6
    //   -1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         0.5f, 0.5f, 0.5f, 1.0f,  // Vertex 7

    //};

    //int indices[] = {
    //    0, 1, 2, 2, 3, 0,  // Front face
    //    4, 5, 6, 6, 7, 4,  // Back face
    //    1, 5, 6, 6, 2, 1,  // Right face
    //    0, 4, 7, 7, 3, 0,  // Left face
    //    3, 2, 6, 6, 7, 3,  // Top face
    //    0, 1, 5, 5, 4, 0   // Bottom face
    //};


    GLfloat vertices[] = {
        // Positions           Normals            Tangents          Texture Coords     Colors (RGB)
        // Front face
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,      1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f,      1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,      1.0f, 0.0f, 0.0f,

        // Back face
        -0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,      0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,      0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,      0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,      0.0f, 1.0f, 0.0f,

        // Left face
        -0.5f, 0.5f, 0.5f,     -1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,      0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,    -1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,      0.0f, 0.0f, 1.0f,

        // Right face
         0.5f, 0.5f, 0.5f,     1.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,  0.0f, 0.0f,      1.0f, 1.0f, 0.0f,
         0.5f, 0.5f, -0.5f,    1.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,  1.0f, 0.0f,      1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,  1.0f, 1.0f,      1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,  0.0f, 1.0f,      1.0f, 1.0f, 0.0f,

         // Top face
         -0.5f, 0.5f, -0.5f,    0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,  0.0f, 0.0f,      0.0f, 1.0f, 1.0f,
          0.5f, 0.5f, -0.5f,    0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,  1.0f, 0.0f,      0.0f, 1.0f, 1.0f,
          0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,  1.0f, 1.0f,      0.0f, 1.0f, 1.0f,
         -0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,  0.0f, 1.0f,      0.0f, 1.0f, 1.0f,

         // Bottom face
         -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,    1.0f, 0.0f, 0.0f,  0.0f, 0.0f,      1.0f, 1.0f, 0.0f,
          0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,    1.0f, 0.0f, 0.0f,  1.0f, 0.0f,      1.0f, 1.0f, 0.0f,
          0.5f, -0.5f, 0.5f,    0.0f, -1.0f, 0.0f,    1.0f, 0.0f, 0.0f,  1.0f, 1.0f,      1.0f, 1.0f, 0.0f,
         -0.5f, -0.5f, 0.5f,    0.0f, -1.0f, 0.0f,    1.0f, 0.0f, 0.0f,  0.0f, 1.0f,      1.0f, 1.0f, 0.0f
    };

    GLuint indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };
    newMesh.vertices_min = glm::vec3(-1.f, -1.f, -1.f);
    newMesh.vertices_max = glm::vec3(1.f, 1.f, 1.f);

    // first, configure the cube's VAO (and VBO)
    //unsigned int VBO, cubeVAO;

    GLuint vaoid;
    GLuint vboid;
    GLuint ebo;
    glGenVertexArrays(1, &vaoid);
    glGenBuffers(1, &vboid);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vboid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(vaoid);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Tangent attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Texture coord
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // color coord
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind vao
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo

    InstanceProperties tempProp;
    tempProp.VAO = vaoid;
    tempProp.drawCount = 36;
    properties.emplace(std::pair<std::string, InstanceProperties>(std::string("Cube"),tempProp));
    newMesh.Vaoids.push_back(vaoid);
    newMesh.Vboids.push_back(vboid);
    newMesh.prim = GL_TRIANGLES;
    newMesh.Drawcounts.push_back(36);
    newMesh.SRT_Buffer_Index.push_back(InstanceSetup(properties["Cube"]));

    mContainer.emplace(std::string("Cube"), newMesh);
}

unsigned int  MESH_Manager::InstanceSetup(InstanceProperties& prop) {
    prop.entitySRTbuffer;
    glGenBuffers(1, &prop.entitySRTbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::mat4), &(prop.entitySRT[0]), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //entitySRTBuffer
    glBindVertexArray(prop.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glBindVertexArray(0);

    return prop.entitySRTbuffer;
}



