/*!***************************************************************************************
\file			ModelDecompiler.cpp
\project
\author         Davin Tan
\co-author      Sean Ngo

\par			Course: GAM300
\date           31/10/2023

\brief
    This file contains the definitions of the following:
    1. Loads model files and extract the meshes, material, textures and animations

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "ModelDecompiler.h"
#include "AssetManager.h"

void ModelDecompiler::DeserializeModel(const std::string & _filePath, ModelImporter& importer)
{
    std::ifstream ifs(_filePath, std::ios::binary);

    // Retrieve mesh assets
    DeserializeMeshes(ifs, _filePath,importer);

    // Retrieve material assets
    DeserializeMaterials(ifs, _filePath,importer);

    // Retrieve texture assets
    DeserializeTextures(ifs, _filePath,importer);

    // Retrieve animation assets
    bool hasAnimation;
    ifs.read(reinterpret_cast<char*>(&hasAnimation), sizeof(hasAnimation));
    if(hasAnimation)
        DeserializeAnimations(ifs, _filePath,importer);

    ifs.close();
}

void ModelDecompiler::DeserializeMeshes(std::ifstream& ifs, const std::string& _filePath, ModelImporter& importer)
{
    size_t meshSize;
    ifs.read(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));
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
        std::vector<std::uint16_t> tempInd;
        size_t indSize;
        ifs.read(reinterpret_cast<char*>(&indSize), sizeof(indSize));
        tempInd.resize(indSize);
        ifs.read(reinterpret_cast<char*>(&tempInd[0]), indSize * sizeof(std::uint16_t));

        ifs.read(reinterpret_cast<char*>(&meshAsset.materialIndex), sizeof(meshAsset.materialIndex)); // Material Index

        // Decompression properties
        glm::vec3 posCompressionScale, texCompressionScale, posCompressionOffset, texCompressionOffset;
        ifs.read(reinterpret_cast<char*>(&posCompressionScale), sizeof(glm::vec3)); // Position Scale
        ifs.read(reinterpret_cast<char*>(&texCompressionScale), sizeof(glm::vec2)); // Texture Scale

        ifs.read(reinterpret_cast<char*>(&posCompressionOffset), sizeof(glm::vec3)); // Position Offset
        ifs.read(reinterpret_cast<char*>(&texCompressionOffset), sizeof(glm::vec2)); // Texture Offset

        ifs.read(reinterpret_cast<char*>(&meshAsset.numBones), sizeof(meshAsset.numBones)); // Number of bones

        std::vector<VertexBoneInfo> tempBoneInfo;
        if (meshAsset.numBones)
        {
            tempBoneInfo.resize(vertSize);
            ifs.read(reinterpret_cast<char*>(&tempBoneInfo[0]), vertSize * sizeof(VertexBoneInfo)); // Number of bones
        }
        
        meshAsset.vertices.resize(vertSize); // Resize our vertices vector
        meshAsset.indices.resize(indSize);

        meshAsset.numVertices = (unsigned int)vertSize;
        meshAsset.numIndices = (unsigned int)indSize;

        // Converts Vertex to ModelVertex
        DecompressVertices(meshAsset.vertices, tempVerts, tempBoneInfo, posCompressionScale, texCompressionScale, posCompressionOffset, texCompressionOffset);
        
        // Converts unsigned short to unsigned int
        DecompressIndices(meshAsset.indices, tempInd);

        glm::vec3 min(FLT_MAX);
        glm::vec3 max(FLT_MIN);
        for (size_t j = 0; j < meshAsset.vertices.size(); ++j)
        {
            glm::vec3& pos = meshAsset.vertices[j].position;

            if(meshAsset.numBones == 0)
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

        AssetImporter<MeshAsset> meshImporter{};

        meshAsset.mFilePath = _filePath;
        meshAsset.mFilePath.replace_extension("");
        meshAsset.mFilePath += "_" + std::to_string(i) + ".geom";
        // If there is already meshes in the importer reassign it
        if (i < importer.meshes.size())
        {
            meshImporter = {importer.meshes[i]};
        }
        else
        {
            //Add to model importer guid
            importer.meshes.push_back(meshImporter.guid);
        }

        ASSET.AddSubAsset(meshAsset,meshImporter);

        // Bean: For testing vertices
        /*if (meshAsset.mFilePath.stem().string().find("Floor1x1Merged") != std::string::npos)
        {
            std::string name = meshAsset.mFilePath.stem().string();
            name += "vert.txt";
            std::ofstream test(name);
            test << "Num vertices: " << meshAsset.numVertices << "\n";
            float topVert = meshAsset.boundsMax.y;
            for (size_t j = 0; j < meshAsset.numVertices; j++)
            {
                ModelVertex& v = meshAsset.vertices[j];

                if(v.position.y >= topVert)
                    test << "Vertex " << j << ": " << v.position.x << " " << v.position.y << " " << v.position.z << "\n";
            }

            test.close();

            name = meshAsset.mFilePath.stem().string();
            name += "ind.txt";
            test.open(name);
            test << "Num indices: " << meshAsset.numIndices << "\n";
            for (size_t j = 0; j < meshAsset.numIndices; j += 3)
            {
                auto& v = meshAsset.indices;
                if(meshAsset.vertices[v[j]].position.y >= topVert && meshAsset.vertices[v[j + 1]].position.y >= topVert && meshAsset.vertices[v[j + 2]].position.y >= topVert)
                    test << "Indices: " << v[j] << " " << v[j + 1] << " " << v[j + 2] << "\n";
            }

            test.close();
        }*/
    }
}

