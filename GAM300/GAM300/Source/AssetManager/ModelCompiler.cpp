/*!***************************************************************************************
\file			ModelCompiler.cpp
\project		
\author         Davin Tan
\co-author      Sean Ngo

\par			Course: GAM300
\date           29/10/2023

\brief
    This file contains the definitions of the following:
    1. Loading of FBX files with assimp loader
		a. Creation of descriptor file for the mesh
		b. Deserialization of FBX file
		b. Reading and storing of meshes
		c. Compression of vertices
	2. Serialization of meshes into custom binary format

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "ModelCompiler.h"
#include "Meshoptimizer/meshoptimizer.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#pragma warning( disable : 4100)

GeomComponents ModelCompiler::LoadModel(const std::filesystem::path& _filePath, const bool& _serialize)
{
	// Ensure that it is a fbx or obj file
	CheckExtension(_filePath);
	
	//DeserializeDescriptor(descriptorFilePath);

	Assimp::Importer assimpImporter;
	uint32_t ImportOptions{};

	// Default import
	ImportOptions =
		aiPostProcessSteps::aiProcess_CalcTangentSpace |
		aiPostProcessSteps::aiProcess_Triangulate |
		aiPostProcessSteps::aiProcess_GenUVCoords |
		aiPostProcessSteps::aiProcess_GenSmoothNormals |
		aiPostProcessSteps::aiProcess_TransformUVCoords |
		aiPostProcessSteps::aiProcess_FlipUVs;
		aiPostProcessSteps::aiProcess_FlipWindingOrder;

	// Import fbx with animation
	const aiScene* defaultScene = assimpImporter.ReadFile(_filePath.string(), ImportOptions);
	E_ASSERT(defaultScene, "Error reading file into assimp _scene!!");

	GeomComponents model;
	pModel = &model;

	// If the scene does not contain any animations
	hasAnimation = defaultScene->HasAnimations();
	if (!hasAnimation)
	{
		ImportOptions =
			aiPostProcessSteps::aiProcess_CalcTangentSpace |			// Calculates the tangents and bitangents for the imported meshes.
			aiPostProcessSteps::aiProcess_Triangulate |					// Triangulates all faces of all meshes
			aiPostProcessSteps::aiProcess_JoinIdenticalVertices |		// Identifies and joins identical vertex data sets within all imported meshes
			aiPostProcessSteps::aiProcess_LimitBoneWeights |			// for skin model max;
			aiPostProcessSteps::aiProcess_GenUVCoords |					// Convert ro proper UV coordinate channel
			aiPostProcessSteps::aiProcess_GenNormals |					// Generate normals
			aiPostProcessSteps::aiProcess_TransformUVCoords |			// apply UV projection
			aiPostProcessSteps::aiProcess_FlipUVs |						// flips all UV coordinates along the y-axis and adjusts
			aiPostProcessSteps::aiProcess_FindInstances |				// searches for duplicate meshes and replaces them with references to the first mesh
			aiPostProcessSteps::aiProcess_RemoveRedundantMaterials |	// remove unreferenced _material
			aiPostProcessSteps::aiProcess_FindInvalidData |				// remove or fix invalid data
			//aiPostProcessSteps::aiProcess_OptimizeMeshes |			// Reduce mesh count
			//aiPostProcessSteps::aiProcess_OptimizeGraph |				// Optimize scene hierarchy
			aiPostProcessSteps::aiProcess_PreTransformVertices;

		// Import fbx without animation
		const aiScene* scene = assimpImporter.ReadFile(_filePath.string(), ImportOptions);
		E_ASSERT(scene, "Error reading file into assimp _scene!!");

		ProcessNode(*scene->mRootNode, *scene);
	}
	else
	{
		pModel->animations.push_back(Animation());

		ProcessNode(*defaultScene->mRootNode, *defaultScene);

		ProcessBones(*defaultScene->mRootNode, *defaultScene);
	}

	if (_serialize)
		SerializeBinaryGeom(_filePath);

	return model;
}

void ModelCompiler::ProcessBones(const aiNode& _node, const aiScene& _scene)
{
	if (hasAnimation)
	{
		auto tempAnimations = _scene.mAnimations[0]; // this might need to change quite a bit since an fbx may hv > 1 anim
		Animation& animation = pModel->animations[0];
		animation.GetDuration() = (float)tempAnimations->mDuration;
		animation.GetTicksPerSecond() = (int)tempAnimations->mTicksPerSecond;
		ReadHierarchyData(animation.GetRootNode(), _scene.mRootNode);
		ReadMissingBones(tempAnimations, animation);
	}
}

void ModelCompiler::ProcessNode(const aiNode& _node, const aiScene& _scene)
{
	for (unsigned int i = 0; i < _node.mNumMeshes; ++i) // Loop through _node meshes
	{
		aiMesh* pMesh = _scene.mMeshes[_node.mMeshes[i]];
		pModel->meshes.push_back(ProcessMesh(*pMesh, _scene));
	}
	for (unsigned int j = 0; j < _node.mNumChildren; ++j) // Initialize all the children nodes
	{
		ProcessNode(*_node.mChildren[j], _scene);
	}
}

Geom_Mesh ModelCompiler::ProcessMesh(const aiMesh& _mesh, const aiScene& _scene)
{
	std::vector<ModelVertex> tempVertex;
	std::vector<unsigned int> tempIndices;
	
	for (unsigned int i = 0; i < _mesh.mNumVertices; ++i) // Processing all vertices in this single mesh
	{
		ModelVertex temp;

		// Vertex position
		temp.position = glm::vec3(static_cast<float>(_mesh.mVertices[i].x),
			static_cast<float>(_mesh.mVertices[i].y),
			static_cast<float>(_mesh.mVertices[i].z));

		// Vertex normal
		temp.normal = glm::vec3(static_cast<float>(_mesh.mNormals[i].x),
			static_cast<float>(_mesh.mNormals[i].y),
			static_cast<float>(_mesh.mNormals[i].z));

		if (_mesh.HasTextureCoords(0)) // Vertex Texture 
		{
			temp.textureCords = glm::vec2(static_cast<float>(_mesh.mTextureCoords[0][i].x),
				static_cast<float>(_mesh.mTextureCoords[0][i].y));
		}

		if (_mesh.HasVertexColors(0)) // Vertex Color
		{
			temp.color = glm::vec4(static_cast<float>(_mesh.mColors[0][i].r),
				static_cast<float>(_mesh.mColors[0][i].g),
				static_cast<float>(_mesh.mColors[0][i].b),
				static_cast<float>(_mesh.mColors[0][i].a));
		}

		if (_mesh.mTangents != nullptr) // Vertex tangent
		{
			temp.tangent = {
				static_cast<float>(_mesh.mTangents[i].x),
				static_cast<float>(_mesh.mTangents[i].y),
				static_cast<float>(_mesh.mTangents[i].z)
			};
		}

		// Animation
		for (int j = 0; j < MAX_BONE_INFLUENCE; j++)
		{
			temp.boneIDs[j] = -1;
			temp.weights[j] = 0.f;
		}

		tempVertex.push_back(temp); // Add this vertex into our vector of vertices
	}

	for (unsigned int j = 0; j < _mesh.mNumFaces; ++j) // Processing all faces in this single mesh
	{
		const aiFace& face = _mesh.mFaces[j];

		for (unsigned int k = 0; k < face.mNumIndices; ++k) // Loop through all indices in this current face
		{
			tempIndices.push_back(face.mIndices[k]); // Store the indices in our vector of indices
		}
	}

	if (_mesh.mMaterialIndex >= 0) // Import the mesh material if it exists
	{
		const aiMaterial& mat = *_scene.mMaterials[_mesh.mMaterialIndex];
		ImportMaterialAndTextures(mat);
	}

	// Calculate the _material index of this mesh
	int materialIndex = static_cast<int>(pModel->materials.size() - 1);

	std::vector<Vertex> _compressedVertices;
	std::vector<std::uint16_t> _compressedIndices;
	std::vector<VertexBoneInfo> tempBoneInfo;
	std::pair<glm::vec3, glm::vec2> mPosTexOffset;
	std::pair<glm::vec3, glm::vec2> mPosTexScale;

	// Only extract bone data if there is animations
	
	unsigned int numBones = _mesh.mNumBones;
	if (hasAnimation)
		ExtractBoneWeightForVertices(tempVertex, _mesh, _scene);
	
	Optimize(tempVertex, tempIndices); // Optimize this mesh

	// Compress vertices for storing in our vertex
	CompressVertices(_compressedVertices, tempVertex, mPosTexOffset, mPosTexScale);

	if (hasAnimation)
	{
		// Bones
		tempBoneInfo.resize(tempVertex.size());
		for (size_t i = 0; i < tempVertex.size(); i++)
		{
			for (size_t j = 0; j < MAX_BONE_INFLUENCE; j++)
			{
				tempBoneInfo[i].boneIDs[j] = static_cast<std::int16_t>(tempVertex[i].boneIDs[j]);
				tempBoneInfo[i].weights[j] = static_cast<std::int16_t>(tempVertex[i].weights[j] >= 0 ? tempVertex[i].weights[j] * 0x7FFF : tempVertex[i].weights[j] * 0x8000);
			}
		}
	}

	// Compress indices for storing in our indices
	CompressIndices(_compressedIndices, tempIndices);

	return Geom_Mesh(_compressedVertices, tempBoneInfo, _compressedIndices, materialIndex, mPosTexScale.first, mPosTexScale.second, mPosTexOffset.first, mPosTexOffset.second, numBones); // Create this mesh
}

void ModelCompiler::Optimize(std::vector<ModelVertex>& _vert, std::vector<unsigned int>& _ind)
{
	std::vector<unsigned int> remap(_ind.size());
	const size_t vertCount = meshopt_generateVertexRemap(&remap[0],
		_ind.data(),
		_ind.size(),
		_vert.data(),
		_ind.size(),
		sizeof(ModelVertex));

	std::vector<unsigned int> remappedIndices(_ind.size());
	std::vector<ModelVertex> remappedVertices(vertCount);

	meshopt_remapIndexBuffer(remappedIndices.data(), _ind.data(), _ind.size(), &remap[0]);
	meshopt_remapVertexBuffer(remappedVertices.data(), _vert.data(), _vert.size(), sizeof(ModelVertex), &remap[0]);

	meshopt_optimizeVertexCache(remappedIndices.data(), remappedIndices.data(), _ind.size(), vertCount);

	meshopt_optimizeOverdraw(remappedIndices.data(),
		remappedIndices.data(),
		_ind.size(),
		&remappedVertices[0].position.x,
		vertCount,
		sizeof(ModelVertex),
		1.05f);

	meshopt_optimizeVertexFetch(remappedVertices.data(),
		remappedIndices.data(),
		_ind.size(),
		remappedVertices.data(),
		vertCount,
		sizeof(ModelVertex));

	// Below is for LOD of meshes

	//const float threshold = 0.2f; // Controls the LOD of the mesh
	//const size_t targetIndexCount = size_t(remappedIndices.size() * threshold);
	//const float targetError = 1e-2f;
	//std::vector<unsigned int> indicesLod(remappedIndices.size());
	//indicesLod.resize(meshopt_simplify(&indicesLod[0],
	//									remappedIndices.data(), 
	//									remappedIndices.size(),
	//									&remappedVertices[0].position.x, 
	//									vertCount, 
	//									sizeof(Vertex), 
	//									targetIndexCount,
	//									targetError));

	_vert = remappedVertices;
	_ind = remappedIndices;
}

// Basically changing floats to 2 bytes integers
void ModelCompiler::CompressVertices(std::vector<Vertex>& _compressedVertices,
	const std::vector<ModelVertex> tempVertex,
	std::pair<glm::vec3, glm::vec2>& _mOffsets,
	std::pair<glm::vec3, glm::vec2>& _mScales)
{
	//for (const auto& _vert : tempVertex)
	//{
	//	Vertex currVert;
	//	currVert.position.x = _vert.position.x;
	//	currVert.position.y = _vert.position.y;
	//	currVert.position.z = _vert.position.z;
	//	currVert.normal.x = _vert.normal.x;
	//	currVert.normal.y = _vert.normal.y;
	//	currVert.normal.z = _vert.normal.z;
	//	currVert.tangent.x = _vert.tangent.x;
	//	currVert.tangent.y = _vert.tangent.y;
	//	currVert.tangent.z = _vert.tangent.z;
	//	//currVert.tanSign = _vert.tangent.z >= 0 ? 0x1 : 0x3;
	//	currVert.textureCords.x = _vert.textureCords.x;
	//	currVert.textureCords.y = _vert.textureCords.y;
	//	currVert.color.r = _vert.color.r;
	//	currVert.color.g = _vert.color.g;
	//	currVert.color.b = _vert.color.b;
	//	currVert.color.a = _vert.color.a;

	//	CompressVertices.push_back(currVert);
	//}


	// Get BoundingBox3D of the vertex position and texture coordinates
	float mPosMinX = FLT_MAX, mPosMinY = FLT_MAX, mPosMinZ = FLT_MAX;
	float mPosMaxX = -FLT_MAX, mPosMaxY = -FLT_MAX, mPosMaxZ = -FLT_MAX;

	float mTexMinU = FLT_MAX, mTexMinV = FLT_MAX;
	float mTexMaxU = -FLT_MAX, mTexMaxV = -FLT_MAX;
	for (const auto& v : tempVertex)
	{
		// Position
		mPosMinX = std::min(mPosMinX, v.position.x);
		mPosMinY = std::min(mPosMinY, v.position.y);
		mPosMinZ = std::min(mPosMinZ, v.position.z);
		mPosMaxX = std::max(mPosMaxX, v.position.x);
		mPosMaxY = std::max(mPosMaxY, v.position.y);
		mPosMaxZ = std::max(mPosMaxZ, v.position.z);

		// Texture
		mTexMinU = std::min(mTexMinU, v.textureCords.x);
		mTexMinV = std::min(mTexMinV, v.textureCords.y);
		mTexMaxU = std::max(mTexMaxU, v.textureCords.x);
		mTexMaxV = std::max(mTexMaxV, v.textureCords.y);
	}

	glm::vec3 minPos{ mPosMinX, mPosMinY, mPosMinZ };
	glm::vec3 maxPos{ mPosMaxX, mPosMaxY, mPosMaxZ };
	glm::vec2 minTex{ mTexMinU, mTexMinV };
	glm::vec2 maxTex{ mTexMaxU, mTexMaxV };

	BoundingBox3D mPosAABB, mTexAABB;
	mPosAABB.mMin = minPos;
	mPosAABB.mMax = maxPos;

	mTexAABB.mMin = { minTex.x, minTex.y, 0.f };
	mTexAABB.mMax = { maxTex.x, maxTex.y, 0.f };

	// Get the scalings of this submesh
	_mScales.first = mPosAABB.mMax - mPosAABB.mMin;
	_mScales.second = mTexAABB.mMax - mTexAABB.mMin;

	// Get the offsets of this submesh
	_mOffsets.first = (mPosAABB.mMin + mPosAABB.mMax) / 2.f;
	_mOffsets.second = (mTexAABB.mMin + mTexAABB.mMax) / 2.f;

	// Compressing the vertices here
	for (const auto& _vert : tempVertex)
	{
		float val;
		Vertex currVert;

		// Position
		val = (_vert.position.x - _mOffsets.first.x) / _mScales.first.x;
		currVert.posX = static_cast<std::int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000); // Multiply by maximum + value or minimum - value

		val = (_vert.position.y - _mOffsets.first.y) / _mScales.first.y;
		currVert.posY = static_cast<std::int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000);

		val = (_vert.position.z - _mOffsets.first.z) / _mScales.first.z;
		currVert.posZ = static_cast<std::int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000);

		// Texture
		val = (_vert.textureCords.x - _mOffsets.second.x) / _mScales.second.x;
		currVert.texU = static_cast<int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000);

		val = (_vert.textureCords.y - _mOffsets.second.y) / _mScales.second.y;
		currVert.texV = static_cast<int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000);

		// Normals
		currVert.normX = static_cast<std::int16_t>(_vert.normal.x >= 0 ? _vert.normal.x * 0x7FFF : _vert.normal.x * 0x8000);
		currVert.normY = static_cast<std::int16_t>(_vert.normal.y >= 0 ? _vert.normal.y * 0x7FFF : _vert.normal.y * 0x8000);
		currVert.normZ = static_cast<std::int16_t>(_vert.normal.z >= 0 ? _vert.normal.z * 0x7FFF : _vert.normal.z * 0x8000);

		// Tangent
		currVert.tanX = static_cast<std::int16_t>(_vert.tangent.x >= 0 ? _vert.tangent.x * 0x7FFF : _vert.tangent.x * 0x8000);
		currVert.tanY = static_cast<std::int16_t>(_vert.tangent.y >= 0 ? _vert.tangent.y * 0x7FFF : _vert.tangent.y * 0x8000);
		currVert.tanZ = static_cast<std::int16_t>(_vert.tangent.z >= 0 ? _vert.tangent.z * 0x7FFF : _vert.tangent.z * 0x8000);

		// Color
		currVert.colorR = static_cast<std::int8_t>(_vert.color.r);
		currVert.colorG = static_cast<std::int8_t>(_vert.color.g);
		currVert.colorB = static_cast<std::int8_t>(_vert.color.b);
		currVert.colorA = static_cast<std::int8_t>(_vert.color.a);

		_compressedVertices.push_back(currVert);
	}
}

void ModelCompiler::CompressIndices(std::vector<std::uint16_t>& _compressedIndices, std::vector<unsigned int>& _tempInd)
{
	_compressedIndices.resize(_tempInd.size());
	for (size_t i = 0; i < _tempInd.size(); i++)
	{
		_compressedIndices[i] = static_cast<std::uint16_t>(_tempInd[i]);
	}
}

void ModelCompiler::ImportMaterialAndTextures(const aiMaterial& _material)
{
	// Materials
	aiColor4D specularColor;
	aiColor4D diffuseColor;
	aiColor4D ambientColor;
	float shininess;

	aiGetMaterialColor(&_material, AI_MATKEY_COLOR_SPECULAR, &specularColor);
	aiGetMaterialColor(&_material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
	aiGetMaterialColor(&_material, AI_MATKEY_COLOR_AMBIENT, &ambientColor);
	aiGetMaterialFloat(&_material, AI_MATKEY_SHININESS, &shininess);

	Material tempMat(
		specularColor.r, specularColor.g, specularColor.b, shininess,
		diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a,
		ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a);

	// Textures
	aiString specName{};
	aiString diffName{};
	aiString ambiName{};

	if (_material.GetTextureCount(aiTextureType_SPECULAR) > 0) // Specular texture
	{
		Texture tempTex;
		_material.Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), specName);
		tempTex.filepath = specName.C_Str();

		tempMat.textures.push_back(tempTex);
	}

	if (_material.GetTextureCount(aiTextureType_DIFFUSE) > 0) // Diffuse texture
	{
		Texture tempTex;
		_material.Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffName);
		tempTex.filepath = diffName.C_Str();

		tempMat.textures.push_back(tempTex);
	}

	if (_material.GetTextureCount(aiTextureType_AMBIENT) > 0) // Ambient texture
	{
		Texture tempTex;
		_material.Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), ambiName);
		tempTex.filepath = ambiName.C_Str();

		tempMat.textures.push_back(tempTex);
	}

	pModel->materials.push_back(tempMat);
	//pMC->materials.push_back(tempMat);

	//_materials.emplace_back();
	//Material& mat = _materials[0];
	//mat.GUID = reinterpret_cast<std::size_t>(&_material);

	//for (unsigned int l = 0; l < _material.mNumProperties; ++l) // Loop through all properties of this _material
	//{
	//	const aiMaterialProperty& matProperty = *_material.mProperties[l];
	//	if (matProperty.mType != aiPTI_String)
	//	{
	//		continue;
	//	}

	//	aiString* pString = reinterpret_cast<aiString*>(matProperty.mData); // Cast bits to aiString*

	//	if (matProperty.mSemantic == aiTextureType_NONE)
	//	{
	//		mat.matName = pString->C_Str();
	//		continue;
	//	}

	//	SampleHolder sample;
	//	Texture texture;

	//	// Setting hint type
	//	/*if (matProperty.mSemantic == aiTextureType_NORMAL_CAMERA || xcore::string::FindStrI(pString->C_Str(), "_Normal") != -1)
	//	{
	//		sample.type = "PBR_NORMAL";
	//	}
	//	else if (matProperty.mSemantic == aiTextureType_BASE_COLOR || xcore::string::FindStrI(pString->C_Str(), "_Base_Color") != -1)
	//	{
	//		sample.type = "PBR_ALBEDO";
	//	}
	//	else if (matProperty.mSemantic == aiTextureType_AMBIENT_OCCLUSION || xcore::string::FindStrI(pString->C_Str(), "_AO") != -1)
	//	{
	//		sample.type = "PBR_OCCLUSION";
	//	}
	//	else if (matProperty.mSemantic == aiTextureType_DIFFUSE_ROUGHNESS || xcore::string::FindStrI(pString->C_Str(), "_Roughness") != -1)
	//	{
	//		sample.type = "PBR_ROUGHNESS";
	//	}
	//	else
	//	{
	//		sample.type = "UNKNOWN";
	//	}*/

	//	texture.filepath = pString->C_Str(); // Set file path of texture

	//	int  iTexture = 0;
	//	bool bFound = false;
	//	for (const auto& Tex : _textures)
	//	{
	//		if (Tex.filepath == texture.filepath)
	//		{
	//			bFound = true;
	//			break;
	//		}
	//		++iTexture;
	//	}

	//	sample.binding = iTexture; // Set sampler texture binding index wrt to textures vector size
	//	mat._samples.push_back(sample); // Set the texture index
	//	if (bFound == false)
	//	{
	//		_textures.push_back(texture); // Add this texture to our vector of textures
	//	}

	//}

	return;
}

