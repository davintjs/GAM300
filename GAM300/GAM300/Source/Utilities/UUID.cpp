/*!***************************************************************************************
\file			UUID.cpp
\project
\author			Matthew Lau

\par			Course: GAM300
\par			Section:
\date			26/08/2023

\brief
	The definition of UUID help functions

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "UUID.h"
#include <random>

namespace
{
	static std::uniform_int_distribution<uint64_t> uniformDistribution;
	static std::random_device randomDevice;
	static std::mt19937_64 randomiser(randomDevice());
}

namespace Engine
{
	UUID CreateUUID()
	{
		return uniformDistribution(randomiser);
	}
}