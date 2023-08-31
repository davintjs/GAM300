/*!***************************************************************************************
\file			editor-undoredo.h
\project
\author			Shawn Tanary

\par			Course: GAM250
\par			Section:
\date			25/11/2022

\brief
	This file holds functions to undo and redo changes made in gameobjects as well as 
	transform components.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#pragma once

#include <stack>
#include <memory>
#include "GameObject/game-object.h"

namespace Copium
{
	namespace UndoRedo
	{
		//The base class for commands
		class Command
		{
			public:
				virtual ~Command() = default;
				virtual void Undo(std::stack<Command*>* stackPointer) = 0;
				virtual void Redo(std::stack<Command*>* stackPointer) = 0;
				virtual void printCommand() = 0;
				virtual bool getDeleting() = 0;

		};

		//a derived subclass specifically for transform components
		class TransformCommand final : public Command
		{
			public:
				/***************************************************************************/
				/*!
				\brief
					Constructor for a transform command
				*/
				/**************************************************************************/
				TransformCommand(float* _pointer, float _value);

				/***************************************************************************/
				/*!
				\brief
					Destructor for a transform command
				*/
				/**************************************************************************/
				~TransformCommand();

				/***************************************************************************/
				/*!
				\brief
					Push its current value to redo and revert the pointer to its previous value
				\param
					a pointer to the opposite stack pointer in case of reverting this func
				*/
				/**************************************************************************/
				void Undo(std::stack<Command*>* stackPointer) override;

				/***************************************************************************/
				/*!
				\brief
					Push its current value to undo and revert the pointer to its previous value
				\param
					a pointer to the opposite stack pointer in case of reverting this func
				*/
				/**************************************************************************/
				void Redo(std::stack<Command*>* stackPointer) override;

				/***************************************************************************/
				/*!
				\brief
					Prints information about the command
				*/
				/**************************************************************************/
				void printCommand()override;

				bool getDeleting()override { return false; }

				float* pointer;//pointer to what was changed
				float value;//original value
		};

		//a derived subclass specifically for game objects
		class GameObjectCommand final : public Command
		{
			public:
				/***************************************************************************/
				/*!
				\brief
					Constructor for a game object command
				*/
				/**************************************************************************/
				//GameObjectCommand(GameObject& _value);

				//GameObjectCommand(GameObject* _value,bool _deleting);
				GameObjectCommand(std::shared_ptr<GameObject>& p, bool _deleting);

				/***************************************************************************/
				/*!
				\brief
					Destructor for a game object command
				*/
				/**************************************************************************/
				~GameObjectCommand();

				/***************************************************************************/
				/*!
				\brief
					Push its current value to redo and revert the pointer to its previous value
				\param
					a pointer to the opposite stack pointer in case of reverting this func
				*/
				/**************************************************************************/
				void Undo(std::stack<Command*>* stackPointer) override;

				/***************************************************************************/
				/*!
				\brief
					Push its current value to undo and revert the pointer to its previous value
				\param
					a pointer to the opposite stack pointer in case of reverting this func
				*/
				/**************************************************************************/
				void Redo(std::stack<Command*>* stackPointer) override;

				/***************************************************************************/
				/*!
				\brief
					Prints information about the command
				*/
				/**************************************************************************/
				void printCommand() override;

				bool getDeleting() override;

				//GameObject* pointer;//pointer to what was changed
				GameObject* pointer;//only use if deleting
				GameObject* value;//original value
				bool deleting;// should it add or delete a gameobject


				// M4
				std::shared_ptr<GameObject> sptr;
		};

		//contains a stack of commands for undo and redo
		class CommandManager
		{
			public:
				std::stack<Command*> undoStack;
				std::stack<Command*> redoStack;
		};
	}
	
}
