#include "Scene/Components.h"  

#ifndef UISYSTEM_H
#define UISYSTEM_H



ENGINE_RUNTIME_SYSTEM(UISystem) {


	//void HandleButtonInput(Button& button, float mouseX, float mouseY, bool mousePressed) {
	//	// Check if the mouse coordinates are within the button boundaries.
	//	if (mouseX >= button.x && mouseX <= button.x + button.width &&
	//		mouseY >= button.y && mouseY <= button.y + button.height) {
	//		// Mouse is within the button boundaries.
	//
	//		// If the mouse is pressed, execute the button's onClick callback.
	//		if (mousePressed) {
	//			button.on_click_callback();
	//
	//		}
	//	}
	//}




public:
	void Init();
	void Update(float dt);
	void Exit();
	//Button button;

	//void drawButton(Button &button);
	//void handleButtonclick(Button &button);
	void HandleButtonInput(Button & button, float mouseX, float mouseY, bool mousePressed);

	//Button* Button_init(const char* id, int x, int y, int width, int height);

};


#endif