void ModelCompiler::ExtractBoneWeightForVertices(std::vector<ModelVertex>& _vert, const aiMesh& _mesh, const aiScene& _scene)
{
	// Just need one animation to reference to get the bones and boneCount, because all animations
	// use the same bones and have the same bone count, id and offset
	auto& boneInfoMap = pModel->animations[0].GetBoneInfoMap();
	int& boneCount = pModel->animations[0].GetBoneCount();
	
	for (int boneIndex = 0; boneIndex < (int)_mesh.mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = _mesh.mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCount;
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(_mesh.mBones[boneIndex]->mOffsetMatrix);
			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCount;
			boneCount++;
		}
		else
		{
			boneID = boneInfoMap[boneName].id;
		}
		E_ASSERT(boneID != -1, "Bone id is invalid!");
		auto weights = _mesh.mBones[boneIndex]->mWeights;
		int numWeights = _mesh.mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			E_ASSERT(vertexId <= _vert.size(), "Vertex id exceed vertices size!");

			//SetVertexBoneData(_vert[vertexId], boneID, weight); 
			// idk if i am doing this right i went to add my own thing
			if (weight == 0.0f) // skip if bone weight 0
				continue;

			for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
			{
				if (_vert[vertexId].boneIDs[i] == boneID) // skip if bone existd alr
					continue;

				if (_vert[vertexId].boneIDs[i] < 0)
				{
					_vert[vertexId].weights[i] = weight;
					_vert[vertexId].boneIDs[i] = boneID;
					break;
				}
			}
		}
	}
}


