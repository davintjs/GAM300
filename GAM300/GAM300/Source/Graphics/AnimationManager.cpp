/*!***************************************************************************************
\file			AnimationManager.cpp
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           10/10/2023

\brief
    This file contains the Animation Manager and the declarations of its related functions.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/


#include "Precompiled.h"
#include "AnimationManager.h"

//#include "GraphicsHeaders.h"
//#include "glslshader.h"
//#include <glm/gtx/quaternion.hpp>
////#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include "AssetManager/AssetManager.h"
#include "Scene/SceneManager.h"




// constructor
AnimationMesh::AnimationMesh(std::vector<AnimationVertex> vertices, std::vector<unsigned int> indices, std::vector<TextureInfo> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

// render the mesh
void AnimationMesh::Draw(GLSLShader& shader)
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.GetHandle(), (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void AnimationMesh::setupMesh()
{
    if (glewInit() != GLEW_OK) {
        std::cout << "omg help";
    }
    GLuint VAO, VBO, EBO;
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AnimationVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimationVertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AnimationVertex), (void*)offsetof(AnimationVertex, Normal));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AnimationVertex), (void*)offsetof(AnimationVertex, Tangent));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AnimationVertex), (void*)offsetof(AnimationVertex, TexCoords));
    //// vertex bitangent
    //glEnableVertexAttribArray(4);
    //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(AnimationVertex), (void*)offsetof(AnimationVertex, Bitangent));
    // vertex color
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(AnimationVertex), (void*)offsetof(AnimationVertex, Color));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(AnimationVertex), (void*)offsetof(AnimationVertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(AnimationVertex), (void*)offsetof(AnimationVertex, m_Weights));
    glBindVertexArray(0);
    _VAO = VAO;
}



Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
    :
    m_Name(name),
    m_ID(ID),
    m_LocalTransform(1.0f)
{
    m_NumPositions = channel->mNumPositionKeys;

    for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
    {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyPosition data;
        data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
        data.timeStamp = timeStamp;
        m_Positions.push_back(data);
    }

    m_NumRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
    {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data;
        data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
        data.timeStamp = timeStamp;
        m_Rotations.push_back(data);
    }

    m_NumScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
    {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        KeyScale data;
        data.scale = AssimpGLMHelpers::GetGLMVec(scale);
        data.timeStamp = timeStamp;
        m_Scales.push_back(data);
    }
}

void Bone::Update(float animationTime)
{
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    m_LocalTransform = translation * rotation * scale;
}


int Bone::GetPositionIndex(float animationTime)
{
    for (int index = 0; index < m_NumPositions - 1; ++index)
    {
        if (animationTime < m_Positions[index + 1].timeStamp)
            return index;
    }
    assert(0);
}

int Bone::GetRotationIndex(float animationTime)
{
    for (int index = 0; index < m_NumRotations - 1; ++index)
    {
        if (animationTime < m_Rotations[index + 1].timeStamp)
            return index;
    }
    assert(0);
}

int Bone::GetScaleIndex(float animationTime)
{
    for (int index = 0; index < m_NumScalings - 1; ++index)
    {
        if (animationTime < m_Scales[index + 1].timeStamp)
            return index;
    }
    assert(0);
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
    if (1 == m_NumPositions)
        return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
        m_Positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position
        , scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
    if (1 == m_NumRotations)
    {
        auto rotation = glm::normalize(m_Rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
        m_Rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation
        , scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);

}

glm::mat4 Bone::InterpolateScaling(float animationTime)
{
    if (1 == m_NumScalings)
        return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
        m_Scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
        , scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}

Bone* Animation::FindBone(const std::string& name)
{
    auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
        [&](const Bone& Bone)
        {
            return Bone.GetBoneName() == name;
        }
    );
    if (iter == m_Bones.end()) return nullptr;
    else return &(*iter);
}


void Animation::ReadMissingBones(const aiAnimation* animation, AnimationModel& model)
{
    int size = animation->mNumChannels;

    auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
    int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

    //reading channels(bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        m_Bones.push_back(Bone(channel->mNodeName.data,
            boneInfoMap[channel->mNodeName.data].id, channel));
    }

    m_BoneInfoMap = boneInfoMap;
}

void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (int i = 0; i < src->mNumChildren; i++)
    {
        AssimpNodeData newData;
        ReadHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}
//
//AnimationAnimator::AnimationAnimator()
//{
//}
//
//void AnimationAnimator::init(Animation* animation)
//{
//    m_CurrentTime = 0.0;
//    m_CurrentAnimation = animation;
//
//    m_FinalBoneMatrices.reserve(100);
//
//    for (int i = 0; i < 100; i++)
//        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
//}
//
//void AnimationAnimator::UpdateAnimation(float dt)
//{
//    m_DeltaTime = dt;
//    if (m_CurrentAnimation)
//    {
//        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
//        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
//        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
//    }
//}
//
//void AnimationAnimator::PlayAnimation(Animation* pAnimation)
//{
//    m_CurrentAnimation = pAnimation;
//    m_CurrentTime = 0.0f;
//}
//
//void AnimationAnimator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
//{
//    std::string nodeName = node->name;
//    glm::mat4 nodeTransform = node->transformation;
//
//    Bone* Bone = m_CurrentAnimation->FindBone(nodeName);
//
//    if (Bone)
//    {
//        Bone->Update(m_CurrentTime);
//        nodeTransform = Bone->GetLocalTransform();
//    }
//
//    glm::mat4 globalTransformation = parentTransform * nodeTransform;
//
//    auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
//    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
//    {
//        int index = boneInfoMap[nodeName].id;
//        glm::mat4 offset = boneInfoMap[nodeName].offset;
//        m_FinalBoneMatrices[index] = globalTransformation * offset;
//    }
//
//    for (int i = 0; i < node->childrenCount; i++)
//        CalculateBoneTransform(&node->children[i], globalTransformation);
//}
//
//std::vector<glm::mat4> AnimationAnimator::GetFinalBoneMatrices()
//{
//    return m_FinalBoneMatrices;
//}

// constructor, expects a filepath to a 3D model.
AnimationModel::AnimationModel()
{
}

void AnimationModel::init(std::string const& path, bool gamma = false)
{
    //gammaCorrection = gamma;
    loadModel(path);
    m_BoneCounter = 0; // Initialize boneCounter to 0
}

// draws the model, and thus all its meshes
void AnimationModel::Draw(GLSLShader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}


// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void AnimationModel::loadModel(std::string const& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals
        | aiProcess_CalcTangentSpace | aiProcess_FlipWindingOrder
        | aiProcess_TransformUVCoords | aiProcess_FlipUVs);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    //directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);

    if (scene->HasAnimations())
    {
        // Animation init
        //Assimp::Importer importer;
        /*const aiScene* */scene = importer.ReadFile(path, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0]; // this might need to change quite a bit since an fbx may hv > 1 anim
        allAnimations.GetDuration() = animation->mDuration;
        allAnimations.GetTicksPerSecond() = animation->mTicksPerSecond;
        aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
        globalTransformation = globalTransformation.Inverse();
        allAnimations.ReadHierarchyData(allAnimations.GetRootNode(), scene->mRootNode);
        allAnimations.ReadMissingBones(animation, *this);
    }
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void AnimationModel::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}


