/*!***************************************************************************************
****
\file			
\project		
\author			
\co-authors		// DELETE IF NOT APPLICABLE

\par			Course: GAM300
\par			Section: 
\date			//DATE LAST MODIFIED

\brief
	// BRIEF DESCRIPTION OF FILE PURPOSE

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************
****/

#pragma once

#ifndef HANDLE_H

#define HANDLE_H

//INCLUDES
#include "Utilities/UUID.h"

template <typename T>
class Handle
{
public:
	Handle(Engine::UUID = 0, T* = nullptr);
	const Engine::UUID uuid;

	T& Get();

	bool IsValid();

	static Handle<T>& Invalid();
private:
	T* pObject{ nullptr };
};

#include "Handle.cpp"



#endif // !HANDLE_H
