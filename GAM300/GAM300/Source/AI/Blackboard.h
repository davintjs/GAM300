/*!***************************************************************************************
\file			Blackboard.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
    This file contains the declarations of the following:
    1. Blackboard class
        a. For sharing of information between AI agents, or any system if required

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include <unordered_map>

template <typename K>
class my_unordered_map
{
public:
    std::unordered_map<const char*, K> mMap;
};

ENGINE_SYSTEM(Blackboard)
{
public:

    // Setter function to write to blackboard
    template <typename T>
    void setValue(const char* key, const T& value)
    {
        getMap<T>().mMap[key] = value;
    }

    // Getter function to get the message from blackboard
    template <typename T>
    T getValue(const char* key) const
    {
        const auto& map = getMap<T>();
        auto res = map.mMap.find(key);
        E_ASSERT(res == map.mMap.end(), "Unable to get a return value from blackboard!");

        return res->second;
    }
    
    // Blackboard initialization
    void Init() {};

    // Blackboard update loop
    void Update(float) {};

    // Blackboard exit
    void Exit() {};

private:

    // Returns a static instance of the templated map that stores any types of value
    template <typename T>
    static my_unordered_map<T>& getMap()
    {
        static my_unordered_map<T> instance;
        return instance;
    }

};