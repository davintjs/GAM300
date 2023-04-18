/*!************************************************************************
\file               Model.cpp
\author             Lian Khai Kiat
\par DP email:      l.kiat\@digipen.edu
\par Course:        CSD2150
\date               07/04/2023
\brief
Implementation of Assimp Loader
**************************************************************************/

#include "Compiler.h"

AssimpLoader::AssimpLoader(const std::string descriptorFilePath, const std::string geomFilePath)
{
	_descriptor = new Descriptor;

	DeserializeDescriptor(descriptorFilePath); // desialize to _descriptor
	LoadModel();
	SerializeBinaryGeom(geomFilePath);
}

AssimpLoader::~AssimpLoader()
{
	if (_descriptor)
	{
		delete _descriptor;
		// delete[] _descriptor;
	}
}

void AssimpLoader::LoadModel()
{
	Assimp::Importer assimpImporter;
	uint32_t ImportOptions{};
	ImportOptions =
		aiPostProcessSteps::aiProcess_CalcTangentSpace |			// Calculates the tangents and bitangents for the imported meshes.
		aiPostProcessSteps::aiProcess_Triangulate |					// Triangulates all faces of all meshes
		aiPostProcessSteps::aiProcess_JoinIdenticalVertices |		// Identifies and joins identical vertex data sets within all imported meshes
		aiPostProcessSteps::aiProcess_LimitBoneWeights |			// for skin model max;
		aiPostProcessSteps::aiProcess_GenUVCoords |					// Convert ro proper UV coordinate channel
		aiPostProcessSteps::aiProcess_TransformUVCoords |			// apply UV projection
		aiPostProcessSteps::aiProcess_FlipUVs |						// flips all UV coordinates along the y-axis and adjusts
		aiPostProcessSteps::aiProcess_FindInstances |				// searches for duplicate meshes and replaces them with references to the first mesh
		aiPostProcessSteps::aiProcess_RemoveRedundantMaterials |	// remove unreferenced material
		aiPostProcessSteps::aiProcess_FindInvalidData;				// remove or fix invalid data

	if (_descriptor->combine)
	{
		ImportOptions |= aiPostProcessSteps::aiProcess_PreTransformVertices;
	}

	// import fbx
	const aiScene* scene = assimpImporter.ReadFile(_descriptor->filePath, ImportOptions);
	if (!scene)
	{
		std::cout << "Error reading file into scene!!" << std::endl;
		exit(EXIT_FAILURE);
	}
	ProcessGeom(*scene->mRootNode, *scene);
}

void AssimpLoader::ProcessGeom(const aiNode& node, const aiScene& scene)
{
	for (unsigned int i = 0; i < node.mNumMeshes; ++i) // Loop through node meshes
	{
		aiMesh* pMesh = scene.mMeshes[node.mMeshes[i]];
		//_meshes.push_back(ProcessMesh(*pMesh, scene));
	}
	for (unsigned int j = 0; j < node.mNumChildren; ++j) // Initialize all the children nodes
	{
		ProcessGeom(*node.mChildren[j], scene);
	}
}