AnimationMesh AnimationModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<AnimationVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureInfo> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        AnimationVertex vertex;

        //SetVertexBoneDataToDefault(vertex);
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }

        vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
        vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<TextureInfo> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //std::vector<Animation> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    //std::vector<AnimationTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    //textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    //std::vector<AnimationTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    ExtractBoneWeightForVertices(vertices, mesh, scene);

    return AnimationMesh(vertices, indices, textures);
}

void AnimationModel::ExtractBoneWeightForVertices(std::vector<AnimationVertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    auto& boneInfoMap = m_BoneInfoMap;
    int& boneCount = m_BoneCounter;

    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            boneCount++;
        }
        else
        {
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());

            //SetVertexBoneData(vertices[vertexId], boneID, weight); 
            // idk if i am doing this right i went to add my own thing
            if (weight == 0.0f) // skip if bone weight 0
            {
                return;
            }

            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
            {
                if (vertices[vertexId].m_BoneIDs[i] == boneID) { // skip if bone existd alr
                    return;
                }

                if (vertices[vertexId].m_BoneIDs[i] < 0)
                {
                    vertices[vertexId].m_Weights[i] = weight;
                    vertices[vertexId].m_BoneIDs[i] = boneID;
                    break;
                }
            }
        }
    }
}

