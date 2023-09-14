#include "Precompiled.h"
#include "GraphicsSystem.h"

#include "LapSupGraphics/Compiler.h"
#include "LapSupGraphics/Mesh.h"
#include "Model3d.h"
#include "Editor_Camera.h"
#include "../Core/FramerateController.h"

#include "Editor/Editor.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"


//Temporary
Model testmodel;

Model LightSource;

Model AffectedByLight;

unsigned int testBoxbuffer;
Model testBox;

unsigned int entitySRTBuffer;
glm::mat4 entitySRT[EntityRenderLimit];
Model Line;

InstanceProperties properties[EntityRenderLimit];

bool SwappingColorSpace = false;
//Editor_Camera testCam;

//Editor_Camera E_Camera;
std::vector<Ray3D> Ray_Container;

void InstanceSetup(GLuint vaoid);

void GraphicsSystem::Init()
{
	
	//std::cout << "-- Graphics Init -- " << std::endl;

	//INIT GRAPHICS HERE
	
	glEnable(GL_EXT_texture_sRGB); // Unsure if this is required	

	// Temporary Stuff, Things will be moved accordingly (hopefully)
	//AssimpLoader assimp("Assets/Models/Skull_textured/Skull_textured.geom.desc", "Assets/Models/Skull_textured/Skull_textured.geom");
	//testmodel.init(&assimp);// The Shader is set up inside this init function

	testmodel.init();
	testBox.instance_cubeinit();
	testBox.setup_instanced_shader();

	// Euan RayCasting Testing
	Line.lineinit();

	// Magic Testing
	LightSource.cubeinit();
	LightSource.setup_lightshader();
	
	AffectedByLight.cubeinit();
	AffectedByLight.setup_affectedShader();

	// Setting up Positions
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	testmodel.position = glm::vec3(0.f, 0.f, -800.f);
	LightSource.position = glm::vec3(0.f, 0.f, -300.f);
	AffectedByLight.position = glm::vec3(0.f, 0.f, -500.f);

	int index = 0;
	for (Entity& entity : currentScene.entities) {
		entitySRT[index] = glm::mat4(1.f);
		++index;
	}

	glGenBuffers(1, &entitySRTBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, entitySRTBuffer);
	glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::mat4), &entitySRT[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	InstanceSetup(testBox.vaoid);

	EditorCam.Init();
}