void ModelCompiler::ReadMissingBones(const aiAnimation* _tempAnimation, Animation& _animation)
{
	int size = _tempAnimation->mNumChannels;

	auto& boneInfoMap = _animation.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
	int& boneCount = _animation.GetBoneCount(); //getting the m_BoneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = _tempAnimation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		_animation.GetBones().push_back(Bone(channel->mNodeName.data,
			boneInfoMap[channel->mNodeName.data].id, channel));
	}
}

void ModelCompiler::ReadHierarchyData(AssimpNodeData& _dest, const aiNode* _src)
{
	E_ASSERT(_src, "Invalid node!");

	_dest.name = _src->mName.data;
	_dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(_src->mTransformation);
	_dest.childrenCount = _src->mNumChildren;

	for (unsigned int i = 0; i < _src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, _src->mChildren[i]);
		_dest.children.push_back(newData);
	}
}

void ModelCompiler::SerializeBinaryGeom(const std::filesystem::path& _filePath)
{
	std::filesystem::path filePath{ _filePath };
	filePath.replace_extension(".model");

	std::ofstream serializeFile(filePath, std::ios_base::binary);
	E_ASSERT(serializeFile, "Could not open output file to serialize geom!");

	// Meshes
	SerializeBinaryMeshes(serializeFile);

	// Materials
	SerializeBinaryMaterials(serializeFile);

	// Animations
	serializeFile.write(reinterpret_cast<char*>(&hasAnimation), sizeof(hasAnimation));
	if (hasAnimation)
		SerializeBinaryAnimations(serializeFile);
	
	serializeFile.flush();
	serializeFile.close();
}

