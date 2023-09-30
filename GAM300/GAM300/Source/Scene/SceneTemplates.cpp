/*!***************************************************************************************
****
\file			
\project		
\author			
\co-authors		// DELETE IF NOT APPLICABLE

\par			Course: GAM200
\par			Section: 
\date			//DATE LAST MODIFIED

\brief
	// BRIEF DESCRIPTION OF FILE PURPOSE

All content � 2022 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************
****/


#include "Scene.h"

template<typename T>
T& Scene::GetByUUID(Engine::UUID uuid)
{
	if constexpr (MultiComponentTypes::Has<T>())
	{
		return multiHandles.GetByUUID<T>(uuid);
	}
	else
	{
		return singleHandles.GetByUUID<T>(uuid);
	}
}