void GraphicsSystem::Update(float dt)
{
	//std::cout << "-- Graphics Update -- " << std::endl;
	
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	currentScene.singleComponentsArrays.GetArray<Transform>();
	
	// I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
	bool checkForSelection = false;
	Ray3D temp;
	if (InputHandler::isMouse_L_DoubleClick())
	{
		temp = EditorCam.Raycasting(EditorCam.GetMouseInNDC().x, EditorCam.GetMouseInNDC().y, 
			EditorCam.getPerspMatrix(), EditorCam.getViewMatrix(), EditorCam.GetCameraPosition());
		Ray_Container.push_back(temp);
		checkForSelection = true;
	}
	float intersected = FLT_MAX;
	float temp_intersect;

	int i = 0;
	for (Entity& entity : currentScene.entities)
	{
		Transform& trans = currentScene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(entity.denseIndex);
		
		//glm::mat4 scaling_mat(
		//	glm::vec4(trans.scale.x, 0.f, 0.f, 0.f),
		//	glm::vec4(0.f, trans.scale.y, 0.f, 0.f),
		//	glm::vec4(0.f, 0.f, trans.scale.z, 0.f),
		//	glm::vec4(0.f, 0.f, 0.f, 1.f)

		//);
		//glm::mat4 rotation_mat(
		//	glm::vec4(cos(90.f), 0.f, -sin(90.f), 0.f),
		//	glm::vec4(0.f, 1.f, 0.f, 0.f),
		//	glm::vec4(sin(90.f), 0.f, cos(90.f), 0.f),
		//	glm::vec4(0.f, 0.f, 0.f, 1.f)
		//);
		//glm::mat4 translation_mat(
		//	glm::vec4(1.f, 0.f, 0.f, 0.f),
		//	glm::vec4(0.f, 1.f, 0.f, 0.f),
		//	glm::vec4(0.f, 0.f, 1.f, 0.f),
		//	glm::vec4(trans.translation, 1.f)
		//);
		//glm::mat4 SRT = translation_mat * rotation_mat * scaling_mat;
		//entitySRT[i] = SRT;
		// 
		entitySRT[i] = trans.GetWorldMatrix();
		//entitySRT[i] = glm::mat4(1.f);
		++i;
		
		// I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
		if (checkForSelection)
		{
			glm::mat4 translation_mat(
					glm::vec4(1.f, 0.f, 0.f, 0.f),
					glm::vec4(0.f, 1.f, 0.f, 0.f),
					glm::vec4(0.f, 0.f, 1.f, 0.f),
					glm::vec4(trans.translation, 1.f)
				);
			glm::mat4 rotation_mat = glm::toMat4(glm::quat(trans.rotation));

			glm::vec3 mins = trans.scale * glm::vec3(-1.f, -1.f, -1.f);	
			glm::vec3 maxs = trans.scale * glm::vec3(1.f, 1.f, 1.f);

			glm::mat4 noscale = translation_mat * rotation_mat;

			if (testRayOBB(temp.origin, temp.direction, mins, maxs,
				noscale, temp_intersect))
			{
				if (temp_intersect < intersected)
				{
					SelectedEntityEvent selectedEvent{ &entity };
					EVENTS.Publish(&selectedEvent);
					intersected = temp_intersect;
				}
			}
		}
	}

	// I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
	if (intersected == FLT_MAX && checkForSelection) 
	{// This means that u double clicked, wanted to select something, but THERE ISNT ANYTHING
		SelectedEntityEvent selectedEvent{ nullptr };
		EVENTS.Publish(&selectedEvent);
	}



	//Currently Putting in Camera Update loop here


	// Bean: For binding framebuffer
	EditorCam.getFramebuffer().bind();

	EditorCam.Update((float)MyFrameRateController.getDt());

	// This one is turbo scuffed i just putting here to test  light
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_UP))
	{
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_ALT))
		{
			LightSource.position.x -= 10.f;
		}
		else
		{
			LightSource.position.y += 10.f;

		}
	}
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT))
	{
		LightSource.position.z -= 10.f;
	}
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_DOWN))
	{
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_ALT))
		{
			LightSource.position.x += 10.f;
		}
		else
		{
			LightSource.position.y -= 10.f;

		}
	}

	if (InputHandler::isKeyButtonHolding(GLFW_KEY_RIGHT))
	{
		LightSource.position.z += 10.f;
	}


	if (InputHandler::isKeyButtonPressed(GLFW_KEY_G))
	{
		SwappingColorSpace = !SwappingColorSpace;
		if (SwappingColorSpace)
		{
			glEnable(GL_FRAMEBUFFER_SRGB);
}
		else
		{
			glDisable(GL_FRAMEBUFFER_SRGB);

		}
	}
	// instanced bind
	glBindBuffer(GL_ARRAY_BUFFER, entitySRTBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &entitySRT[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Draw();

	// Bean: For unbinding framebuffer
	EditorCam.getFramebuffer().unbind();
	//glDisable(GL_FRAMEBUFFER_SRGB);
}

void GraphicsSystem::Draw() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.5f, 0.5f, 1.f);
	glEnable(GL_DEPTH_BUFFER);

	testmodel.draw();
	// for  model : models{
	//	for tex : model.tex_vaoid{
	//		bind texture into uniform sampler2d
	//	}
	//	draw instance
	// }
	testBox.instanceDraw(EntityRenderLimit);

	/*LightSource.lightSource_draw();
	AffectedByLight.affectedByLight_draw(LightSource.position);*/

	
	// This is to render the Rays -> Uncomment if u wanna see

	if (Ray_Container.size() > 0)
	{
		
		for (int i = 0; i < Ray_Container.size(); ++i)
		{
			Ray3D ray = Ray_Container[i];
			
			//std::cout << "ray " << ray.origin.x << "\n";
			//std::cout << "ray direc" << ray.direction.x << "\n";

			glm::mat4 SRT
			{
				glm::vec4(ray.direction.x * 1000000.f, 0.f , 0.f , 0.f),
				glm::vec4(0.f, ray.direction.y * 1000000.f, 0.f , 0.f),
				glm::vec4(0.f , 0.f , ray.direction.z * 1000000.f , 0.f),
				glm::vec4(ray.origin.x, ray.origin.y, ray.origin.z,1.f)
			};
			//std::cout << "in here draw\n";
			Line.debugline_draw(SRT);

		}
	}


}

void InstanceSetup(GLuint vaoid) {
	//entitySRTBuffer
	glBindVertexArray(vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, entitySRTBuffer);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
	glBindVertexArray(0);
}

void GraphicsSystem::Exit()
{
	//std::cout << "-- Graphics Exit -- " << std::endl;

	//CLEANUP GRAPHICS HERE
}