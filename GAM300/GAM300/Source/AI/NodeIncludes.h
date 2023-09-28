/*!***************************************************************************************
\file			NodeIncludes.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the master header of all behavior tree nodes including

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once
// This should only be included in BehaviorTreeBuilder.cpp !!

// Control Flow Nodes
#include "AI/ControlFlow/Sequencer.h"

// Decorator Nodes
#include "AI/Decorators/Inverter.h"

// Leaf Nodes
#include "AI/LeafNodes/MoveToPosition.h"