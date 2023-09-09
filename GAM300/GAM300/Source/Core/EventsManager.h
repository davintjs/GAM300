/*!***************************************************************************************
\file			events-system.h
\project		GAM200
\author			Zacharie Hong

\par			Course: GAM200
\par			Section: A
\date

\brief
    This file contains the declarations of the following:
    1. IEventHandler
    2. MemberFunctionHandler
    3. Event System

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef EVENTS_SYSTEM_H

#define EVENTS_SYSTEM_H

#include "Events.h"
#include "SystemInterface.h"
#include <typeindex>
#include <list>
#include <map>

#define EVENTS EventsManager::Instance()

class IEventHandler
{
public:
    void exec(IEvent* evnt)
    {
        call(evnt);
    }
    virtual ~IEventHandler() {};
private:
    // Implemented by MemberFunctionHandler
    virtual void call(IEvent* evnt) = 0;
};

template<class T, class EventType>
class MemberFunctionHandler : public IEventHandler
{
public:
    typedef void (T::* MemberFunction)(EventType*);

    MemberFunctionHandler(T* instance, MemberFunction memberFunction) : instance{ instance }, memberFunction{ memberFunction } {};

    void call(IEvent* evnt) {
        // Cast event to the correct type and call member function
        (instance->*memberFunction)(static_cast<EventType*>(evnt));
    }
private:
    // Pointer to class instance
    T* instance;

    // Pointer to member function
    MemberFunction memberFunction;
};

typedef std::list<IEventHandler*> HandlerList;
SINGLETON(EventsManager) {
private:
    std::map<std::type_index, HandlerList> subscribers;
public:
    /*******************************************************************************
    /*!
    *
    \brief
        Cleanup the IEventHandler

    \return
        void
    */
    /*******************************************************************************/
    ~EventsManager()
    {
        for (auto& keyPair : subscribers)
        {
            auto it{ keyPair.second.begin() };
            while (it != keyPair.second.end())
            {
                delete* it;
                ++it;
            }
        }
    }

    /*******************************************************************************
    /*!
    *
    \brief
        Sends a signal that an event has occurred and also sends the container with the
        variables

    \param evnt
        where the parameters that are to be passed on are stored

    \return
        void
    */
    /*******************************************************************************/
    template<typename EventType>
    void Publish(EventType * evnt)
    {
        HandlerList& handlers = subscribers[typeid(EventType)];

        for (auto handler : handlers) {
            if (handler != nullptr) {
                handler->exec(evnt);
            }
        }
    }

    /*******************************************************************************
    /*!
    *
    \brief
        Become a listener to the event specified

    \param instance
        the object that owns the specified member function

    \param memberFunction
        the member function of the instance

    \return
        void
    */
    /*******************************************************************************/
    template<class T, class EventType>
    void Subscribe(T * instance, void (T:: * memberFunction)(EventType*)) {
        HandlerList& handlers = subscribers[typeid(EventType)];
        handlers.push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
    }
};

#endif // !EVENTS_SYSTEM_H
