#include "Precompiled.h"
#include "MeshManager.h"
#include "GraphicsSystem.h"

extern std::map<std::string, InstanceProperties> properties;
// extern InstanceProperties properties[EntityRenderLimit];
//extern std::vector <Materials> temp_MaterialContainer;

// v scuffed ik i am sorry ;v;
extern std::vector <glm::vec4> temp_AlbedoContainer;
extern std::vector <glm::vec4> temp_SpecularContainer;
extern std::vector <glm::vec4> temp_DiffuseContainer;
extern std::vector <glm::vec4> temp_AmbientContainer;
extern std::vector <float> temp_ShininessContainer;


void MESH_Manager::Init()
{
    // Create all the hardcoded meshes here : Cube , (Maybe circle)?
	CreateInstanceCube();
    CreateInstanceSphere();
    
    CreateInstanceLine();
}

void MESH_Manager::Update(float dt)
{
    UNREFERENCED_PARAMETER(dt);
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

    /*std::cout << "I have Materials : " << newGeom._materials.size() << 
        "from " << filePath << "\n";*/

    for (int i = 0; i < newGeom._materials.size(); ++i)
    {
        /*std::cout << "Ambience : " << newGeom._materials[i].Ambient.r << "\n";
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
        std::cout << "\n\n";*/

        //Materials temporary;
        //temporary.Albedo = glm::vec4(1.f, 1.f, 1.f, 1.f);

        //temporary.Diffuse = glm::vec4(newGeom._materials[i].Diffuse.r, newGeom._materials[i].Diffuse.g,
        //    newGeom._materials[i].Diffuse.b, newGeom._materials[i].Diffuse.a);
        //
        //temporary.Specular = glm::vec4(newGeom._materials[i].Specular.r, newGeom._materials[i].Specular.g,
        //    newGeom._materials[i].Specular.b, newGeom._materials[i].Specular.a);

        //temporary.Ambient = glm::vec4(newGeom._materials[i].Ambient.r, newGeom._materials[i].Ambient.g,
        //    newGeom._materials[i].Ambient.b, newGeom._materials[i].Ambient.a);

        //temp_MaterialContainer.push_back(temporary);

        temp_AlbedoContainer.push_back(glm::vec4(1.f, 1.f, 1.f, 1.f));
        temp_DiffuseContainer.push_back(glm::vec4(newGeom._materials[i].Diffuse.r, newGeom._materials[i].Diffuse.g,
            newGeom._materials[i].Diffuse.b, newGeom._materials[i].Diffuse.a));
        temp_SpecularContainer.push_back(glm::vec4(newGeom._materials[i].Specular.r, newGeom._materials[i].Specular.g,
            newGeom._materials[i].Specular.b, newGeom._materials[i].Specular.a));
        temp_AmbientContainer.push_back(glm::vec4(newGeom._materials[i].Ambient.r, newGeom._materials[i].Ambient.g,
            newGeom._materials[i].Ambient.b, newGeom._materials[i].Ambient.a));
        temp_ShininessContainer.push_back(0.f);
    }

    Mesh newMesh;
    newMesh.index = mContainer.size();
    glm::vec3 min(FLT_MAX);
    glm::vec3 max(FLT_MIN);
    for (int i = 0; i < newGeom.mMeshes.size(); ++i)
    {
        //std::cout << "ouchie\n";
        for (int k = 0; k < newGeom.mMeshes[i]._vertices.size(); ++k)
        {
            glm::vec3 pos = newGeom.mMeshes[i]._vertices[k].pos;

            min.x = std::min(pos.x, min.x);
            min.y = std::min(pos.y, min.y);
            min.z = std::min(pos.z, min.z);

            max.x = std::max(pos.x, max.x);
            max.y = std::max(pos.y, max.y);
            max.z = std::max(pos.z, max.z);
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

        glBufferData(GL_ARRAY_BUFFER, newGeom.mMeshes[i]._vertices.size() * sizeof(gVertex), &newGeom.mMeshes[i]._vertices[0], GL_STATIC_DRAW);

        // set the vertex attributes to tell vert shader pos, uv, normal etc.
        // note offsetof uses the struct Vertex as first argument, and member name as second.
        // it returns the offset to the member name :)
        glEnableVertexAttribArray(0); //pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gVertex), (void*)0);

        glEnableVertexAttribArray(1); // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(gVertex), (void*)offsetof(gVertex, normal));

        glEnableVertexAttribArray(2); //tangent
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(gVertex), (void*)offsetof(gVertex, tangent));

        glEnableVertexAttribArray(3); // tex (uv coords)
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(gVertex), (void*)offsetof(gVertex, tex));

        glEnableVertexAttribArray(4); // color
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(gVertex), (void*)offsetof(gVertex, color));


        // bind indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, newGeom.mMeshes[i]._indices.size() * sizeof(unsigned int), &newGeom.mMeshes[i]._indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0); // unbind vao
        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo

        InstanceProperties tempProp;
        tempProp.VAO = VAO;
        tempProp.drawCount = newGeom.mMeshes[i]._indices.size();
        tempProp.drawType = GL_TRIANGLES;
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
        newMesh.Drawcounts.push_back((GLuint)(newGeom.mMeshes[i]._indices.size()));

        newMesh.SRT_Buffer_Index.push_back(InstanceSetup_PBR(properties[newName]));
    }

    newMesh.vertices_min = min;
    newMesh.vertices_max = max;
    debugAABB_setup(newMesh.vertices_min, newMesh.vertices_max, properties[fileName]);

    mContainer.emplace(fileName, newMesh);
}

