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
		const Engine::UUID id = COLOURPICKER.ColorPickingUIButton(camera);
		for (SpriteRenderer& sr : currentScene.GetArray<SpriteRenderer>())
		{
			sr.onClick = false;
			sr.onHover = false;
		}

		for (SpriteRenderer& sr : currentScene.GetArray<SpriteRenderer>())
		{
			if (sr.EUID() != id)
				continue;
			
			if (!InputHandler::isMouseButtonPressed_L())
			{
				sr.onHover = true;
			}
			else
			{
				sr.onClick = true;
			}
			return;
		}
	}
};

void UISystem::Exit()
{

};