//Mesh AssimpLoader::ProcessMesh(const aiMesh& mesh, const aiScene& scene)
//{
//	for (unsigned int i = 0; i < mesh.mNumVertices; ++i) // Vertices
//	{
//		Vertex temp;
//
//		temp.pos = xcore::vector3(static_cast<float>(mesh.mVertices[i].x),
//			static_cast<float>(mesh.mVertices[i].y),
//			static_cast<float>(mesh.mVertices[i].z));
//
//		// normal vectors
//		temp.normal = xcore::vector3d(static_cast<float>(mesh.mNormals[i].x),
//			static_cast<float>(mesh.mNormals[i].y),
//			static_cast<float>(mesh.mNormals[i].z));
//
//		if (mesh.HasTextureCoords(0))
//		{
//			temp.tex = xcore::vector2(static_cast<float>(mesh.mTextureCoords[0][i].x),
//				static_cast<float>(mesh.mTextureCoords[0][i].y));
//		}
//		if (mesh.HasVertexColors(0))
//		{
//			temp.color = xcore::icolor(xcore::vector4(static_cast<float>(mesh.mColors[0][i].r),
//				static_cast<float>(mesh.mColors[0][i].g),
//				static_cast<float>(mesh.mColors[0][i].b),
//				static_cast<float>(mesh.mColors[0][i].a)));
//		}
//
//		if (mesh.mTangents != nullptr)
//		{
//			temp.tangent = {
//				static_cast<float>(mesh.mTangents[i].x),
//				static_cast<float>(mesh.mTangents[i].y),
//				static_cast<float>(mesh.mTangents[i].z)
//			};
//		}
//		_vertices.push_back(temp); // Add this vertex into our vector of vertices
//	}
//
//	for (unsigned int j = 0; j < mesh.mNumFaces; ++j) // Faces
//	{
//		const aiFace& face = mesh.mFaces[j];
//
//		for (unsigned int k = 0; k < face.mNumIndices; ++k) // Loop through all indices in each face
//		{
//			_indices.push_back(face.mIndices[k]); // Store the indices in our vector of indices
//		}
//	}
//
//	if (mesh.mMaterialIndex >= 0) // Material
//	{
//		const aiMaterial& mat = *scene.mMaterials[mesh.mMaterialIndex];
//		ImportMaterialAndTextures(mat, scene);
//	}
//
//	Optimize(); // Optimize before storing
//
//	int materialIndex = static_cast<int>(_materials.size() - 1);
//	return Mesh(this->_vertices, this->_indices, materialIndex);
//}

//void AssimpLoader::Optimize()
//{
//	meshopt_optimizeVertexCache(_indices.data(), _indices.data(), _indices.size(), _vertices.size());
//	meshopt_optimizeVertexFetch(_vertices.data(), _indices.data(), _indices.size(), _vertices.data(), _vertices.size(), sizeof(Vertex));
//
//	TransformVertices();
//}

void AssimpLoader::TransformVertices() // Apply the modifications to our vertices from desc to our geom
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

	/*for (size_t i = 0; i < _vertices.size(); ++i)
	{
		glm::vec3 resultant = concat * _vertices[i].pos;
		_vertices[i].pos = resultant;
	}*/
}

void AssimpLoader::ImportMaterialAndTextures(const aiMaterial& material, const aiScene& scene)
{
	_materials.emplace_back();
	Material& mat = _materials[0];
	mat.GUID = reinterpret_cast<std::size_t>(&material);

	for (unsigned int l = 0; l < material.mNumProperties; ++l) // Loop through all properties of this material
	{
		const aiMaterialProperty& matProperty = *material.mProperties[l];
		if (matProperty.mType != aiPTI_String)
		{
			continue;
		}

		aiString* pString = reinterpret_cast<aiString*>(matProperty.mData); // Cast bits to aiString*

		if (matProperty.mSemantic == aiTextureType_NONE)
		{
			mat.matName = pString->C_Str();
			continue;
		}

		SampleHolder sample;
		Texture texture;

		// Setting hint type
		/*if (matProperty.mSemantic == aiTextureType_NORMAL_CAMERA || xcore::string::FindStrI(pString->C_Str(), "_Normal") != -1)
		{
			sample.type = "PBR_NORMAL";
		}
		else if (matProperty.mSemantic == aiTextureType_BASE_COLOR || xcore::string::FindStrI(pString->C_Str(), "_Base_Color") != -1)
		{
			sample.type = "PBR_ALBEDO";
		}
		else if (matProperty.mSemantic == aiTextureType_AMBIENT_OCCLUSION || xcore::string::FindStrI(pString->C_Str(), "_AO") != -1)
		{
			sample.type = "PBR_OCCLUSION";
		}
		else if (matProperty.mSemantic == aiTextureType_DIFFUSE_ROUGHNESS || xcore::string::FindStrI(pString->C_Str(), "_Roughness") != -1)
		{
			sample.type = "PBR_ROUGHNESS";
		}
		else
		{
			sample.type = "UNKNOWN";
		}*/

		texture.filepath = pString->C_Str(); // Set file path of texture

		int  iTexture = 0;
		bool bFound = false;
		for (const auto& Tex : _textures)
		{
			if (Tex.filepath == texture.filepath)
			{
				bFound = true;
				break;
			}
			++iTexture;
		}

		sample.binding = iTexture; // Set sampler texture binding index wrt to textures vector size
		mat._samples.push_back(sample); // Set the texture index
		if (bFound == false)
		{
			_textures.push_back(texture); // Add this texture to our vector of textures
		}

	}
	return;
}

