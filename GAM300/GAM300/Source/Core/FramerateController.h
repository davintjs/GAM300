/*!***************************************************************************************
\file			FramerateController.h
\project
\author			Matthew Lau
\co-authors

\par			Course: GAM200
\par			Section:
\date			05/09/2022

\brief
	Contains declarations for FrameRateController class, its constructor and member functions.

All content © 2022 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#define MyFrameRateController FrameRateController::Instance()

#ifndef FRAMERATECONTROLLER_H
#define FRAMERATECONTROLLER_H
#include "Core/SystemInterface.h"

//Fixed delta time steps
#define MAX_STEP_COUNT 2

class FrameRateController : public Singleton<FrameRateController> {
public:
	void Init(float _fixedDt);
	/*******************************************************************************
	/*!
	*
	\brief
		Start the frame rate controller
		Note: this function should be called at the start of each game loop

	\return
		void
	*/
	/*******************************************************************************/
	void Start();
	/*******************************************************************************
	/*!
	*
	\brief
		End the frame rate controller for the frame.
		Sets the frameEnd time and increments the frame count.
		Note: this function should be called at the end of each game loop

	\return
		void
	*/
	/*******************************************************************************/
	float End();
private:
	double frameStart{ 0 }, frameEnd{ 0 };
	double fixedDeltaTime{ 0 };
	double accumulatedTime{ 0 };
	size_t steps{ 0 };
public:
	double GetFixedDt() const { return fixedDeltaTime; }
	size_t GetSteps() const { return steps; }
};


#endif //FRAMERATECONTROLLER_H
