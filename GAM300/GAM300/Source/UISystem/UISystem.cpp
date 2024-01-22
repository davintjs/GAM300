#include "Precompiled.h"
#include "UISystem.h"
#include "Scene/Components.h" 
#include "Scene/SceneManager.h"




//Button* Button_init(const char* id, int x, int y, int width, int height /*void (*on_click)(void)*/)
//{
//	Button* button = (Button*)malloc(sizeof(Button));
//	button->id = (char*)malloc(strlen(id) + 1);
//	strcpy(button->id, id);
//	button->x = x;
//	button->y = y;
//	button->width = width;
//	button->height = height;
//	button->is_clicked = false;
//	//button->on_click = on_click;
//	//return button;
//}


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


void UISystem::HandleButtonInput(Button& button, float mouseX, float mouseY, bool mousePressed) {
	// Check if the mouse coordinates are within the button boundaries.
	if (mouseX >= button.x && mouseX <= button.x + button.width &&
		mouseY >= button.y && mouseY <= button.y + button.height) {
		// Mouse is within the button boundaries.

		// If the mouse is pressed, execute the button's onClick callback.
		if (mousePressed) {
			button.on_click_callback();

		}
	}
}

void UISystem::Update(float dt)
{
	UISystem UI; 
	Button button;
	UI.HandleButtonInput(button, 10, 10, true);
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