void AssimpLoader::SerializeDescriptor(const std::string filepath)
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> jason(buffer);

	jason.StartObject();

	jason.String("Mesh FilePath");
	jason.String(_descriptor->filePath.c_str());

	jason.String("Mesh Scale");
	jason.StartArray();
	jason.Double(_descriptor->scale.x);
	jason.Double(_descriptor->scale.y);
	jason.Double(_descriptor->scale.z);
	jason.EndArray();

	jason.String("Mesh Rotation");
	jason.StartArray();
	jason.Double(_descriptor->rotate.x);
	jason.Double(_descriptor->rotate.y);
	jason.Double(_descriptor->rotate.z);
	jason.EndArray();

	jason.String("Mesh Translation");
	jason.StartArray();
	jason.Double(_descriptor->translate.x);
	jason.Double(_descriptor->translate.y);
	jason.Double(_descriptor->translate.z);
	jason.EndArray();

	jason.String("Mesh MergeMeshes");
	jason.Bool(_descriptor->combine);

	jason.String("Mesh MeshName");
	jason.String(_descriptor->meshName.c_str());

	jason.EndObject();

	std::ofstream serializeFile(filepath);
	if (!serializeFile)
	{
		std::cerr << "Cannot open the output file." << std::endl;
		return;
	}

	serializeFile << buffer.GetString();
	serializeFile.flush();
	serializeFile.close();
}

void AssimpLoader::SerializeBinaryGeom(const std::string filepath) // Serialize to geom binary file
{
	//std::ofstream serializeFile(filepath, std::ios_base::binary);
	//if (!serializeFile)
	//{
	//	std::cerr << "Could not open output file to serialize geom!" << std::endl;
	//	return;
	//}

	///*size_t meshSize = _meshes.size();
	//serializeFile.write(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));*/

	//size_t vertexSize = _vertices.size();
	//serializeFile.write(reinterpret_cast<char*>(&vertexSize), sizeof(vertexSize));
	//serializeFile.write(reinterpret_cast<char*>(&_vertices[0]), vertexSize * sizeof(Vertex));

	//size_t indicesSize = _indices.size();
	//serializeFile.write(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
	//serializeFile.write(reinterpret_cast<char*>(&_indices[0]), indicesSize * sizeof(int32_t));

	//size_t texSize = _textures.size();
	//serializeFile.write(reinterpret_cast<char*>(&texSize), sizeof(texSize));
	//serializeFile.write(reinterpret_cast<char*>(&_textures[0]), texSize * sizeof(Texture));

	//size_t matSize = _materials.size();
	//serializeFile.write(reinterpret_cast<char*>(&matSize), sizeof(matSize));
	//serializeFile.write(reinterpret_cast<char*>(&_materials[0]), matSize * sizeof(Material));

	//serializeFile.flush();
	//serializeFile.close();
}

void AssimpLoader::DeserializeDescriptor(const std::string filepath)
{
	// Takes in the _descriptor file path and load assimp _descriptor struct with data before loading the assimp

	std::ifstream file(filepath);
	if(!file.is_open()){
		std::cout << "euan good boi\n";
	}
	rapidjson::IStreamWrapper streamWrapper(file);

	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	_descriptor->filePath = doc["Mesh FilePath"].GetString(); // File path
	//_descriptor->filePath = "";

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


	_descriptor->meshName = doc["Mesh MeshName"].GetString();
	_descriptor->combine = doc["Mesh MergeMeshes"].GetBool();
}

int main() {
	AssimpLoader assimp("../GAM300/Assets/Models/Skull_textured.geom.desc","../GAM300/Assets/Models/Skull_textured.geom");
	return 0;
}