//Mesh& DereferencingMesh(std::string mesh_Name);// Either Geom or Vaoid


// PRIVATE FUNCTIONS

GeomImported MESH_Manager::DeserializeGeoms(const std::string filePath)
{
    GeomImported tempGeom;
    std::ifstream ifs(filePath, std::ios::binary);

    size_t meshSize;
    ifs.read(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));

    for (int i = 0; i < meshSize; ++i)
    {
        gMesh tempMesh;

        // Vertices
        std::vector<Vertex> tempVerts;
        size_t vertSize;
        ifs.read(reinterpret_cast<char*>(&vertSize), sizeof(vertSize));
        tempVerts.resize(vertSize);
        ifs.read(reinterpret_cast<char*>(&tempVerts[0]), vertSize * sizeof(Vertex));

        // Indices
        size_t indSize;
        ifs.read(reinterpret_cast<char*>(&indSize), sizeof(indSize));
        tempMesh._indices.resize(indSize);
        ifs.read(reinterpret_cast<char*>(&tempMesh._indices[0]), indSize * sizeof(unsigned int));

        ifs.read(reinterpret_cast<char*>(&tempMesh.materialIndex), sizeof(tempMesh.materialIndex)); // Material Index

        ifs.read(reinterpret_cast<char*>(&tempMesh.mPosCompressionScale), sizeof(glm::vec3)); // Position Scale
        ifs.read(reinterpret_cast<char*>(&tempMesh.mTexCompressionScale), sizeof(glm::vec2)); // Texture Scale

        ifs.read(reinterpret_cast<char*>(&tempMesh.mPosCompressionOffset), sizeof(glm::vec3)); // Position Offset
        ifs.read(reinterpret_cast<char*>(&tempMesh.mTexCompressionOffset), sizeof(glm::vec2)); // Texture Offset

        tempMesh._vertices.resize(vertSize); // Resize our vertices vector
        DecompressVertices(tempMesh._vertices, tempVerts, tempMesh.mPosCompressionScale, tempMesh.mTexCompressionScale, tempMesh.mPosCompressionOffset, tempMesh.mTexCompressionOffset); // Converts Vertex to gVertex

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

        //size_t texSize;
        //ifs.read(reinterpret_cast<char*>(&texSize), sizeof(texSize));
        //if (texSize > 0)
        //{
        //    tempMat.textures.resize(texSize);
        //    ifs.read(reinterpret_cast<char*>(&tempMat.textures[0]), texSize * sizeof(Texture));
        //}

        tempGeom._materials.push_back(tempMat);
    }

    ifs.close();

    return tempGeom;
}