void ModelDecompiler::DeserializeMaterials(std::ifstream& ifs, const std::string& _filePath, ModelImporter& importer)
{
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

        AssetImporter<MaterialAsset> materialImporter{};

        // If there is already materials in the importer reassign it
        //if (j < importer.materials.size())
        //{
        //    materialImporter = { importer.materials[j] };
        //}
        //else
        //{
        //    //Add to model importer guid
        //    importer.materials.push_back(materialImporter.guid);
        //}
        //Uncomment this once Materials are converted to material asset
        //ASSET.AddSubAsset(std::move(meshAsset), std::move(materialImporter));
    }
}

void ModelDecompiler::DeserializeTextures(std::ifstream& ifs, const std::string& _filePath, ModelImporter& importer)
{

}

void ModelDecompiler::DeserializeAnimations(std::ifstream& ifs, const std::string& _filePath, ModelImporter& importer)
{
    //size_t animationSize = pModel->animations.GetAnimations().size();
    size_t animationSize;
    ifs.read(reinterpret_cast<char*>(&animationSize), sizeof(animationSize));
    AnimationAsset animation;

    ifs.read(reinterpret_cast<char*>(&animation.duration), sizeof(animation.duration));
    ifs.read(reinterpret_cast<char*>(&animation.ticksPerSecond), sizeof(animation.ticksPerSecond));

    // Bones
    size_t boneSize;
    ifs.read(reinterpret_cast<char*>(&boneSize), sizeof(boneSize));
    animation.bones.resize(boneSize);

    for (size_t i = 0; i < boneSize; i++)
    {
        Bone& bone = animation.bones[i];
        // Position
        ifs.read(reinterpret_cast<char*>(&bone.m_NumPositions), sizeof(bone.m_NumPositions));
        bone.m_Positions.resize(bone.m_NumPositions);
        ifs.read(reinterpret_cast<char*>(&bone.m_Positions[0]), bone.m_NumPositions * sizeof(KeyPosition));

        // Rotation
        ifs.read(reinterpret_cast<char*>(&bone.m_NumRotations), sizeof(bone.m_NumRotations));
        bone.m_Rotations.resize(bone.m_NumRotations);
        ifs.read(reinterpret_cast<char*>(&bone.m_Rotations[0]), bone.m_NumRotations * sizeof(KeyRotation));

        // Scale
        ifs.read(reinterpret_cast<char*>(&bone.m_NumScalings), sizeof(bone.m_NumScalings));
        bone.m_Scales.resize(bone.m_NumScalings);
        ifs.read(reinterpret_cast<char*>(&bone.m_Scales[0]), bone.m_NumScalings * sizeof(KeyScale));

        // Local Transform
        ifs.read(reinterpret_cast<char*>(&bone.m_LocalTransform), sizeof(glm::mat4));

        // Name of Bone
        size_t nameSize;
        ifs.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        bone.m_Name.resize(nameSize);
        ifs.read(reinterpret_cast<char*>(&bone.m_Name[0]), nameSize * sizeof(char));

        // Bone ID
        ifs.read(reinterpret_cast<char*>(&bone.m_ID), sizeof(bone.m_ID));
    }

    // AssimpNodeData
    AssimpNodeData& nodeData = animation.rootNode;
    DeserializeRecursiveNode(ifs, _filePath,nodeData);

    // Bone Info Map
    size_t boneInfoSize = 0;
    ifs.read(reinterpret_cast<char*>(&boneInfoSize), sizeof(boneInfoSize));

    for (size_t i = 0; i < boneInfoSize; i++)
    {
        // Kay of BoneInfoMap
        size_t keySize;
        ifs.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        std::string key;
        key.resize(keySize);
        ifs.read(reinterpret_cast<char*>(&key[0]), keySize * sizeof(char));

        // Value of BoneInfoMap
        BoneInfo boneInfo;
        ifs.read(reinterpret_cast<char*>(&boneInfo), sizeof(boneInfo));

        animation.boneInfoMap[key] = boneInfo;
    }

    ifs.read(reinterpret_cast<char*>(&animation.boneCounter), sizeof(animation.boneCounter));

    animation.animationRange = importer.animationStates;

    animation.mFilePath = _filePath;
    animation.mFilePath.replace_extension("");
    animation.mFilePath += "_" + std::to_string(0) + ".anim";

    // Check if the importer animation size is the same as the model file animation size
    if (importer.animations.size() != animationSize)
    {
        importer.animations.clear();

        // Re-add all the animations into the importer
        for (size_t i = 0; i < animationSize; i++)
        {
            AssetImporter<AnimationAsset> animImporter{};
            importer.animations.push_back(animImporter.guid);

            // Bean: This will be out of this if condition when we change the animation to become a vector of animations
            ASSET.AddSubAsset(animation, animImporter); 
        }
    }
    else // If there is no animations in the importer, add this animation in
    {
        AssetImporter<AnimationAsset> animImporter{};
        // Animation importer already exist so just reassign (Bean: for now its only 1 animation)
        animImporter = { importer.animations[0] };
        ASSET.AddSubAsset(animation, animImporter);
    }
}