// Serialize Meshes
void ModelCompiler::SerializeBinaryMeshes(std::ofstream& _serializeFile)
{
	size_t meshSize = pModel->meshes.size();
	_serializeFile.write(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));

	for (auto& _mesh : pModel->meshes)
	{
		// Vertices
		size_t vertexSize = _mesh._vertices.size();
		_serializeFile.write(reinterpret_cast<char*>(&vertexSize), sizeof(vertexSize));
		_serializeFile.write(reinterpret_cast<char*>(&_mesh._vertices[0]), vertexSize * sizeof(Vertex));

		// Indices
		size_t indicesSize = _mesh._indices.size();
		_serializeFile.write(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
		_serializeFile.write(reinterpret_cast<char*>(&_mesh._indices[0]), indicesSize * sizeof(std::uint16_t));

		_serializeFile.write(reinterpret_cast<char*>(&_mesh.materialIndex), sizeof(_mesh.materialIndex));// Material index

		_serializeFile.write(reinterpret_cast<char*>(&_mesh.mPosCompressionScale), sizeof(glm::vec3));	// Position scale
		_serializeFile.write(reinterpret_cast<char*>(&_mesh.mTexCompressionScale), sizeof(glm::vec2));   // Texture scale

		_serializeFile.write(reinterpret_cast<char*>(&_mesh.mPosCompressionOffset), sizeof(glm::vec3));	// Position offset
		_serializeFile.write(reinterpret_cast<char*>(&_mesh.mTexCompressionOffset), sizeof(glm::vec2));	// Texture offset

		// Number of bones
		_serializeFile.write(reinterpret_cast<char*>(&_mesh.numBones), sizeof(_mesh.numBones));	// Number of bones

		// Bone info for each vertex for this mesh
		if(_mesh.numBones) // Check if there is bones within this mesh
			_serializeFile.write(reinterpret_cast<char*>(&_mesh._boneInfo[0]), vertexSize * sizeof(VertexBoneInfo));
	}
}

