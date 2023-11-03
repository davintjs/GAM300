/*!***************************************************************************************
\file			UUID.h
\project
\author			Matthew Lau

\par			Course: GAM300
\par			Section:
\date			26/08/2023

\brief
	The declaration of UUID type

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#pragma once

#ifndef UUID_H
#define UUID_H

namespace Engine
{
	size_t CreateUUID();
	struct UUID 
	{
		UUID(size_t _data = CreateUUID()){}
		operator size_t () { return data; }
		size_t data{};
	};
	using UUID = size_t;
	//Generates a random UUID
}


#endif // !UUID_H

