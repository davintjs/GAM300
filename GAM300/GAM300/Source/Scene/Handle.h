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
	Handle() = delete;
	Handle(Engine::UUID, T&);
	const Engine::UUID uuid;

	T& Get();
	std::size_t operator()() const;
	bool operator==(const Handle<T>& rhs) const;
private:
	T* pObject{ nullptr };
};


//USING

#include "Handle.cpp"



#endif // !HANDLE_H