void ModelCompiler::SerializeBinaryMaterials(std::ofstream& _serializeFile)
{
	size_t materialSize = pModel->materials.size();
	_serializeFile.write(reinterpret_cast<char*>(&materialSize), sizeof(materialSize));

	for (auto& mat : pModel->materials) // Save material of this model
	{
		_serializeFile.write(reinterpret_cast<char*>(&mat.Specular), sizeof(aiColor4D));
		_serializeFile.write(reinterpret_cast<char*>(&mat.Diffuse), sizeof(aiColor4D));
		_serializeFile.write(reinterpret_cast<char*>(&mat.Ambient), sizeof(aiColor4D));

		//size_t texSize = mat.textures.size(); // Save all textures of this material
		//_serializeFile.write(reinterpret_cast<char*>(&texSize), sizeof(texSize));
		//if (texSize > 0)
		//{
		//	_serializeFile.write(reinterpret_cast<char*>(&mat.textures[0]), texSize * sizeof(Texture));
		//}
	}
}

void ModelCompiler::SerializeBinaryAnimations(std::ofstream& _serializeFile)
{
	//size_t animationSize = pModel->animations.GetAnimations().size();
	size_t animationSize = 1;
	_serializeFile.write(reinterpret_cast<char*>(&animationSize), sizeof(animationSize));

	Animation& animation = pModel->animations[0];

	_serializeFile.write(reinterpret_cast<char*>(&animation.GetDuration()), sizeof(animation.GetDuration()));
	_serializeFile.write(reinterpret_cast<char*>(&animation.GetTicksPerSecond()), sizeof(animation.GetTicksPerSecond()));

	// Bones
	size_t boneSize = animation.GetBones().size();
	_serializeFile.write(reinterpret_cast<char*>(&boneSize), sizeof(boneSize));

	for (auto& bone : animation.GetBones())
	{
		// Position
		_serializeFile.write(reinterpret_cast<char*>(&bone.m_NumPositions), sizeof(bone.m_NumPositions));
		_serializeFile.write(reinterpret_cast<char*>(&bone.m_Positions[0]), bone.m_NumPositions * sizeof(KeyPosition));

		// Rotation
		_serializeFile.write(reinterpret_cast<char*>(&bone.m_NumRotations), sizeof(bone.m_NumRotations));
		_serializeFile.write(reinterpret_cast<char*>(&bone.m_Rotations[0]), bone.m_NumRotations * sizeof(KeyRotation));

		// Scale
		_serializeFile.write(reinterpret_cast<char*>(&bone.m_NumScalings), sizeof(bone.m_NumScalings));
		_serializeFile.write(reinterpret_cast<char*>(&bone.m_Scales[0]), bone.m_NumScalings * sizeof(KeyScale));

		// Local Transform
		_serializeFile.write(reinterpret_cast<char*>(&bone.m_LocalTransform), sizeof(glm::mat4));

		// Name of Bone
		size_t nameSize = bone.m_Name.length();
		_serializeFile.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
		_serializeFile.write(reinterpret_cast<char*>(&bone.m_Name[0]), nameSize * sizeof(char));

		// Bone ID
		_serializeFile.write(reinterpret_cast<char*>(&bone.m_ID), sizeof(bone.m_ID));
	}

	// AssimpNodeData
	AssimpNodeData& nodeData = animation.GetRootNode();
	SerializeBinaryRecursiveNode(_serializeFile, nodeData);

	// Bone Info Map
	size_t boneInfoSize = animation.GetBoneInfoMap().size();
	_serializeFile.write(reinterpret_cast<char*>(&boneInfoSize), sizeof(boneInfoSize));

	for (auto it = animation.GetBoneInfoMap().begin(); it != animation.GetBoneInfoMap().end(); ++it)
	{
		// Kay of BoneInfoMap
		size_t keySize = it->first.length();
		_serializeFile.write(reinterpret_cast<char*>(&keySize), sizeof(keySize));
		std::string key = it->first;
		_serializeFile.write(reinterpret_cast<char*>(&key[0]), keySize * sizeof(char));

		// Value of BoneInfoMap
		BoneInfo boneInfo = it->second;
		_serializeFile.write(reinterpret_cast<char*>(&boneInfo), sizeof(BoneInfo));
	}

	_serializeFile.write(reinterpret_cast<char*>(&animation.GetBoneCount()), sizeof(BoneInfo));
}

void ModelCompiler::SerializeBinaryRecursiveNode(std::ofstream& _serializeFile, AssimpNodeData& _nodeData)
{
	_serializeFile.write(reinterpret_cast<char*>(&_nodeData.transformation), sizeof(glm::mat4)); // Transformation

	size_t nameSize = _nodeData.name.length();
	_serializeFile.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
	_serializeFile.write(reinterpret_cast<char*>(&_nodeData.name[0]), nameSize * sizeof(char)); // Name of Node

	_serializeFile.write(reinterpret_cast<char*>(&_nodeData.childrenCount), sizeof(_nodeData.childrenCount)); // Num of children

	for (auto& child : _nodeData.children)
	{
		SerializeBinaryRecursiveNode(_serializeFile, child);
	}
}

void ModelCompiler::CheckExtension(const std::filesystem::path& _filePath)
{
	namespace fs = std::filesystem;
	fs::path extension = _filePath.extension();

	if (extension != ".fbx" && extension != ".obj")
		E_ASSERT(false, "Error: Invalid file type!!");

	PRINT("Compiling ", _filePath.filename(), "...\n");
}
