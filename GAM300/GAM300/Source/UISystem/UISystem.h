#include "Scene/Components.h"  

#ifndef UISYSTEM_H
#define UISYSTEM_H



ENGINE_SYSTEM(UISystem) {


public:
	void Init();
	void Update(float dt);
	void Exit();

	//void drawButton(Button &button);
	//void handleButtonclick(Button &button);


	//Button* Button_init(const char* id, int x, int y, int width, int height);

};


#endif
