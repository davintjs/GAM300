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



// constructor
AnimationMesh::AnimationMesh(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices, std::vector<TextureInfo> textures)
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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ModelVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, normal));
    // vertex tangent
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, tangent));
    // vertex texture coords
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, textureCords));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(ModelVertex), (void*)offsetof(ModelVertex, boneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, weights));
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

// draws the model, and thus all its meshes
void AnimationModel::Draw(GLSLShader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
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
    GeomComponents md = MODELCOMPILER.LoadModel("Assets/Models/Walking.fbx", false);
        sstr << ourShader.GetLog() << "\n";
        PRINT(sstr.str());
        std::exit(EXIT_FAILURE);
    }


	// we want compiler to serialise model info including the animations
    // Bean: This should NOT be called, the model animations will be retrieved from AssetManager in the future
    ModelComponents md = MODELCOMPILER.LoadModel("Assets/Models/Doctor_Attacking/Doctor_Attacking.fbx", false);
    allModels_ = md.animations;
    
	// called to animate animaation
    //allAnimators_.init(&allModels_.GetAnimations());

    // temp
    //Animation tempanim;
    //tempanim.GetDuration() = allModels_.GetAnimations().GetDuration();
    //tempanim.GetTicksPerSecond() = allModels_.GetAnimations().GetTicksPerSecond();
    //tempanim.GetRootNode() = allModels_.GetAnimations().GetRootNode();

    //std::map<std::string, BoneInfo> tempmap = tempanim.GetBoneIDMap();
    //for (auto& tempmap_src : allModels_.GetAnimations().GetBoneIDMap())
    //{
    //    tempmap.emplace(tempmap_src);
    //}

    //std::vector<Bone> tempbone = tempanim.GetBones();
    //for (auto& tempbone_src : allModels_.GetAnimations().GetBones())
    //{
    //    tempbone.push_back(tempbone_src);
    //}
    //mAnimationContainer.emplace("docattc", tempanim);
    ////   
    mAnimationContainer.emplace("docattc", allModels_.GetAnimations());


    Scene& currentScene = MySceneManager.GetCurrentScene();
    for (Animator& animator : currentScene.GetArray<Animator>()) // temp,  move to subsys later
    {

        Entity& entity = currentScene.Get<Entity>(animator);
        MeshRenderer& meshrenderer = currentScene.Get<MeshRenderer>(entity);
        
        //auto animIt = mAnimationContainer.find("docattc");
        //if (animIt != mAnimationContainer.end())
        //    animator.SetAnimation(&animIt->second); // Pass the 'temp_anim' to SetAnimation
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
        glGetUniformLocation(ourShader.GetHandle(), "persp_projection");
    GLint uniform2 =
        glGetUniformLocation(ourShader.GetHandle(), "View");
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
	model = glm::scale(model, glm::vec3(.01f, .01f, .01f));	// it's a bit too big for our scene, so scale it down
	glUniformMatrix4fv(glGetUniformLocation(ourShader.GetHandle(), "SRT"), 1, GL_FALSE,
        }
    }


	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(.01f, .01f, .01f));	// it's a bit too big for our scene, so scale it down
	glUniformMatrix4fv(glGetUniformLocation(ourShader.GetHandle(), "model"), 1, GL_FALSE,
		glm::value_ptr(model));
	allModels_.Draw(ourShader);

    ourShader.UnUse();
}

void Animation_Manager::Exit()
{
}