void ModelDecompiler::DeserializeRecursiveNode(std::ifstream& ifs, const std::string& _filePath, AssimpNodeData& _node)
{
    ifs.read(reinterpret_cast<char*>(&_node.transformation), sizeof(glm::mat4)); // Transformation

    size_t nameSize;
    ifs.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
    std::string name;
    //_node.name.resize(nameSize);
    name.resize(nameSize);
    ifs.read(reinterpret_cast<char*>(&name[0]), nameSize * sizeof(char)); // Name of Node
    _node.name = name;

    ifs.read(reinterpret_cast<char*>(&_node.childrenCount), sizeof(_node.childrenCount)); // Num of children

    _node.children.resize(_node.childrenCount);
    for (size_t i = 0; i < _node.childrenCount; i++)
    {
        DeserializeRecursiveNode(ifs, _filePath, _node.children[i]);
    }
}

void ModelDecompiler::DecompressVertices(std::vector<ModelVertex>& _meshVertices,
	const std::vector<Vertex>& _oVertices,
    const std::vector<VertexBoneInfo>& _boneInfo,
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
        if (!_boneInfo.empty()) // If there are boneInfos
        {
            E_ASSERT(_meshVertices.size() == _boneInfo.size(), "Mesh vertices and bone size not equal for decompressing.");
            for (size_t j = 0; j < MAX_BONE_INFLUENCE; j++)
            {
                _meshVertices[i].boneIDs[j] = static_cast<int>(_boneInfo[i].boneIDs[j]);
                _meshVertices[i].weights[j] = (_boneInfo[i].weights[j] >= 0 ? static_cast<float>(_boneInfo[i].weights[j]) / 0x7FFF : static_cast<float>(_boneInfo[i].weights[j]) / 0x8000);
            }
        }
        else
        {
            auto& boneId = _meshVertices[i].boneIDs;
            auto& weights = _meshVertices[i].weights;

            boneId[0] = boneId[1] = boneId[2] = boneId[3] = -1;
            weights[0] = weights[1] = weights[2] = weights[3] = 0.f;
        }
    }
}

void ModelDecompiler::DecompressIndices(std::vector<unsigned int>&_meshIndices, const std::vector<std::uint16_t>&_oIndices)
{
    E_ASSERT(_meshIndices.size() == _oIndices.size(), "Both indices vector sizes not equal for decompressing.");

    for (size_t i = 0; i < _oIndices.size(); i++)
    {
        _meshIndices[i] = static_cast<unsigned int>(_oIndices[i]);
    }
}

//GeomImported MeshManager::DeserializeGeoms(const std::string& filePath, const Engine::GUID& guid)
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
//void MeshManager::DecompressVertices(std::vector<gVertex>& mMeshVertices,
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