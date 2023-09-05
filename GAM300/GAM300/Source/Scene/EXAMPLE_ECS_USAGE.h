#include "Scene/SceneManager.h"


//REGISTERING/CREATION OF COMPONENTS
//1. Define all components in Components.h
//2. Scroll all the way down, until you see a pragma region COMPONENTS, define your component there
//3. Append to the template pack below the COMPONENTS region
	//SingleComponentTypes if the entity should only have one of it
	//MultiComponentTypes if the entity should multiple have of it

void ITERATE_THROUGH_COMPONENTS()
{
	Scene& scene = MySceneManager.GetCurrentScene();

	//METHOD 1___________________________________________________________________________________
	
	//Iterator method, MOST PREFERRED
	//You can substitute Rigidbody for any other kinds of components like Transform,Tag,Script,etc...
	auto& rbArray = scene.GetComponentsArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it)
	{
		//Get object from iterator
		Rigidbody& rb = *it;
		//Get Entity owning this rigidbody
		Entity& entity = scene.GetEntity(rb);

		//If object is not active, skip
		if (!it.IsActive())
			continue;

		//Do something with object

	}

	//METHOD 2___________________________________________________________________________________

	//For range Loop, LEAST PREFERRED
	//You can substitute Rigidbody for any other kinds of components
	auto& rbArray = scene.GetComponentsArray<Rigidbody>();
	size_t i = 0;
	for (Rigidbody& rb : rbArray)
	{
		if (rbArray.IsActive(i));
		{
			//DO SOMETHING WITH OBJECT
		}
		++i;
	}
}

void ITERATE_THROUGH_ENTITIES()
{
	Scene& scene = MySceneManager.GetCurrentScene();
	for (Entity& entity : scene.entities)
	{
		if (scene.)
	}
}

void CREATION()
{
	Entity& entity = 
}

void ITERATE_