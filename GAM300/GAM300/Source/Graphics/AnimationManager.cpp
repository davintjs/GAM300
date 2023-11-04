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
#include "AssetManager/ModelCompiler.h"

#include "Scene/SceneManager.h"

AnimationMesh::AnimationMesh(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices, std::vector<TextureInfo> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    /*static int id = 0;
    std::string name = "Vertices_";
    name += std::to_string(id++) + ".txt";
    std::ofstream out(name.c_str());
    
    for (size_t i = 0; i < vertices.size(); i++)
    {
        out << "Vertex " << i << " : "<< vertices[i].position.x << " "<< vertices[i].position.y << " " << vertices[i].position.z << '\n';
    }

    out.close();

    static int id2 = 0;
    name = "VerticesBones_" + std::to_string(id2++) + ".txt";
    out.open(name.c_str());

    for (size_t i = 0; i < vertices.size(); i++)
    {
        for (size_t j = 0; j < MAX_BONE_INFLUENCE; j++)
        {
            if (vertices[i].boneIDs[j] > 100)
                out << "Vertex " << " : " << vertices[i].boneIDs[j] << "\n";
        }
        
    }

    out.close();*/

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
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

    m_BoneInfoMap_ = boneInfoMap;
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

void Animation_Manager::Init()
{
    std::cout << "ANIMATION MANAGER INIT\n";
	// we want compiler to serialise model info including the animations
    // Bean: This should NOT be called, the model animations will be retrieved from AssetManager in the future
    GeomComponents md = MODELCOMPILER.LoadModel("Assets/Models/Player/PlayerV2_Running.fbx", false);
    //GeomComponents md = MODELCOMPILER.LoadModel("Assets/Models/Player/Walking.fbx", false);
    allModels_ = md.animations;
   
    mAnimationContainer.emplace("docattc", allModels_.GetAnimations());
}


void Animation_Manager::Update(float dt)
{
    Scene& currentScene = MySceneManager.GetCurrentScene();
    for (Animator& animator : currentScene.GetArray<Animator>()) // temp,  move to subsys later
    {
        if (animator.playing && animator.AnimationAttached())
        {
            animator.UpdateAnimation(dt);
        }
        else if (!animator.AnimationAttached())
        {
            animator.SetAnimation(&allModels_.GetAnimations());
        }
    }
}

void Animation_Manager::Exit()
{
}


bool Animation_Manager::HasBones(MeshAsset meshAsset) {
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if ((meshAsset.vertices[0].boneIDs[i] != 0) || (meshAsset.vertices[0].weights[i] != 0.0f) )
        {
            return true;  // At least one non-zero element in either array
        }
    }
    return false;  // All elements are zero in both arrays
}