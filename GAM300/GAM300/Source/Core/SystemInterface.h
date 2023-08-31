/*!***************************************************************************************
\file			SystemInterface.h
\project
\author			Zacharie Hong
\co-authors

\par			Course: GAM300
\par			Section:
\date			18/09/2022

\brief
    Contains declarations for ISystem and Singleton<T> for other systems to inherit
    off to have single instanced systems and overridden member functions

All content © 2022 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#ifndef SYSTEM_INTERFACE_H

#define SYSTEM_INTERFACE_H

/**************************************************************************/
/*!
  \brief
    Macro to define a class that should be a system that runs regardless
    of the state of the engine
  \param TYPE
    System/class name
*/
/**************************************************************************/
#define SINGLETON(TYPE) class TYPE final : public Singleton<TYPE>

/**************************************************************************/
/*!
  \brief
    Macro to define a class that should be a system that runs regardless
    of the state of the engine
  \param TYPE
    System/class name
*/
/**************************************************************************/

#define ENGINE_SYSTEM(TYPE) class TYPE final : public ISystem, public Singleton<TYPE> 
/**************************************************************************/
/*!
  \brief
    Macro to define a class that should be a system that runs during the
    game runtime
  \param TYPE
    System/class name
*/
/**************************************************************************/
#define ENGINE_RUNTIME_SYSTEM(TYPE) class TYPE final : public IRuntimeSystem, public Singleton<TYPE>

/**************************************************************************/
/*!
  \brief
    Macro to define a class that should be a system that runs during the
    editor
  \param TYPE
    System/class name
*/
/**************************************************************************/
#define ENGINE_EDITOR_SYSTEM(TYPE) class TYPE final : public IEditorSystem, public Singleton<TYPE>

/**************************************************************************/
/*!
    \brief
    Type based Singleton, restrict instantiating a Singleton of same types
    so it is suitable for single-instance classes like the different
    systems
*/
/**************************************************************************/
//Derived Class = T
template <typename T>
class Singleton
{
public:
    Singleton<T>(Singleton<T>& other) = delete;
    void operator=(const Singleton<T>&) = delete;

    /**************************************************************************/
    /*!
        \brief
        Gets the unique instance of type T and returns it as a pointer

        \return
        Pointer to unique class T.
    */
    /**************************************************************************/
    static T& Instance()
    {
        static T instance;
        return instance;
    }
protected:
    Singleton<T>(){}
};


enum SystemMode
{
    ENUM_SYSTEM_RUNTIME = 0b00000001,
    ENUM_SYSTEM_EDITOR  = 0b00000010,
    ENUM_SYSTEM_ALL     = 0b00000011,

};

/**************************************************************************/
/*!
    \brief
    Pure virtual class for systems to implement
*/
/**************************************************************************/
class ISystem
{
    public:
        /**************************************************************************/
        /*!
            \brief
                Runs when engine starts
        */
        /**************************************************************************/
        virtual void Init() = 0;
        /**************************************************************************/
        /*!
            \brief
                Runs every frame of the engine
        */
        /**************************************************************************/
        virtual void Update() = 0;
        /**************************************************************************/
        /*!
            \brief
                Run when the engine exits
        */
        /**************************************************************************/
        virtual void Exit() = 0;

        virtual SystemMode GetMode() const {return ENUM_SYSTEM_ALL;}
};

class IRuntimeSystem : public ISystem
{
    virtual SystemMode GetMode() const{return ENUM_SYSTEM_RUNTIME;}
};

class IEditorSystem : public ISystem
{
    virtual SystemMode GetMode() const{ return ENUM_SYSTEM_EDITOR; }
};

#endif // !SYSTEM_INTERFACE_H
