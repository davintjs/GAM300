#include "Precompiled.h"
#include "UISystem.h"
#include "Scene/Components.h" 
#include "Scene/SceneManager.h"
#include "IOManager/InputHandler.h"
#include "IOManager/InputSystem.h"
#include "Graphics/GraphicsHeaders.h"
#include "Graphics/GraphicsSystem.h"



void UISystem::Init()
{
	//UISystem UI;
	//Button myButton(10, 10, 100, 50, "Click me!", []() 
	//    {
	//        // Code to be executed when the button is clicked
	//        std::cout << "Button clicked!" << std::endl;
	//    });

	//UI.addButton(myButton);



};



void UISystem::Update(float dt)
{

	//std::cout << "test" << std::endl
	/*Button button;
	HandleButtonInput(button, 10, 10, true);*/
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();


	// ColorpickingUIbutton - change to return the entity
	for (Camera& camera : currentScene.GetArray<Camera>())
	{
		Engine::UUID id = COLOURPICKER.ColorPickingUIButton(camera);
		for (SpriteRenderer& sr : currentScene.GetArray<SpriteRenderer>())
		{

			sr.onClick = false;
			sr.onHover = false;
			//if (sr.onHover == false)
			//{
			//	sr.orgScale = currentScene.Get<Transform>(sr).GetLocalScale();
			//	sr.hoverTimer = 0.f;

			//}
		}
		for (SpriteRenderer& sr : currentScene.GetArray<SpriteRenderer>())
		{
			if (sr.EUID() == id)
			{
				if (!InputHandler::isMouseButtonPressed_L())
				{
					sr.onHover = true;
					//std::cout << "buttonHover" << std::endl;
					//To scale the size of the text when hovering 
					//Transform& t = currentScene.Get<Transform>(sr);
					//if (sr.hoverDuration > 0 && sr.hoverTimer < sr.hoverDuration)
					//{
					//	vec3 targetScale = glm::mix((glm::vec3)sr.orgScale, (glm::vec3)sr.hoverTarget, sr.hoverTimer / sr.hoverDuration);	//Scale the text '
					//	sr.hoverTimer += dt;
					//	//std::cout << targetScale.x << "," << targetScale.y << "," << targetScale.z << std::endl;
					//	t.SetLocalScale(targetScale);

					//}
					//SpriteRenderer::onHover = true;
				}
				else
				{
					//SpriteRenderer::ColourPicked = true;

					//sr.ColourPicked = true;
					//std::cout << "buttonClicked" << std::endl;
					//std::cout << "Object: " << currentScene.Get<Tag>(sr).name << "\n";
					sr.onClick = true;

					//SceneManager::Instance().LoadScene("Assets/Scene/LevelPlay2.scene");
				}
				return;
			}
			else
			{
				//Transform& t = currentScene.Get<Transform>(sr);
				//if (sr.hoverDuration > 0 && sr.hoverTimer > 0)
				//{
				//	vec3 targetScale = glm::mix((glm::vec3)sr.orgScale, (glm::vec3)sr.hoverTarget, sr.hoverTimer / sr.hoverDuration);	//Scale the text '
				//	sr.hoverTimer -= dt;
				//	//std::cout << targetScale.x << "," << targetScale.y << "," << targetScale.z << std::endl;
				//	t.SetLocalScale(targetScale);

				//}
			}
		}

		//Entity& test = currentScene.Get<Entity>();
	}


	// IF ur not hovering a button

		// RETURN

	// U ARE HOVERING OVER A BUTTON

		// IF mouse NEVER press -> change the texture onButtonHover()

		// IF MOUSE PRESS -> run the button functionlity onButtonClick()



	//std::cout << "test" << std::endl;



};

//void UISystem::drawButton(Button &button)
//{
//    //To draw the button 
//}
//
//void UISystem::handleButtonclick(Button &button) {
//    // Call the button's click handler function
//    button.clickHandler();
//}

void UISystem::Exit()
{

};
