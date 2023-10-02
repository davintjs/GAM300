/*!***************************************************************************************
\file			SystemsGroup.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			25/09/2023

\brief
	This file contains a definition for a systems group

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#pragma once


template<typename... Ts>
struct SystemsGroup
{	
	//Template pack initialization
	constexpr SystemsGroup(TemplatePack<Ts...>) {}

	//System group
	SystemsGroup() = default;

	//Init all systems
	constexpr static void Init()
	{
		(Ts::Instance().Init(), ...);
	}

	//Updates all systems by calling function
	constexpr static void Update(float dt, std::function<void(ISystem*)> func)
	{
		UNREFERENCED_PARAMETER(dt);
		(func(&Ts::Instance()), ...);
	}

	//Updates all systems by calling update function
	constexpr static void Update(float dt)
	{
		(Ts::Instance().Update(dt), ...);
	}

	//Exit helper to exit in reverse order
	template<typename... T1s>
	constexpr static void ExitHelper(TemplatePack<T1s...>)
	{
		(T1s::Instance().Exit(), ...);
	}

	//Exit all systems in reverse order
	constexpr static void Exit()
	{
		ExitHelper(RevertTypeSeq<Ts...>::type());
	}
};