void MESH_Manager::DecompressVertices(std::vector<gVertex>& mMeshVertices,
    const std::vector<Vertex>& oVertices,
    const glm::vec3& mPosCompressScale,
    const glm::vec2& mTexCompressScale,
    const glm::vec3& mPosOffset,
    const glm::vec2& mTexOffset)
{
    E_ASSERT(mMeshVertices.size() == oVertices.size(), "Both vertices vector sizes not equal for decompressing.");

    for (int i = 0; i < mMeshVertices.size(); ++i)
    {
        // Position
        mMeshVertices[i].pos.x = (oVertices[i].posX >= 0 ? static_cast<float>(oVertices[i].posX) / 0x7FFF : static_cast<float>(oVertices[i].posX) / 0x8000) * mPosCompressScale.x + mPosOffset.x;
        mMeshVertices[i].pos.y = (oVertices[i].posY >= 0 ? static_cast<float>(oVertices[i].posY) / 0x7FFF : static_cast<float>(oVertices[i].posY) / 0x8000) * mPosCompressScale.y + mPosOffset.y;
        mMeshVertices[i].pos.z = (oVertices[i].posZ >= 0 ? static_cast<float>(oVertices[i].posZ) / 0x7FFF : static_cast<float>(oVertices[i].posZ) / 0x8000) * mPosCompressScale.z + mPosOffset.z;

        // Texture
        mMeshVertices[i].tex.x = (oVertices[i].texU >= 0 ? static_cast<float>(oVertices[i].texU) / 0x7FFF : static_cast<float>(oVertices[i].texU) / 0x8000) * mTexCompressScale.x + mTexOffset.x;
        mMeshVertices[i].tex.y = (oVertices[i].texV >= 0 ? static_cast<float>(oVertices[i].texV) / 0x7FFF : static_cast<float>(oVertices[i].texV) / 0x8000) * mTexCompressScale.y + mTexOffset.y;

        // Normal
        mMeshVertices[i].normal.x = (oVertices[i].normX >= 0 ? static_cast<float>(oVertices[i].normX) / 0x7FFF : static_cast<float>(oVertices[i].normX) / 0x8000);
        mMeshVertices[i].normal.y = (oVertices[i].normY >= 0 ? static_cast<float>(oVertices[i].normY) / 0x7FFF : static_cast<float>(oVertices[i].normY) / 0x8000);
        mMeshVertices[i].normal.z = (oVertices[i].normZ >= 0 ? static_cast<float>(oVertices[i].normZ) / 0x7FFF : static_cast<float>(oVertices[i].normZ) / 0x8000);

        // Tangent
        mMeshVertices[i].tangent.x = (oVertices[i].tanX >= 0 ? static_cast<float>(oVertices[i].tanX) / 0x7FFF : static_cast<float>(oVertices[i].tanX) / 0x8000);
        mMeshVertices[i].tangent.y = (oVertices[i].tanY >= 0 ? static_cast<float>(oVertices[i].tanX) / 0x7FFF : static_cast<float>(oVertices[i].tanY) / 0x8000);
        mMeshVertices[i].tangent.z = (oVertices[i].tanZ >= 0 ? static_cast<float>(oVertices[i].tanZ) / 0x7FFF : static_cast<float>(oVertices[i].tanZ) / 0x8000);

        // Color
        mMeshVertices[i].color.r = oVertices[i].colorR;
        mMeshVertices[i].color.g = oVertices[i].colorG;
        mMeshVertices[i].color.b = oVertices[i].colorB;
        mMeshVertices[i].color.a = oVertices[i].colorA;
    }
}

