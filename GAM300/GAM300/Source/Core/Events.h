/*!***************************************************************************************
\file			events.h
\project
\author			Zacharie Hong
				Sean Ngo

\par			Course: GAM200
\par			Section:
\date			15/02/2023

\brief
	This file contains the declarations of all event structures in the engine.

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef EVENTS_H
#define EVENTS_H

#include "Scene/Entity.h"
#include "Scene/Components.h"

struct IEvent
{
protected:
	virtual ~IEvent() {};
};

struct StartPreviewEvent : IEvent
{
	StartPreviewEvent(){}
};

struct StopPreviewEvent : IEvent
{
	StopPreviewEvent(){}
};

struct QuitEngineEvent : IEvent
{
	QuitEngineEvent() {};
};

template <typename T>
struct ComponentAddEvent : IEvent
{
	ComponentAddEvent(const Entity& _entity, T*& _componentContainer, UUID _uuid = UUID()) :
		gameObject{ _gameObject }, componentContainer{ _componentContainer }, uuid{_uuid} {}
	Entity& entity;
	T*& componentContainer;
	UUID uuid;
};

template <typename T>
struct ComponentDeleteEvent : IEvent
{
	ComponentDeleteEvent(T& _component) : component{_component}{}
	T& component;
};

#endif //!EVENTS_H