std::vector<TextureInfo> AnimationModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<TextureInfo> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            TextureInfo texture;

            std::string filename = str.C_Str();
            std::string nameWithoutExtension;
            size_t lastDotPos = filename.find_last_of('.');
            if (lastDotPos != std::string::npos) { // Extract the part of the string before the last dot
                nameWithoutExtension = filename.substr(0, lastDotPos);
            }
            texture.id = TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID(nameWithoutExtension));
            //texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}




void Animation_Manager::Init()
{
    std::cout << "ANIMATION MANAGER INIT\n";


    // move this out....

    std::vector<std::pair<GLenum, std::string>> shdr_files;
    // Vertex Shader
    shdr_files.emplace_back(std::make_pair(
        GL_VERTEX_SHADER,
        "GAM300/Shaders/BasicAnimation.vert"));

    // Fragment Shader
    shdr_files.emplace_back(std::make_pair(
        GL_FRAGMENT_SHADER,
        "GAM300/Shaders/BasicAnimation.frag"));

    PRINT("animshader", '\n');
    ourShader.CompileLinkValidate(shdr_files);
    PRINT("animshader", "\n\n");

    // if linking failed
    if (GL_FALSE == ourShader.IsLinked())
    {
        std::stringstream sstr;
        sstr << "Unable to compile/link/validate shader programs\n";
        sstr << ourShader.GetLog() << "\n";
        PRINT(sstr.str());
        std::exit(EXIT_FAILURE);
    }


    // we want compiler to serialise model info including the animations
    allModels_.init("Assets/Models/Doctor_Attacking/Doctor_Attacking.fbx", false);
    // called to animate animaation, by right i think it should be a component thing 
    //then in mb render use this fn & init we init whatever is attached to model
    //allAnimators_.init(&allModels_.GetAnimations());

    Scene& currentScene = MySceneManager.GetCurrentScene();
    for (Animator& animator : currentScene.GetArray<Animator>()) // temp,  move to subsys later
    {
        animator.SetAnimation(&allModels_.GetAnimations());
    }
}


void Animation_Manager::Update(float dt)
{
    //UNREFERENCED_PARAMETER(dt);
    //allAnimators_.UpdateAnimation(dt);

    Scene& currentScene = MySceneManager.GetCurrentScene();
    for (Animator& animator : currentScene.GetArray<Animator>()) // temp,  move to subsys later
    {
        if (animator.AnimationAttached())
            animator.UpdateAnimation(dt);
        else
            animator.SetAnimation(&allModels_.GetAnimations());

    }
}

void Animation_Manager::Draw(BaseCamera& _camera)
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    // FOR ALL W ANIMATIONS, PROB TEMP 

    ourShader.Use();
    GLint uniform1 =
        glGetUniformLocation(ourShader.GetHandle(), "projection");
    GLint uniform2 =
        glGetUniformLocation(ourShader.GetHandle(), "view");
    glUniformMatrix4fv(uniform1, 1, GL_FALSE,
        glm::value_ptr(_camera.GetProjMatrix()));
    glUniformMatrix4fv(uniform2, 1, GL_FALSE,
        glm::value_ptr(_camera.GetViewMatrix()));

    //auto transforms = allAnimators_.GetFinalBoneMatrices();
    //for (int i = 0; i < transforms.size(); ++i)
    //{
    //	std::string temp = "finalBonesMatrices[" + std::to_string(i) + "]";
    //	GLint uniform3 =
    //		glGetUniformLocation(ourShader.GetHandle(), temp.c_str());

    //	glUniformMatrix4fv(uniform3, 1, GL_FALSE,
    //		glm::value_ptr(transforms[i]));
    //}

    // this is fking wrong
    Scene& currentScene = MySceneManager.GetCurrentScene();
    for (Animator& animator : currentScene.GetArray<Animator>()) // temp,  move to subsys later
    {
        if (animator.AnimationAttached())
        {

            auto transforms = animator.GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i)
            {
                std::string temp = "finalBonesMatrices[" + std::to_string(i) + "]";
                GLint uniform3 =
                    glGetUniformLocation(ourShader.GetHandle(), temp.c_str());

                glUniformMatrix4fv(uniform3, 1, GL_FALSE,
                    glm::value_ptr(transforms[i]));
            }
        }
    }


    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(.01f, .01f, .01f));	// it's a bit too big for our scene, so scale it down
    glUniformMatrix4fv(glGetUniformLocation(ourShader.GetHandle(), "model"), 1, GL_FALSE,
        glm::value_ptr(model));


    allModels_.Draw(ourShader);
}

void Animation_Manager::Exit()
{
}
