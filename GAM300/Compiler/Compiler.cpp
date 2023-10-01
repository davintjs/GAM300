/*!***************************************************************************************
\file			Compiler.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

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

#include "Compiler.h"

#pragma warning( disable : 4100)
ModelLoader::ModelLoader(const std::string descriptorFilePath, const std::string geomFilePath)
{
	_descriptor = new Descriptor;

	DeserializeDescriptor(descriptorFilePath);
	LoadModel();
	SerializeBinaryGeom(geomFilePath);
}

ModelLoader::~ModelLoader()
{
	if (_descriptor)
	{
		delete _descriptor;
	}
}

void ModelLoader::LoadModel()
{
	Assimp::Importer assimpImporter;
	uint32_t ImportOptions{};
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
		aiPostProcessSteps::aiProcess_RemoveRedundantMaterials |	// remove unreferenced material
		aiPostProcessSteps::aiProcess_FindInvalidData 			// remove or fix invalid data
	
	| aiPostProcessSteps::aiProcess_PreTransformVertices;

	// Import fbx
	const aiScene* scene = assimpImporter.ReadFile(_descriptor->filePath, ImportOptions);
	if (!scene)
	{
		std::cout << "Error reading file into scene!!" << std::endl;
		exit(EXIT_FAILURE);
	}

	//ProcessBones(*scene->mRootNode, *scene); // WIP
	ProcessGeom(*scene->mRootNode, *scene);
}

void ModelLoader::ProcessBones(const aiNode& node, const aiScene& scene)
{
	std::unordered_map<std::string, const aiNode*> nodeName;
	std::unordered_map<std::string, const aiBone*> boneName;

	for (unsigned int i = 0; i < scene.mNumMeshes; ++i) // Looping through all meshes in the scene to find all bones
	{
		const aiMesh& mesh = *scene.mMeshes[i];
		for (unsigned int j = 0; j < mesh.mNumBones; ++j) // Looping through all bones in the current mesh
		{
			const aiBone& bone = *mesh.mBones[j];
			if (auto go = boneName.find(bone.mName.data); go == boneName.end()) // Found a new bone, add it into our map
			{
				auto pNode = scene.mRootNode->FindNode(bone.mName);
				boneName[bone.mName.data] = &bone; // Initialize the map using the bone name and its corresponding aiBone
				nodeName[bone.mName.data] = pNode; // Initialize the map using the bone name and its corresponding node
			}
		}
	}

	for (unsigned int i = 0; i < scene.mNumAnimations; ++i) // Looping through available animations for this model
	{
		const aiAnimation& currAnimation = *scene.mAnimations[i];
		for (unsigned int j = 0; j < currAnimation.mNumChannels; ++j) // Looping through this animation channels
		{
			aiNodeAnim& currChannel = *currAnimation.mChannels[j];

			if (auto go = nodeName.find(currChannel.mNodeName.data); go == nodeName.end())
			{
				nodeName.insert({ currChannel.mNodeName.data, scene.mRootNode->FindNode(currChannel.mNodeName) });
			}
		}
	}

	struct BoneNode
	{
		const aiNode* m_AssimpNode{ nullptr };
		int             m_Depth{ 0 };
		int             m_TotalChildren{ 0 };
		int             m_Children{ 0 };
	};

	std::vector<BoneNode> totalNodes(nodeName.size());
	
	int temp = 0;
	for (auto itr = nodeName.begin(); itr != nodeName.end(); ++itr)
	{
		auto& currNode = totalNodes[temp++];
		currNode.m_AssimpNode = itr->second;
	}

	for (unsigned int i = 0; i < totalNodes.size(); ++i)
	{
		auto& currNode = totalNodes[i];
		bool foundParent = false;

		for (aiNode* pNode = currNode.m_AssimpNode->mParent; pNode != nullptr; pNode = pNode->mParent)
		{
			++currNode.m_Depth;

			for (size_t j = 0; j < totalNodes.size(); ++j)
			{
				auto& parentNode = totalNodes[j];
				if (pNode == parentNode.m_AssimpNode)
				{
					++parentNode.m_TotalChildren;
					if (foundParent == false)
					{
						++parentNode.m_Children;
					}
					foundParent = true;
					break;
				}
			}
		}
	}

	// Sort the nodes in ascending order
	std::qsort(totalNodes.data(),
		totalNodes.size(),
		sizeof(BoneNode),
		[](const void* x, const void* y)
		{
			const auto& A = *reinterpret_cast<const BoneNode*>(x);
			const auto& B = *reinterpret_cast<const BoneNode*>(y);

			if (A.m_Depth < B.m_Depth)
			{
				return -1;
			}
			else if (A.m_Depth > B.m_Depth)
			{
				return 1;
			}

			if (A.m_TotalChildren < B.m_TotalChildren)
			{
				return -1;
			}

			return static_cast<int>((A.m_TotalChildren > B.m_TotalChildren));
		});

	// Creating the bones

}

void ModelLoader::ProcessGeom(const aiNode& node, const aiScene& scene)
{
	BoundingBox3D mGlobalPosAABB;
	BoundingBox3D mGlobalTexAABB;

	for (unsigned int i = 0; i < node.mNumMeshes; ++i) // Loop through node meshes
	{
		aiMesh* pMesh = scene.mMeshes[node.mMeshes[i]];
		_meshes.push_back(ProcessMesh(*pMesh, scene));
	}
	for (unsigned int j = 0; j < node.mNumChildren; ++j) // Initialize all the children nodes
	{
		ProcessGeom(*node.mChildren[j], scene);
	}
}

Geom_Mesh ModelLoader::ProcessMesh(const aiMesh& mesh, const aiScene& scene)
{
	std::vector<TempVertex> tempVertex;
	std::vector<unsigned int> tempIndices;

	for (unsigned int i = 0; i < mesh.mNumVertices; ++i) // Processing all vertices in this single mesh
	{
		TempVertex temp;

		// Vertex position
		temp.pos = glm::vec3(static_cast<float>(mesh.mVertices[i].x),
							static_cast<float>(mesh.mVertices[i].y),
							static_cast<float>(mesh.mVertices[i].z));

		// Vertex normal
		temp.normal = glm::vec3(static_cast<float>(mesh.mNormals[i].x),
								static_cast<float>(mesh.mNormals[i].y),
								static_cast<float>(mesh.mNormals[i].z));

		if (mesh.HasTextureCoords(0)) // Vertex Texture 
		{
			temp.tex = glm::vec2(static_cast<float>(mesh.mTextureCoords[0][i].x),
								static_cast<float>(mesh.mTextureCoords[0][i].y));
		}

		if (mesh.HasVertexColors(0)) // Vertex Color
		{
			temp.color = glm::vec4(static_cast<float>(mesh.mColors[0][i].r),
									static_cast<float>(mesh.mColors[0][i].g),
									static_cast<float>(mesh.mColors[0][i].b),
									static_cast<float>(mesh.mColors[0][i].a));
		}

		if (mesh.mTangents != nullptr) // Vertex tangent
		{
			temp.tangent = {
				static_cast<float>(mesh.mTangents[i].x),
				static_cast<float>(mesh.mTangents[i].y),
				static_cast<float>(mesh.mTangents[i].z)
			};
		}
		tempVertex.push_back(temp); // Add this vertex into our vector of vertices
	}

	for (unsigned int j = 0; j < mesh.mNumFaces; ++j) // Processing all faces in this single mesh
	{
		const aiFace& face = mesh.mFaces[j];

		for (unsigned int k = 0; k < face.mNumIndices; ++k) // Loop through all indices in this current face
		{
			tempIndices.push_back(face.mIndices[k]); // Store the indices in our vector of indices
		}
	}

	if (mesh.mMaterialIndex >= 0) // Import the mesh material if it exists
	{
		const aiMaterial& mat = *scene.mMaterials[mesh.mMaterialIndex];
		ImportMaterialAndTextures(mat);
	}

	TransformVertices(tempVertex); // Apply transformation on the mesh according to descriptor file specifications

	Optimize(tempVertex, tempIndices); // Optimize this mesh

	// Compress vertices for storing in our vertex
	std::vector<Vertex> CompressedVertices;
	std::pair<glm::vec3, glm::vec2> mPosTexOffset;
	std::pair<glm::vec3, glm::vec2> mPosTexScale;
	CompressVertices(CompressedVertices, tempVertex, mPosTexOffset, mPosTexScale);

	// Calculate the material index of this mesh
	int materialIndex = static_cast<int>(_materials.size() - 1);

	return Geom_Mesh(CompressedVertices, tempIndices, materialIndex, mPosTexScale.first, mPosTexScale.second, mPosTexOffset.first, mPosTexOffset.second); // Create this mesh
}

void ModelLoader::Optimize(std::vector<TempVertex>& vert, std::vector<unsigned int>& ind)
{
	std::vector<unsigned int> remap(ind.size());
	const size_t vertCount = meshopt_generateVertexRemap(&remap[0],
		ind.data(),
		ind.size(),
		vert.data(),
		ind.size(),
		sizeof(TempVertex));

	std::vector<unsigned int> remappedIndices(ind.size());
	std::vector<TempVertex> remappedVertices(vertCount);

	meshopt_remapIndexBuffer(remappedIndices.data(), ind.data(), ind.size(), &remap[0]);
	meshopt_remapVertexBuffer(remappedVertices.data(), vert.data(), vert.size(), sizeof(TempVertex), &remap[0]);

	meshopt_optimizeVertexCache(remappedIndices.data(), remappedIndices.data(), ind.size(), vertCount);

	meshopt_optimizeOverdraw(remappedIndices.data(),
							remappedIndices.data(),
							ind.size(),
							&remappedVertices[0].pos.x,
							vertCount,
							sizeof(TempVertex),
							1.05f);

	meshopt_optimizeVertexFetch(remappedVertices.data(), 
								remappedIndices.data(), 
								ind.size(),
								remappedVertices.data(), 
								vertCount, 
								sizeof(TempVertex));

	// Below is for LOD of meshes
		
	//const float threshold = 0.2f; // Controls the LOD of the mesh
	//const size_t targetIndexCount = size_t(remappedIndices.size() * threshold);
	//const float targetError = 1e-2f;
	//std::vector<unsigned int> indicesLod(remappedIndices.size());
	//indicesLod.resize(meshopt_simplify(&indicesLod[0],
	//									remappedIndices.data(), 
	//									remappedIndices.size(),
	//									&remappedVertices[0].pos.x, 
	//									vertCount, 
	//									sizeof(Vertex), 
	//									targetIndexCount,
	//									targetError));

	vert = remappedVertices;
	ind = remappedIndices;
}

// Basically changing floats to 2 bytes integers
void ModelLoader::CompressVertices(std::vector<Vertex>& CompressedVertices,
	const std::vector<TempVertex> tempVertex,
	std::pair<glm::vec3, glm::vec2>& mOffsets,
	std::pair<glm::vec3, glm::vec2>& mScales)
{
	//for (const auto& vert : tempVertex)
	//{
	//	Vertex currVert;
	//	currVert.pos.x = vert.pos.x;
	//	currVert.pos.y = vert.pos.y;
	//	currVert.pos.z = vert.pos.z;
	//	currVert.normal.x = vert.normal.x;
	//	currVert.normal.y = vert.normal.y;
	//	currVert.normal.z = vert.normal.z;
	//	currVert.tangent.x = vert.tangent.x;
	//	currVert.tangent.y = vert.tangent.y;
	//	currVert.tangent.z = vert.tangent.z;
	//	//currVert.tanSign = vert.tangent.z >= 0 ? 0x1 : 0x3;
	//	currVert.tex.x = vert.tex.x;
	//	currVert.tex.y = vert.tex.y;
	//	currVert.color.r = vert.color.r;
	//	currVert.color.g = vert.color.g;
	//	currVert.color.b = vert.color.b;
	//	currVert.color.a = vert.color.a;

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
		mPosMinX = std::min(mPosMinX, v.pos.x);
		mPosMinY = std::min(mPosMinY, v.pos.y);
		mPosMinZ = std::min(mPosMinZ, v.pos.z);
		mPosMaxX = std::max(mPosMaxX, v.pos.x);
		mPosMaxY = std::max(mPosMaxY, v.pos.y);
		mPosMaxZ = std::max(mPosMaxZ, v.pos.z);

		// Texture
		mTexMinU = std::min(mTexMinU, v.tex.x);
		mTexMinV = std::min(mTexMinV, v.tex.y);
		mTexMaxU = std::max(mTexMaxU, v.tex.x);
		mTexMaxV = std::max(mTexMaxV, v.tex.y);
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
	mScales.first = mPosAABB.mMax - mPosAABB.mMin;
	mScales.second = mTexAABB.mMax - mTexAABB.mMin;

	// Get the offsets of this submesh
	mOffsets.first = (mPosAABB.mMin + mPosAABB.mMax) / 2.f;
	mOffsets.second = (mTexAABB.mMin + mTexAABB.mMax) / 2.f;

	// Compressing the vertices here
	for (const auto& vert : tempVertex)
	{
		float val;
		Vertex currVert;

		// Position
		val = (vert.pos.x - mOffsets.first.x) / mScales.first.x;
		currVert.posX = static_cast<std::int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000); // Multiply by maximum + value or minimum - value

		val = (vert.pos.y - mOffsets.first.y) / mScales.first.y;
		currVert.posY = static_cast<std::int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000);

		val = (vert.pos.z - mOffsets.first.z) / mScales.first.z;
		currVert.posZ = static_cast<std::int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000);

		// Texture
		val = (vert.tex.x - mOffsets.second.x) / mScales.second.x;
		currVert.texU = static_cast<int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000);

		val = (vert.tex.y - mOffsets.second.y) / mScales.second.y;
		currVert.texV = static_cast<int16_t>(val >= 0 ? val * 0x7FFF : val * 0x8000);

		// Normals
		currVert.normX = static_cast<std::int16_t>(vert.normal.x >= 0 ? vert.normal.x * 0x7FFF : vert.normal.x * 0x8000);
		currVert.normY = static_cast<std::int16_t>(vert.normal.y >= 0 ? vert.normal.y * 0x7FFF : vert.normal.y * 0x8000);
		currVert.normZ = static_cast<std::int16_t>(vert.normal.z >= 0 ? vert.normal.z * 0x7FFF : vert.normal.z * 0x8000);

		// Tangent
		currVert.tanX = static_cast<std::int16_t>(vert.tangent.x >= 0 ? vert.tangent.x * 0x7FFF : vert.tangent.x * 0x8000);
		currVert.tanY = static_cast<std::int16_t>(vert.tangent.y >= 0 ? vert.tangent.y * 0x7FFF : vert.tangent.y * 0x8000);
		currVert.tanZ = static_cast<std::int16_t>(vert.tangent.z >= 0 ? vert.tangent.z * 0x7FFF : vert.tangent.z * 0x8000);

		// Color
		currVert.colorR = static_cast<std::int8_t>(vert.color.r);
		currVert.colorG = static_cast<std::int8_t>(vert.color.g);
		currVert.colorB = static_cast<std::int8_t>(vert.color.b);
		currVert.colorA = static_cast<std::int8_t>(vert.color.a);

		CompressedVertices.push_back(currVert);
	}
}

void ModelLoader::TransformVertices(std::vector<TempVertex> vert) // Apply the modifications to our vertices from desc to our geom
{
	glm::mat4 scaleMat
	{
		_descriptor->scale.x, 0.f, 0.f, 0.f,
		0.f, _descriptor->scale.y, 0.f, 0.f,
		0.f, 0.f, _descriptor->scale.z, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	glm::mat4 rotX
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, cos(glm::radians(_descriptor->rotate.x)), sin(glm::radians(_descriptor->rotate.x)), 0.f,
		0.f, -sin(glm::radians(_descriptor->rotate.x)), cos(glm::radians(_descriptor->rotate.x)), 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	glm::mat4 rotY
	{
		cos(glm::radians(_descriptor->rotate.y)), 0.f, -sin(glm::radians(_descriptor->rotate.y)), 0.f,
		0.f, 1.f, 0.f, 0.f,
		sin(glm::radians(_descriptor->rotate.y)), 0.f, cos(glm::radians(_descriptor->rotate.y)), 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	glm::mat4 rotZ
	{
		cos(glm::radians(_descriptor->rotate.z)), sin(glm::radians(_descriptor->rotate.z)), 0.f, 0.f,
		-sin(glm::radians(_descriptor->rotate.z)), cos(glm::radians(_descriptor->rotate.z)), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	glm::mat4 transMat
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		_descriptor->translate.x, _descriptor->translate.y, _descriptor->translate.z, 1.f
	};

	glm::mat4 rotMat = rotZ * rotY * rotX;
	glm::mat4 concat = transMat * rotMat * scaleMat;

	for (size_t i = 0; i < vert.size(); ++i)
	{
		glm::vec3 resultant = concat * glm::vec4(vert[i].pos, 0.f);
		vert[i].pos = resultant;
	}
}

void ModelLoader::ImportMaterialAndTextures(const aiMaterial& material)
{
	// Materials
	aiColor4D specularColor;
	aiColor4D diffuseColor;
	aiColor4D ambientColor;
	float shininess;

	aiGetMaterialColor(&material, AI_MATKEY_COLOR_SPECULAR, &specularColor);
	aiGetMaterialColor(&material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
	aiGetMaterialColor(&material, AI_MATKEY_COLOR_AMBIENT, &ambientColor);
	aiGetMaterialFloat(&material, AI_MATKEY_SHININESS, &shininess);

	Material tempMat(
		specularColor.r, specularColor.g, specularColor.b, shininess,
		diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a,
		ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a );

	// Textures
	aiString specName{};
	aiString diffName{};
	aiString ambiName{};

	if (material.GetTextureCount(aiTextureType_SPECULAR) > 0) // Specular texture
	{
		Texture tempTex;
		material.Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), specName);
		tempTex.filepath = specName.C_Str();

		tempMat.textures.push_back(tempTex);
	}

	if (material.GetTextureCount(aiTextureType_DIFFUSE) > 0) // Diffuse texture
	{
		Texture tempTex;
		material.Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffName);
		tempTex.filepath = diffName.C_Str();

		tempMat.textures.push_back(tempTex);
	}

	if (material.GetTextureCount(aiTextureType_AMBIENT) > 0) // Ambient texture
	{
		Texture tempTex;
		material.Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), ambiName);
		tempTex.filepath = ambiName.C_Str();

		tempMat.textures.push_back(tempTex);
	}

	_materials.push_back(tempMat);

	//_materials.emplace_back();
	//Material& mat = _materials[0];
	//mat.GUID = reinterpret_cast<std::size_t>(&material);

	//for (unsigned int l = 0; l < material.mNumProperties; ++l) // Loop through all properties of this material
	//{
	//	const aiMaterialProperty& matProperty = *material.mProperties[l];
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

// Serialize to geom binary file a single FBX file
void ModelLoader::SerializeBinaryGeom(const std::string filepath)
{
	std::ofstream serializeFile(filepath, std::ios_base::binary);
	if (!serializeFile)
	{
		std::cerr << "Could not open output file to serialize geom!" << std::endl;
		return;
	}

	// Mesh vertices
	size_t meshSize = this->_meshes.size();
	serializeFile.write(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));

	for (auto& _mesh : this->_meshes)
	{
		// Vertices
		size_t vertexSize = _mesh._vertices.size();
		serializeFile.write(reinterpret_cast<char*>(&vertexSize), sizeof(vertexSize));
		serializeFile.write(reinterpret_cast<char*>(&_mesh._vertices[0]), vertexSize * sizeof(Vertex));

		// Indices
		size_t indicesSize = _mesh._indices.size();
		serializeFile.write(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
		serializeFile.write(reinterpret_cast<char*>(&_mesh._indices[0]), indicesSize * sizeof(unsigned int));

		serializeFile.write(reinterpret_cast<char*>(&_mesh.materialIndex), sizeof(_mesh.materialIndex)); // Material index

		serializeFile.write(reinterpret_cast<char*>(&_mesh.mPosCompressionScale), sizeof(glm::vec3));	 // Position scale
		serializeFile.write(reinterpret_cast<char*>(&_mesh.mTexCompressionScale), sizeof(glm::vec2));    // Texture scale

		serializeFile.write(reinterpret_cast<char*>(&_mesh.mPosCompressionOffset), sizeof(glm::vec3));	 // Position offset
		serializeFile.write(reinterpret_cast<char*>(&_mesh.mTexCompressionOffset), sizeof(glm::vec2));	 // Texture offset
	}

	size_t materialSize = this->_materials.size();
	serializeFile.write(reinterpret_cast<char*>(&materialSize), sizeof(materialSize));

	for (auto& mat : _materials) // Save material of this model
	{
		serializeFile.write(reinterpret_cast<char*>(&mat.Specular), sizeof(aiColor4D));
		serializeFile.write(reinterpret_cast<char*>(&mat.Diffuse), sizeof(aiColor4D));
		serializeFile.write(reinterpret_cast<char*>(&mat.Ambient), sizeof(aiColor4D));

		//size_t texSize = mat.textures.size(); // Save all textures of this material
		//serializeFile.write(reinterpret_cast<char*>(&texSize), sizeof(texSize));
		//if (texSize > 0)
		//{
		//	serializeFile.write(reinterpret_cast<char*>(&mat.textures[0]), texSize * sizeof(Texture));
		//}
	}

	serializeFile.flush();
	serializeFile.close();
}

void ModelLoader::DeserializeDescriptor(const std::string filepath)
{
	// Takes in the _descriptor file path and load assimp _descriptor struct with data before loading the assimp

	std::ifstream file(filepath);
	if(!file.is_open()){
		std::cerr << "Unable to open geom descriptor file" << std::endl;
		exit(EXIT_FAILURE);
	}
	rapidjson::IStreamWrapper streamWrapper(file);

	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	_descriptor->filePath = doc["Mesh FilePath"].GetString(); // File path

	float values[3]{ 0.f, 0.f, 0.f };
	int i = 0;

	// Scale
	rapidjson::Value& scale = doc["Mesh Scale"].GetArray();
	for (rapidjson::Value::ConstValueIterator it = scale.Begin(); it != scale.End(); ++it, ++i)
	{
		const rapidjson::Value& v = *it;
		values[i] = v.GetFloat();
	}

	_descriptor->scale.x = values[0];
	_descriptor->scale.y = values[1];
	_descriptor->scale.z = values[2];

	// rotate
	rapidjson::Value& rotate = doc["Mesh Rotation"].GetArray();
	i = 0;
	for (rapidjson::Value::ConstValueIterator it = rotate.Begin(); it != rotate.End(); ++it, ++i)
	{
		const rapidjson::Value& v = *it;
		values[i] = static_cast<float>((v.GetDouble()));
	}

	_descriptor->rotate.x = values[0];
	_descriptor->rotate.y = values[1];
	_descriptor->rotate.z = values[2];

	// translate
	rapidjson::Value& translate = doc["Mesh Translation"].GetArray();
	i = 0;
	for (rapidjson::Value::ConstValueIterator it = translate.Begin(); it != translate.End(); ++it, ++i)
	{
		const rapidjson::Value& v = *it;
		values[i] = static_cast<float>((v.GetDouble()));
	}

	_descriptor->translate.x = values[0];
	_descriptor->translate.y = values[1];
	_descriptor->translate.z = values[2];
}

void CreateDescFile(const std::string fbxFilePath, const std::string writeDescFilePath, const std::string meshFileName)
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

	writer.StartObject();

	writer.String("Mesh FilePath");
	writer.String(fbxFilePath.c_str());

	writer.String("Mesh Scale");
	writer.StartArray();
	writer.Double(1.0);
	writer.Double(1.0);
	writer.Double(1.0);
	writer.EndArray();

	writer.String("Mesh Rotation");
	writer.StartArray();
	writer.Double(10.0);
	writer.Double(78.0);
	writer.Double(0.0);
	writer.EndArray();

	writer.String("Mesh Translation");
	writer.StartArray();
	writer.Double(0.0);
	writer.Double(0.0);
	writer.Double(0.0);
	writer.EndArray();

	writer.EndObject();

	std::ofstream serializeFile(writeDescFilePath);
	if (!serializeFile)
	{
		std::cerr << "Cannot open the output file." << std::endl;
		return;
	}

	serializeFile << buffer.GetString();
	serializeFile.flush();
	serializeFile.close();
}

int main() {
	std::cout << "Compiling models..." << std::endl;

	for (const auto& dir : std::filesystem::recursive_directory_iterator("Assets/Models"))
	{
		if (dir.symlink_status().type() == std::filesystem::file_type::directory) // Is a folder (All should be in folder)
		{
			for (const auto& _dir : std::filesystem::recursive_directory_iterator(dir)) // Looping through contents of the model's folder
			{
				std::string subFilePath = _dir.path().generic_string();
				std::string subFilePathDesc = subFilePath;
				std::string geomFilePath = subFilePath;
				std::string fileType{};
				std::string fileName{};

				// Get the file type of the current file in this folder
				for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
				{
					fileType += subFilePath[i];
				}

				if (strcmp(fileType.c_str(), "fbx")) // Skip this file if not fbx
				{
					continue;
				}

				// Reaching here means we are at the fbx file

				// Get file name
				for (size_t j = subFilePath.find_last_of('/') + 1; j != subFilePath.find_last_of('.'); ++j)
				{
					fileName += subFilePath[j];
				}
				std::cout << "Compiling " << fileName << "...";
				geomFilePath.erase(geomFilePath.find_last_of('.'), strlen(fileType.c_str()) + 1);
				geomFilePath += ".geom";

				subFilePathDesc.erase(subFilePathDesc.find_last_of('.'), strlen(fileType.c_str()) + 1);
				subFilePathDesc += ".geom.desc";

				// Check if this model had already been previously serialized
				if (std::filesystem::exists(geomFilePath) && std::filesystem::exists(subFilePathDesc)) // Both geom and desc files must be present
				{
					std::cout << "Done!" << std::endl;
					break; // Go to next model folder
				}

				// Find the desc file of this model
				if (!std::filesystem::exists(subFilePathDesc))
				{
					// Create desc file for this model
					CreateDescFile(subFilePath, subFilePathDesc, fileName); // fbx filepath & desc filepath & filename parameters
				}

				// Reaching here means the desc file exists, but the model is not serialized yet
				ModelLoader myLoader(subFilePathDesc, geomFilePath);

				// Reaching here means the model is now serialized and now contains desc and geom files
				std::cout << " Done!" << std::endl;
				break; // Go to next model folder (Optimization by skipping rest of files)
			}
		}
	}

	std::cout << "Finished compiling all models!" << std::endl;

	return 0;
}