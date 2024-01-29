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
			if (sr.EUID() == id)
			{
				if (!InputHandler::isMouseButtonPressed_L())
				{
					sr.onHover = true;
					return;
					//SpriteRenderer::onHover = true;
				}
				else
				{
					//SpriteRenderer::ColourPicked = true;

					//sr.ColourPicked = true;
					std::cout << "buttonClicked" << std::endl;
					sr.onClick = true;

					//SceneManager::Instance().LoadScene("Assets/Scene/LevelPlay2.scene");
					//StartCoroutine(LoadScene(0.1f));

				}

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
