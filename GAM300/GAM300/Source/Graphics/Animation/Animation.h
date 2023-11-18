/*!***************************************************************************************
\file			Animation.h
\project
\author         Euphrasia Theophelia Tan Ee Mun
\co-author      Sean Ngo

\par			Course: GAM300
\date           09/11/2023

\brief
    This file contains the declarations of the following:
    1. Animation for models
    2. Keyframe animations

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef ANIMATION_H
#define ANIMATION_H

#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssimpGLMHelpers
{
public:

    static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
    {
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

    static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
    {
        return glm::vec3(vec.x, vec.y, vec.z);
    }

    static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
    {
        return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
    }
};


struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

class Bone // these store keyframe data of a bone
{
public:
    Bone() {};
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);

    void Update(float animationTime);
    glm::mat4 GetLocalTransform() { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() { return m_ID; }

    int GetPositionIndex(float animationTime);

    int GetRotationIndex(float animationTime);

    int GetScaleIndex(float animationTime);

    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;

private:

    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

    glm::mat4 InterpolatePosition(float animationTime);

    glm::mat4 InterpolateRotation(float animationTime);

    glm::mat4 InterpolateScaling(float animationTime);
};

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;
};

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

struct AnimationState
{
    std::string label;
    glm::vec2 minMax; //x for min , y for max

    AnimationState() { label = "New state"; minMax = glm::vec2(); }
    AnimationState(const std::string& _label, const glm::vec2& _vec2) : label(_label), minMax(_vec2) {}
};

class Animation
{
public:
    Bone* FindBone(const std::string& name);

    AnimationState* GetAnimationState(const std::string& _state);

    inline float& GetDuration() { return m_Duration; }
    inline int& GetTicksPerSecond() { return m_TicksPerSecond; }
    inline std::vector<AnimationState>& GetAnimationStates() { return m_AnimationStates; }
    inline std::vector<Bone>& GetBones() { return m_Bones; }
    inline AssimpNodeData& GetRootNode() { return m_RootNode; }
    inline std::map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
    inline int& GetBoneCount() { return m_BoneCounter; }

protected:
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<AnimationState> m_AnimationStates;  // Maps the name of the animation with the range using vec2 where x rep start, y rep end
    std::vector<Bone> m_Bones; // Bean: We need to copy this for animator 
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap; // Bean: We need to copy this for animator
    int m_BoneCounter = 0;
};

#endif // !ANIMATION_H