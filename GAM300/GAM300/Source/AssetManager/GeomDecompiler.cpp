/*!***************************************************************************************
\file			GeomDecompiler.cpp
\project
\author         Davin Tan
\co-author      Sean Ngo

\par			Course: GAM300
\date           31/10/2023

\brief
    This file contains the definitions of the following:
    1. Loads Geoms files and extract the meshes, material, textures and animations

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "GeomDecompiler.h"

ModelComponents GeomDecompiler::DeserializeGeoms(const std::string& _filePath, const Engine::GUID& _guid)
{
    ModelComponents tempModel;
    std::ifstream ifs(_filePath, std::ios::binary);

    size_t meshSize;
    ifs.read(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));

    // Retrieve mesh assets
    for (int i = 0; i < meshSize; ++i)
    {
        MeshAsset meshAsset;

        // Vertices
        std::vector<Vertex> tempVerts;
        size_t vertSize;
        ifs.read(reinterpret_cast<char*>(&vertSize), sizeof(vertSize));
        tempVerts.resize(vertSize);
        ifs.read(reinterpret_cast<char*>(&tempVerts[0]), vertSize * sizeof(Vertex));

        // Indices
        size_t indSize;
        ifs.read(reinterpret_cast<char*>(&indSize), sizeof(indSize));
        meshAsset.indices.resize(indSize);
        ifs.read(reinterpret_cast<char*>(&meshAsset.indices[0]), indSize * sizeof(unsigned int));

        ifs.read(reinterpret_cast<char*>(&meshAsset.materialIndex), sizeof(meshAsset.materialIndex)); // Material Index

        // Decompression properties
        glm::vec3 posCompressionScale, texCompressionScale, posCompressionOffset, texCompressionOffset;
        ifs.read(reinterpret_cast<char*>(&posCompressionScale), sizeof(glm::vec3)); // Position Scale
        ifs.read(reinterpret_cast<char*>(&texCompressionScale), sizeof(glm::vec2)); // Texture Scale

        ifs.read(reinterpret_cast<char*>(&posCompressionOffset), sizeof(glm::vec3)); // Position Offset
        ifs.read(reinterpret_cast<char*>(&texCompressionOffset), sizeof(glm::vec2)); // Texture Offset

        meshAsset.vertices.resize(vertSize); // Resize our vertices vector

        // Converts Vertex to ModelVertex
        DecompressVertices(meshAsset.vertices, tempVerts, posCompressionScale, texCompressionScale, posCompressionOffset, texCompressionOffset);

        glm::vec3 min(FLT_MAX);
        glm::vec3 max(FLT_MIN);
        for (int i = 0; i < meshAsset.vertices.size(); ++i)
        {
            glm::vec3& pos = meshAsset.vertices[i].position;
            pos = pos * 0.01f; // Bean: 0.01f here converts the vertices position from centimeters to meters

            min.x = std::min(pos.x, min.x);
            min.y = std::min(pos.y, min.y);
            min.z = std::min(pos.z, min.z);

            max.x = std::max(pos.x, max.x);
            max.y = std::max(pos.y, max.y);
            max.z = std::max(pos.z, max.z);
        }

        // Bounds
        meshAsset.boundsMin = min;
        meshAsset.boundsMax = max;

        // Add this tempMesh into our tempGeom
        tempModel.meshes.push_back(meshAsset);
    }

    // Retrieve material assets

    // Retrieve texture assets

    // Retrieve animation assets

    ifs.close();

    return tempModel;
}

void GeomDecompiler::DecompressVertices(std::vector<ModelVertex>& _meshVertices,
	const std::vector<Vertex>& _oVertices,
	const glm::vec3& _posCompressScale,
	const glm::vec2& _texCompressScale,
	const glm::vec3& _posOffset,
	const glm::vec2& _texOffset)
{
    E_ASSERT(_meshVertices.size() == _oVertices.size(), "Both vertices vector sizes not equal for decompressing.");

    for (int i = 0; i < _meshVertices.size(); ++i)
    {
        // Position
        _meshVertices[i].position.x = (_oVertices[i].posX >= 0 ? static_cast<float>(_oVertices[i].posX) / 0x7FFF : static_cast<float>(_oVertices[i].posX) / 0x8000) * _posCompressScale.x + _posOffset.x;
        _meshVertices[i].position.y = (_oVertices[i].posY >= 0 ? static_cast<float>(_oVertices[i].posY) / 0x7FFF : static_cast<float>(_oVertices[i].posY) / 0x8000) * _posCompressScale.y + _posOffset.y;
        _meshVertices[i].position.z = (_oVertices[i].posZ >= 0 ? static_cast<float>(_oVertices[i].posZ) / 0x7FFF : static_cast<float>(_oVertices[i].posZ) / 0x8000) * _posCompressScale.z + _posOffset.z;

        // Texture
        _meshVertices[i].textureCords.x = (_oVertices[i].texU >= 0 ? static_cast<float>(_oVertices[i].texU) / 0x7FFF : static_cast<float>(_oVertices[i].texU) / 0x8000) * _texCompressScale.x + _texOffset.x;
        _meshVertices[i].textureCords.y = (_oVertices[i].texV >= 0 ? static_cast<float>(_oVertices[i].texV) / 0x7FFF : static_cast<float>(_oVertices[i].texV) / 0x8000) * _texCompressScale.y + _texOffset.y;

        // Normal
        _meshVertices[i].normal.x = (_oVertices[i].normX >= 0 ? static_cast<float>(_oVertices[i].normX) / 0x7FFF : static_cast<float>(_oVertices[i].normX) / 0x8000);
        _meshVertices[i].normal.y = (_oVertices[i].normY >= 0 ? static_cast<float>(_oVertices[i].normY) / 0x7FFF : static_cast<float>(_oVertices[i].normY) / 0x8000);
        _meshVertices[i].normal.z = (_oVertices[i].normZ >= 0 ? static_cast<float>(_oVertices[i].normZ) / 0x7FFF : static_cast<float>(_oVertices[i].normZ) / 0x8000);

        // Tangent
        _meshVertices[i].tangent.x = (_oVertices[i].tanX >= 0 ? static_cast<float>(_oVertices[i].tanX) / 0x7FFF : static_cast<float>(_oVertices[i].tanX) / 0x8000);
        _meshVertices[i].tangent.y = (_oVertices[i].tanY >= 0 ? static_cast<float>(_oVertices[i].tanX) / 0x7FFF : static_cast<float>(_oVertices[i].tanY) / 0x8000);
        _meshVertices[i].tangent.z = (_oVertices[i].tanZ >= 0 ? static_cast<float>(_oVertices[i].tanZ) / 0x7FFF : static_cast<float>(_oVertices[i].tanZ) / 0x8000);

        // Color
        _meshVertices[i].color.r = _oVertices[i].colorR;
        _meshVertices[i].color.g = _oVertices[i].colorG;
        _meshVertices[i].color.b = _oVertices[i].colorB;
        _meshVertices[i].color.a = _oVertices[i].colorA;

        // Animation
        for (size_t j = 0; j < MAX_BONE_INFLUENCE; j++)
        {
            _meshVertices[i].boneIDs[j] = static_cast<int>(_oVertices[i].boneIDs[j]);
            _meshVertices[i].weights[j] = (_oVertices[i].weights[j] >= 0 ? static_cast<float>(_oVertices[i].weights[j]) / 0x7FFF : static_cast<float>(_oVertices[i].weights[j]) / 0x8000);
        }
    }
}

//GeomImported MESH_Manager::DeserializeGeoms(const std::string& filePath, const Engine::GUID& guid)
//{
//    GeomImported tempGeom;
//    std::ifstream ifs(filePath, std::ios::binary);
//
//    size_t meshSize;
//    ifs.read(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));
//
//    for (int i = 0; i < meshSize; ++i)
//    {
//        gMesh tempMesh;
//
//        // Vertices
//        std::vector<Vertex> tempVerts;
//        size_t vertSize;
//        ifs.read(reinterpret_cast<char*>(&vertSize), sizeof(vertSize));
//        tempVerts.resize(vertSize);
//        ifs.read(reinterpret_cast<char*>(&tempVerts[0]), vertSize * sizeof(Vertex));
//
//        // Indices
//        size_t indSize;
//        ifs.read(reinterpret_cast<char*>(&indSize), sizeof(indSize));
//        tempMesh._indices.resize(indSize);
//        ifs.read(reinterpret_cast<char*>(&tempMesh._indices[0]), indSize * sizeof(unsigned int));
//
//        ifs.read(reinterpret_cast<char*>(&tempMesh.materialIndex), sizeof(tempMesh.materialIndex)); // Material Index
//
//        ifs.read(reinterpret_cast<char*>(&tempMesh.mPosCompressionScale), sizeof(glm::vec3)); // Position Scale
//        ifs.read(reinterpret_cast<char*>(&tempMesh.mTexCompressionScale), sizeof(glm::vec2)); // Texture Scale
//
//        ifs.read(reinterpret_cast<char*>(&tempMesh.mPosCompressionOffset), sizeof(glm::vec3)); // Position Offset
//        ifs.read(reinterpret_cast<char*>(&tempMesh.mTexCompressionOffset), sizeof(glm::vec2)); // Texture Offset
//
//        tempMesh._vertices.resize(vertSize); // Resize our vertices vector
//        DecompressVertices(tempMesh._vertices, tempVerts, tempMesh.mPosCompressionScale, tempMesh.mTexCompressionScale, tempMesh.mPosCompressionOffset, tempMesh.mTexCompressionOffset); // Converts Vertex to gVertex
//
//        // Store the geom vertices position and indices in asset manager
//        for (int v = 0; v < tempMesh._vertices.size(); ++v)
//        {
//            StoreMeshVertex(guid, tempMesh._vertices[v].pos);
//        }
//        for (int k = 0; k < tempMesh._indices.size(); ++k)
//        {
//            StoreMeshIndex(guid, tempMesh._indices[k]);
//        }
//
//        // Add this tempMesh into our tempGeom
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
//        //size_t texSize;
//        //ifs.read(reinterpret_cast<char*>(&texSize), sizeof(texSize));
//        //if (texSize > 0)
//        //{
//        //    tempMat.textures.resize(texSize);
//        //    ifs.read(reinterpret_cast<char*>(&tempMat.textures[0]), texSize * sizeof(Texture));
//        //}
//
//        tempGeom._materials.push_back(tempMat);
//    }
//
//    ifs.close();
//
//    return tempGeom;
//}
//
//void MESH_Manager::DecompressVertices(std::vector<gVertex>& mMeshVertices,
//    const std::vector<Vertex>& oVertices,
//    const glm::vec3& mPosCompressScale,
//    const glm::vec2& mTexCompressScale,
//    const glm::vec3& mPosOffset,
//    const glm::vec2& mTexOffset)
//{
//    E_ASSERT(mMeshVertices.size() == oVertices.size(), "Both vertices vector sizes not equal for decompressing.");
//
//    for (int i = 0; i < mMeshVertices.size(); ++i)
//    {
//        // Position
//        mMeshVertices[i].pos.x = (oVertices[i].posX >= 0 ? static_cast<float>(oVertices[i].posX) / 0x7FFF : static_cast<float>(oVertices[i].posX) / 0x8000) * mPosCompressScale.x + mPosOffset.x;
//        mMeshVertices[i].pos.y = (oVertices[i].posY >= 0 ? static_cast<float>(oVertices[i].posY) / 0x7FFF : static_cast<float>(oVertices[i].posY) / 0x8000) * mPosCompressScale.y + mPosOffset.y;
//        mMeshVertices[i].pos.z = (oVertices[i].posZ >= 0 ? static_cast<float>(oVertices[i].posZ) / 0x7FFF : static_cast<float>(oVertices[i].posZ) / 0x8000) * mPosCompressScale.z + mPosOffset.z;
//
//        // Texture
//        mMeshVertices[i].tex.x = (oVertices[i].texU >= 0 ? static_cast<float>(oVertices[i].texU) / 0x7FFF : static_cast<float>(oVertices[i].texU) / 0x8000) * mTexCompressScale.x + mTexOffset.x;
//        mMeshVertices[i].tex.y = (oVertices[i].texV >= 0 ? static_cast<float>(oVertices[i].texV) / 0x7FFF : static_cast<float>(oVertices[i].texV) / 0x8000) * mTexCompressScale.y + mTexOffset.y;
//
//        // Normal
//        mMeshVertices[i].normal.x = (oVertices[i].normX >= 0 ? static_cast<float>(oVertices[i].normX) / 0x7FFF : static_cast<float>(oVertices[i].normX) / 0x8000);
//        mMeshVertices[i].normal.y = (oVertices[i].normY >= 0 ? static_cast<float>(oVertices[i].normY) / 0x7FFF : static_cast<float>(oVertices[i].normY) / 0x8000);
//        mMeshVertices[i].normal.z = (oVertices[i].normZ >= 0 ? static_cast<float>(oVertices[i].normZ) / 0x7FFF : static_cast<float>(oVertices[i].normZ) / 0x8000);
//
//        // Tangent
//        mMeshVertices[i].tangent.x = (oVertices[i].tanX >= 0 ? static_cast<float>(oVertices[i].tanX) / 0x7FFF : static_cast<float>(oVertices[i].tanX) / 0x8000);
//        mMeshVertices[i].tangent.y = (oVertices[i].tanY >= 0 ? static_cast<float>(oVertices[i].tanX) / 0x7FFF : static_cast<float>(oVertices[i].tanY) / 0x8000);
//        mMeshVertices[i].tangent.z = (oVertices[i].tanZ >= 0 ? static_cast<float>(oVertices[i].tanZ) / 0x7FFF : static_cast<float>(oVertices[i].tanZ) / 0x8000);
//
//        // Color
//        mMeshVertices[i].color.r = oVertices[i].colorR;
//        mMeshVertices[i].color.g = oVertices[i].colorG;
//        mMeshVertices[i].color.b = oVertices[i].colorB;
//        mMeshVertices[i].color.a = oVertices[i].colorA;
//    }
//}