/*!***************************************************************************************
\file			AnimationManager.h
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           10/10/2023

\brief
	This file contains the Animation Manager and the declarations of its related functions.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "AssetManager/AssetManager.h"

#include "Editor_Camera.h"

void AnimationManager::Init()
{
    std::cout << "ANIMATION MANAGER INIT\n";




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

	// load models
	// -----------
	//AnimationModel ourModel("Assets/Models/AnimatedSkeleton/AnimatedSkeleton.fbx", false);
	//Animation ourAnimation("Assets/Models/AnimatedSkeleton/AnimatedSkeleton.fbx", &ourModel);
	//AnimationAnimator animator(&ourAnimation);


	allModels.init("Assets/Models/Doctor_Attacking/Doctor_Attacking.fbx", false);
	allAnimations.init("Assets/Models/Doctor_Attacking/Doctor_Attacking.fbx", &allModels);
	allAnimators.init(&allAnimations);

}


void AnimationManager::Update(float dt)
{
    //UNREFERENCED_PARAMETER(dt);

	// draw in wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// render loop
		// -----------
			// per-frame time logic
			// --------------------
		//float currentFrame = glfwGetTime();
		//deltaTime = currentFrame - lastFrame;
		//lastFrame = currentFrame;

		// input
		// -----
		//processInput(window);
		allAnimators.UpdateAnimation(dt);


}

void AnimationManager::Draw()
{

	// render
	// ------
	//glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// don't forget to enable shader before setting uniforms
	ourShader.Use();

	// view/projection transformations
	GLint uniform1 =
		glGetUniformLocation(ourShader.GetHandle(), "projection");
	GLint uniform2 =
		glGetUniformLocation(ourShader.GetHandle(), "view");
	glUniformMatrix4fv(uniform1, 1, GL_FALSE,
		glm::value_ptr(EditorCam.getPerspMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.getViewMatrix()));

	auto transforms = allAnimators.GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
	{
		std::string temp = "finalBonesMatrices[" + std::to_string(i) + "]";
		GLint uniform3 =
			glGetUniformLocation(ourShader.GetHandle(), temp.c_str());

		glUniformMatrix4fv(uniform3, 1, GL_FALSE,
			glm::value_ptr(transforms[i]));
	}


	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(.5f, .5f, .5f));	// it's a bit too big for our scene, so scale it down
	glUniformMatrix4fv(glGetUniformLocation(ourShader.GetHandle(), "model"), 1, GL_FALSE,
		glm::value_ptr(model));
	allModels.Draw(ourShader);
}
void AnimationManager::Exit()
{
}
