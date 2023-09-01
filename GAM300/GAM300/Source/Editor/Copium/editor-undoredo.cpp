/*!***************************************************************************************
\file			editor-undoredo.cpp
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

#include "pch.h"
#include "Editor/editor-undoredo.h"
#include "GameObject/game-object-factory.h"
#include "SceneManager/scene-manager.h"

namespace Copium
{
	UndoRedo::TransformCommand::TransformCommand(float* _pointer, float _value)
	{
		this->pointer = _pointer;
		this->value =  _value;
		std::cout << "New Undo created with value: " << this->value<<"\n";
	}

	UndoRedo::TransformCommand::~TransformCommand()
	{
		
	}

	void UndoRedo::TransformCommand::Undo(std::stack<Command*>* stackPointer)
	{
		if (!stackPointer)
		{
			PRINT("Invalid stack pointer");
			return;
		}
		Command* temp = new TransformCommand(this->pointer, *this->pointer);
		stackPointer->push(temp);
		if (this->pointer != nullptr)
		{
			*this->pointer = this->value;
		}
		else
		{
			std::cout << "The pointer is invalid, skipping undo command";
		}
	}

	void UndoRedo::TransformCommand::Redo(std::stack<Command*>* stackPointer)
	{
		if (!stackPointer)
		{
			PRINT("Invalid stack pointer");
			return;
		}
		Command* temp = new TransformCommand(this->pointer, *this->pointer);
		stackPointer->push(temp);

		if (this->pointer)
		{
			*this->pointer = this->value;
		}
		else
		{
			std::cout << "The pointer is invalid, skipping redo command";
		}
	}

	void UndoRedo::TransformCommand::printCommand()
	{
		if (this->pointer)
		{
			std::cout << "The command's pointer is valid";
		}
		else
		{
			std::cout << "The command's pointer is valid";
		}
		std::cout << " & the value to restore is "<<this->value;
	}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//UndoRedo::GameObjectCommand::GameObjectCommand(std:* _value, bool _deleting) : sptr{new GameObject*(_value), [](GameObject** _go)
	//{
	//		std::cout << "gameobj cmd delete called\n";
	//		if (*_go)
	//		{
	//			std::cout << "go is still alive\n";
	//			delete *_go; 
	//			(*_go) = nullptr;
	//		}
	//		delete _go;
	//			
	//} }
	//{
	//	std::cout << "gameobj cmd\n";
	//	std::cout << "reference count: " << sptr.use_count() << std::endl;


	//	if (_deleting)
	//	{
	//		std::cout << "marked for deletion\n";
	//		value = _value;
	//		pointer = _value;
	//	}
	//	else
	//	{
	//		std::cout << "transfer ownership\n";
	//		//this->value = MyGOF.clone(*_value,nullptr);
	//		value = _value;
	//		pointer = _value;

	//	}

	//	deleting = _deleting;
	//	std::cout << (*sptr) << std::endl;

	//}
	UndoRedo::GameObjectCommand::GameObjectCommand(std::shared_ptr<GameObject>& p, bool _deleting) : sptr{p}
	{
		std::cout << "gameobjcmd sharedptr ctor\n";
		std::cout << "reference count: " << sptr.use_count() << std::endl;


		if (_deleting)
		{
			std::cout << "marked for deletion\n";

		}
		else
		{
			std::cout << "transfer ownership\n";


		}

		value =  &(*sptr);
		pointer = &(*sptr);
		deleting = _deleting;

	}


	UndoRedo::GameObjectCommand::~GameObjectCommand()
	{
		std::cout << "game obj command destructed\n";
		std::cout << "reference count: " << sptr.use_count() << std::endl;
		//std::cout << "Undo Redo being destroyed: " << this->value->name << " | " << this->value << "\n\n";
		//MyGOF.destroy(this->value);
		//delete this->value;

	}

	void UndoRedo::GameObjectCommand::Undo(std::stack<Command*>* stackPointer)
	{
		if (!stackPointer)
		{
			PRINT("Invalid stack pointer");
			return;
		}

		bool isDeleting = this->deleting;

		if (isDeleting)
		{
			if (!value->name.empty())
			{
				std::cout << "Deleting game object" << std::endl;
				//MyGOF.destroy(this->pointer);
				Command* temp = new GameObjectCommand(sptr, false);
				stackPointer->push(temp);
			}
			else
			{
				std::cout << "The pointer is invalid, skipping undo command";
			}
		}
		else
		{
			if (!value->name.empty())
			{
				std::cout << "Create" << std::endl;
				//GameObject* newObj = MyGOF.instantiate(*this->value);
				//Copium::SceneManager::Instance()->get_current_scene()->gameObjects.push_back(value);
				Copium::SceneManager::Instance()->get_current_scene()->gameObjectSPTRS.push_back(sptr);				
				Command* temp = new GameObjectCommand(sptr, true);
				stackPointer->push(temp);
			}
			else
			{
				std::cout << "The pointer is invalid, skipping undo command";
			}
		}
	}
	void UndoRedo::GameObjectCommand::Redo(std::stack<Command*>* stackPointer)
	{
		if (!stackPointer)
		{
			PRINT("Invalid stack pointer");
			return;
		}
		std::cout << "redo\n";
		bool isDeleting = this->deleting;

		if (isDeleting)
		{

			if (!value->name.empty())
			{
				std::cout << "Delete" << std::endl;
				//MyGOF.destroy(this->pointer);
				Command* temp = new GameObjectCommand(sptr, false);
				stackPointer->push(temp);
			}
			else
			{
				std::cout << "The pointer is invalid, skipping undo command";
			}
		}
		else
		{
			if (!value->name.empty())
			{
				std::cout << "Create" << std::endl;
				//GameObject* newObj = MyGOF.instantiate(*this->value);
				//Copium::SceneManager::Instance()->get_current_scene()->gameObjects.push_back(value);
				Copium::SceneManager::Instance()->get_current_scene()->gameObjectSPTRS.push_back(sptr);


				Command* temp = new GameObjectCommand(sptr, true);
				stackPointer->push(temp);
			}
			else
			{
				std::cout << "The pointer is invalid, skipping undo command";
			}
		}
	}
	void UndoRedo::GameObjectCommand::printCommand()
	{
		std::cout << "//Printing Command Info//\n";
		if (!value->name.empty())
		{
			std::cout << value->name << std::endl;
		}
		std::cout << "//End Command Info//\n";
	}

	bool UndoRedo::GameObjectCommand::getDeleting()
	{
		return deleting;
	}
}
