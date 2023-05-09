#include "Compiler.h"

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
		aiPostProcessSteps::aiProcess_TransformUVCoords |			// apply UV projection
		aiPostProcessSteps::aiProcess_FlipUVs |						// flips all UV coordinates along the y-axis and adjusts
		aiPostProcessSteps::aiProcess_FindInstances |				// searches for duplicate meshes and replaces them with references to the first mesh
		aiPostProcessSteps::aiProcess_RemoveRedundantMaterials |	// remove unreferenced material
		aiPostProcessSteps::aiProcess_FindInvalidData |				// remove or fix invalid data
		aiPostProcessSteps::aiProcess_PreTransformVertices;

	// Import fbx
	const aiScene* scene = assimpImporter.ReadFile(_descriptor->filePath, ImportOptions);
	if (!scene)
	{
		std::cout << "Error reading file into scene!!" << std::endl;
		exit(EXIT_FAILURE);
	}

	ProcessBones(*scene->mRootNode, *scene); // WIP
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

Mesh ModelLoader::ProcessMesh(const aiMesh& mesh, const aiScene& scene)
{
	for (unsigned int i = 0; i < mesh.mNumVertices; ++i) // Processing all vertices in this single mesh
	{
		Vertex temp;

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
		_vertices.push_back(temp); // Add this vertex into our vector of vertices in model loader
	}

	for (unsigned int j = 0; j < mesh.mNumFaces; ++j) // Processing all faces in this single mesh
	{
		const aiFace& face = mesh.mFaces[j];

		for (unsigned int k = 0; k < face.mNumIndices; ++k) // Loop through all indices in this current face
		{
			_indices.push_back(face.mIndices[k]); // Store the indices in our vector of indices in model loader
		}
	}

	if (mesh.mMaterialIndex >= 0) // Import the mesh material if it exists
	{
		const aiMaterial& mat = *scene.mMaterials[mesh.mMaterialIndex];
		ImportMaterialAndTextures(mat);
	}

	//Optimize(); // Optimize before storing
	TransformVertices();

	int materialIndex = static_cast<int>(_materials.size() - 1);
	return Mesh(this->_vertices, this->_indices, materialIndex); // Create the mesh class with model loader data
}

void ModelLoader::Optimize()
{
	meshopt_optimizeVertexCache(_indices.data(), _indices.data(), _indices.size(), _vertices.size());
	meshopt_optimizeVertexFetch(_vertices.data(), _indices.data(), _indices.size(), _vertices.data(), _vertices.size(), sizeof(Vertex));

	TransformVertices();
}

void ModelLoader::TransformVertices() // Apply the modifications to our vertices from desc to our geom
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

	for (size_t i = 0; i < _vertices.size(); ++i)
	{
		glm::vec3 resultant = concat * glm::vec4(_vertices[i].pos, 0.f);
		_vertices[i].pos = resultant;
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

	Material tempMat({
		{specularColor.r, specularColor.g, specularColor.b, shininess},
		{diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a},
		{ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a} });

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

void ModelLoader::SerializeBinaryGeom(const std::string filepath) // Serialize to geom binary file
{
	std::ofstream serializeFile(filepath, std::ios_base::binary);
	if (!serializeFile)
	{
		std::cerr << "Could not open output file to serialize geom!" << std::endl;
		return;
	}

	/*size_t meshSize = _meshes.size();
	serializeFile.write(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));*/

	size_t vertexSize = _vertices.size();
	serializeFile.write(reinterpret_cast<char*>(&vertexSize), sizeof(vertexSize));
	serializeFile.write(reinterpret_cast<char*>(&_vertices[0]), vertexSize * sizeof(Vertex));

	size_t indicesSize = _indices.size();
	serializeFile.write(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
	serializeFile.write(reinterpret_cast<char*>(&_indices[0]), indicesSize * sizeof(int32_t));

	//size_t texSize = _textures.size();
	//serializeFile.write(reinterpret_cast<char*>(&texSize), sizeof(texSize));
	//serializeFile.write(reinterpret_cast<char*>(&_textures[0]), texSize * sizeof(Texture));

	//size_t matSize = _materials.size();
	//serializeFile.write(reinterpret_cast<char*>(&matSize), sizeof(matSize));
	//serializeFile.write(reinterpret_cast<char*>(&_materials[0]), matSize * sizeof(Material));

	for (auto i : _materials) // Save material one at a time
	{
		serializeFile.write(reinterpret_cast<char*>(&i.Specular), sizeof(aiColor4D));
		serializeFile.write(reinterpret_cast<char*>(&i.Diffuse), sizeof(aiColor4D));
		serializeFile.write(reinterpret_cast<char*>(&i.Ambient), sizeof(aiColor4D));

		size_t texSize = i.textures.size(); // Save all textures of this material
		if (texSize > 0)
		{
			serializeFile.write(reinterpret_cast<char*>(&texSize), sizeof(texSize));
			serializeFile.write(reinterpret_cast<char*>(&i.textures[0]), texSize * sizeof(Texture));
		}
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

std::string GenerateGUID(const std::string meshFileName) // Using meshFileName to convert to ascii value
{
	std::stringstream stream{};
	for (size_t i = 0; i < meshFileName.length(); ++i)
	{
		int asc = static_cast<int>(meshFileName[i]); // Convert from char to int first
		stream << std::hex << asc; // Convert to hexadecimal
	}

	std::uniform_real_distribution<double> distribution(0, 1000);
	std::random_device rd;
	std::default_random_engine generator(rd());
	int number = static_cast<int>(distribution(generator));
	stream << std::hex << number;

	std::string GUID(stream.str()); // Concat the string of hex asc
	return GUID;
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

	writer.String("Mesh GUID");
	writer.String(GenerateGUID(meshFileName).c_str());

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
					std::cout << " Done!" << std::endl;
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