void MESH_Manager::CreateInstanceCube()
{
    Mesh newMesh;
    newMesh.index = (unsigned int)mContainer.size();

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
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,      1.0f, 0.0f, 0.0f, 1.f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,      1.0f, 0.0f, 0.0f, 1.f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f,      1.0f, 0.0f, 0.0f, 1.f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,      1.0f, 0.0f, 0.0f, 1.f,

        // Back face
        -0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,      0.0f, 1.0f, 0.0f, 1.f,
         0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,      0.0f, 1.0f, 0.0f, 1.f,
         0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,      0.0f, 1.0f, 0.0f, 1.f,
        -0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,      0.0f, 1.0f, 0.0f, 1.f,

        // Left face
        -0.5f, 0.5f, 0.5f,     -1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,      0.0f, 0.0f, 1.0f, 1.f,
        -0.5f, 0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      0.0f, 0.0f, 1.0f, 1.f,
        -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      0.0f, 0.0f, 1.0f, 1.f,
        -0.5f, -0.5f, 0.5f,    -1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,      0.0f, 0.0f, 1.0f, 1.f,

        // Right face
         0.5f, 0.5f, 0.5f,     1.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,  0.0f, 0.0f,      1.0f, 1.0f, 0.0f, 1.f,
         0.5f, 0.5f, -0.5f,    1.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,  1.0f, 0.0f,      1.0f, 1.0f, 0.0f, 1.f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,  1.0f, 1.0f,      1.0f, 1.0f, 0.0f, 1.f,
         0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,  0.0f, 1.0f,      1.0f, 1.0f, 0.0f, 1.f,

         // Top face
         -0.5f, 0.5f, -0.5f,    0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,  0.0f, 0.0f,      0.0f, 1.0f, 1.0f, 1.f,
          0.5f, 0.5f, -0.5f,    0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,  1.0f, 0.0f,      0.0f, 1.0f, 1.0f, 1.f,
          0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,  1.0f, 1.0f,      0.0f, 1.0f, 1.0f, 1.f,
         -0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,  0.0f, 1.0f,      0.0f, 1.0f, 1.0f, 1.f,

         // Bottom face
         -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,    1.0f, 0.0f, 0.0f,  0.0f, 0.0f,      1.0f, 1.0f, 0.0f, 1.f,
          0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,    1.0f, 0.0f, 0.0f,  1.0f, 0.0f,      1.0f, 1.0f, 0.0f, 1.f,
          0.5f, -0.5f, 0.5f,    0.0f, -1.0f, 0.0f,    1.0f, 0.0f, 0.0f,  1.0f, 1.0f,      1.0f, 1.0f, 0.0f, 1.f,
         -0.5f, -0.5f, 0.5f,    0.0f, -1.0f, 0.0f,    1.0f, 0.0f, 0.0f,  0.0f, 1.0f,      1.0f, 1.0f, 0.0f, 1.f
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

    newMesh.vertices_min = glm::vec3(-0.5f, -0.5f, -0.5f);
    newMesh.vertices_max = glm::vec3(0.5f, 0.5f, 0.5f);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Tangent attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Texture coord
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // color coord
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind vao
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo

    InstanceProperties tempProp;
    tempProp.VAO = vaoid;
    tempProp.drawCount = 36;
    tempProp.drawType = GL_TRIANGLES;

    properties.emplace(std::pair<std::string, InstanceProperties>(std::string("Cube"),tempProp));
    newMesh.Vaoids.push_back(vaoid);
    newMesh.Vboids.push_back(vboid);
    newMesh.prim = GL_TRIANGLES;
    newMesh.Drawcounts.push_back(36);
    newMesh.SRT_Buffer_Index.push_back(InstanceSetup_PBR(properties["Cube"]));
    debugAABB_setup(newMesh.vertices_min, newMesh.vertices_max, properties["Cube"]);

    mContainer.emplace(std::string("Cube"), newMesh);
}


void MESH_Manager::CreateInstanceSphere()
{
    Mesh newMesh;
    newMesh.index = (unsigned int)mContainer.size();

    GLuint vaoid;
    GLuint vboid;
    GLuint ebo;


    glGenVertexArrays(1, &vaoid);
    glGenBuffers(1, &vboid);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;

    glm::vec3 min(FLT_MAX);
    glm::vec3 max(FLT_MIN);

    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI) * 15.f;
            float yPos = std::cos(ySegment * PI) * 15.f;
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI) * 15.f;

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
            min.x = std::min(xPos, min.x);
            min.y = std::min(yPos, min.y);
            min.z = std::min(zPos, min.z);

            max.x = std::max(xPos, max.x);
            max.y = std::max(yPos, max.y);
            max.z = std::max(zPos, max.z);

        }
    }
    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }
    glBindVertexArray(vaoid);
    glBindBuffer(GL_ARRAY_BUFFER, vboid);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    unsigned int stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    InstanceProperties tempProp;
    tempProp.drawType = GL_TRIANGLE_STRIP;
    tempProp.VAO = vaoid;
    tempProp.drawCount = (unsigned int)(indices.size()) ;
    properties.emplace(std::pair<std::string, InstanceProperties>(std::string("Sphere"), tempProp));
    newMesh.Vaoids.push_back(vaoid);
    newMesh.Vboids.push_back(vboid);
    newMesh.prim = GL_TRIANGLE_STRIP;
    newMesh.Drawcounts.push_back((unsigned int)(indices.size()));
    newMesh.SRT_Buffer_Index.push_back(InstanceSetup_PBR(properties["Sphere"]));

    newMesh.vertices_min = min;
    newMesh.vertices_max = max;

    debugAABB_setup(newMesh.vertices_min, newMesh.vertices_max, properties["Sphere"]);
    mContainer.emplace(std::string("Sphere"), newMesh);


}


// THIS IS THE PREVIOUS MATERIAL STUFFS -> BLINN PHONG THINGS
unsigned int  MESH_Manager::InstanceSetup(InstanceProperties& prop) {


    // SRT Buffer set up
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


    // Albedo Buffer Setup
    prop.AlbedoBuffer;
    glGenBuffers(1, &prop.AlbedoBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::vec4), &(prop.Albedo[0]), GL_STATIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribDivisor(10, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Specular Buffer Setup
    prop.SpecularBuffer;
    glGenBuffers(1, &prop.SpecularBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.SpecularBuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::vec4), &(prop.Specular[0]), GL_STATIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribDivisor(11, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Diffuse Buffer Setup
    prop.DiffuseBuffer;
    glGenBuffers(1, &prop.DiffuseBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.DiffuseBuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::vec4), &(prop.Diffuse[0]), GL_STATIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(12);
    glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribDivisor(12, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Ambient Buffer SetupF
    prop.AmbientBuffer;
    glGenBuffers(1, &prop.AmbientBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.AmbientBuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::vec4), &(prop.Ambient[0]), GL_STATIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(13);
    glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribDivisor(13, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Shininess Buffer Setup
    prop.ShininessBuffer;
    glGenBuffers(1, &prop.ShininessBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.ShininessBuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(float), &(prop.Shininess[0]), GL_STATIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(14);
    glVertexAttribPointer(14, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(14, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    prop.textureIndexBuffer;
    glGenBuffers(1, &prop.textureIndexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.textureIndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::vec2), &(prop.textureIndex[0]), GL_STATIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(15);
    glVertexAttribPointer(15, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glVertexAttribDivisor(15, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //// Material Buffer Setup
    //prop.entityMATbuffer;
    //glBindVertexArray(prop.VAO);

    //glGenBuffers(1, &prop.entityMATbuffer);
    //glBindBuffer(GL_ARRAY_BUFFER, prop.entityMATbuffer);

    //glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit *sizeof(Materials), &(prop.entityMAT[0]), GL_STATIC_DRAW);
    //// Albedo
    //glEnableVertexAttribArray(10);

    //glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)0);
    //// Specular
    //glEnableVertexAttribArray(11);

    //glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(sizeof(glm::vec4)));
    //// Diffuse
    //glEnableVertexAttribArray(12);

    //glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(2 * sizeof(glm::vec4)));
    //// Ambient
    //glEnableVertexAttribArray(13);

    //glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(3 * sizeof(glm::vec4)));
    //// Shininess
    //glEnableVertexAttribArray(14);

    //glVertexAttribPointer(14, 1, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(4 * sizeof(glm::vec4)));
    //glVertexAttribDivisor(10, 1);
    //glVertexAttribDivisor(11, 1);
    //glVertexAttribDivisor(12, 1);
    //glVertexAttribDivisor(13, 1);
    //glVertexAttribDivisor(14, 1);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);



    return prop.entitySRTbuffer;
}

// THIS IS THE PREVIOUS MATERIAL STUFFS -> BLINN PHONG THINGS
unsigned int  MESH_Manager::InstanceSetup_PBR(InstanceProperties& prop) {


    // SRT Buffer set up
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


    // Albedo Buffer Setup
    prop.AlbedoBuffer;
    glGenBuffers(1, &prop.AlbedoBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::vec4), &(prop.Albedo[0]), GL_STATIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribDivisor(10, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Metal_Rough_AO_Texture_Buffer Buffer Setup
    prop.Metal_Rough_AO_Texture_Buffer;
    glGenBuffers(1, &prop.Metal_Rough_AO_Texture_Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.Metal_Rough_AO_Texture_Buffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::vec3), &(prop.M_R_A_Texture[0]), GL_DYNAMIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(11, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Diffuse Buffer Setup
    prop.Metal_Rough_AO_Texture_Constant;
    glGenBuffers(1, &prop.Metal_Rough_AO_Texture_Constant);
    glBindBuffer(GL_ARRAY_BUFFER, prop.Metal_Rough_AO_Texture_Constant);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::vec3), &(prop.M_R_A_Constant[0]), GL_DYNAMIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(12);
    glVertexAttribPointer(12, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(12, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    prop.textureIndexBuffer;
    glGenBuffers(1, &prop.textureIndexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, prop.textureIndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::vec2), &(prop.textureIndex[0]), GL_STATIC_DRAW);

    glBindVertexArray(prop.VAO);
    glEnableVertexAttribArray(15);
    glVertexAttribPointer(15, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glVertexAttribDivisor(15, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return prop.entitySRTbuffer;
}

void MESH_Manager::CreateInstanceLine()
{
    Mesh newMesh;
    newMesh.index = (unsigned int)(mContainer.size());

    GLfloat vertices[] = {
        -1.f, 0.f, 0.f,   
        1.0f, 0.f, 0.f
    };

    GLuint indices[] = {
        0, 1
    };
    newMesh.vertices_min = glm::vec3(-1.f, 0.f, 0.f);
    newMesh.vertices_max = glm::vec3(-1.f, 0.f, 0.f);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind vao
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo

    InstanceProperties tempProp;
    tempProp.VAO = vaoid;
    tempProp.drawCount = 2;
    properties.emplace(std::pair<std::string, InstanceProperties>(std::string("Line"), tempProp));
    newMesh.Vaoids.push_back(vaoid);
    newMesh.Vboids.push_back(vboid);
    newMesh.prim = GL_LINES;
    newMesh.Drawcounts.push_back(2);
    newMesh.SRT_Buffer_Index.push_back(InstanceSetup_PBR(properties["Line"]));
    //debugAABB_setup(newMesh.vertices_min, newMesh.vertices_max, properties["Line"]);

    mContainer.emplace(std::string("Line"), newMesh);

}

void MESH_Manager::debugAABB_setup(glm::vec3 minpt, glm::vec3 maxpt, InstanceProperties& prop) // vao
{
    //// find min max points for each axis
    //glm::vec3 minpt = _geom->_vertices[0].pos, maxpt = _geom->_vertices[0].pos;

    //for (size_t i = 0; i < _geom->_vertices.size(); i++)
    //{
    //    minpt.x = std::min(minpt.x, _geom->_vertices[i].pos.x);
    //    minpt.y = std::min(minpt.y, _geom->_vertices[i].pos.y);
    //    minpt.z = std::min(minpt.z, _geom->_vertices[i].pos.z);

    //    maxpt.x = std::max(maxpt.x, _geom->_vertices[i].pos.x);
    //    maxpt.y = std::max(maxpt.y, _geom->_vertices[i].pos.y);
    //    maxpt.z = std::max(maxpt.z, _geom->_vertices[i].pos.z);
    //}
    glm::vec3 pntAABB[8];
    std::vector<glm::ivec2> idxAABB{};

    pntAABB[0] = minpt;
    pntAABB[1] = glm::vec3(minpt.x, minpt.y, maxpt.z);
    pntAABB[2] = glm::vec3(minpt.x, maxpt.y, maxpt.z);
    pntAABB[3] = glm::vec3(minpt.x, maxpt.y, minpt.z);

    pntAABB[4] = maxpt;
    pntAABB[5] = glm::vec3(maxpt.x, maxpt.y, minpt.z);
    pntAABB[6] = glm::vec3(maxpt.x, minpt.y, minpt.z);
    pntAABB[7] = glm::vec3(maxpt.x, minpt.y, maxpt.z);

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
    //GLuint DebugVaoid; // point buffer
    GLuint Pbuff; // point buffer
    GLuint Ibuff; // indice buffer

    glGenVertexArrays(1, &prop.debugVAO);
    glGenBuffers(1, &Pbuff);
    glGenBuffers(1, &Ibuff);

    glBindVertexArray(prop.debugVAO);
    glBindBuffer(GL_ARRAY_BUFFER, Pbuff);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 8, &pntAABB[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ibuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 2 * idxAABB.size(),
        &idxAABB[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind vao
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo

    //entitySRTBuffer
    glBindVertexArray(prop.debugVAO);
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
    //return DebugVaoid;
}

