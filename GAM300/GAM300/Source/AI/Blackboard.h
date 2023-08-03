#include <unordered_map>

#define BLACKBOARD Blackboard::Instance()
#define BLACKBOARD_TERMINATE Blackboard::Terminate()

template <typename K>
class my_unordered_map
{
public:
    std::unordered_map<const char*, K> mMap;
};

class Blackboard
{
public:
    static Blackboard* Instance()
    {
        if (BlackboardInstance == NULL) {
            BlackboardInstance = new Blackboard();
        }
        return BlackboardInstance;
    }

    static void Terminate()
    {
        if (BlackboardInstance != NULL) {
            delete BlackboardInstance;
        }
    }

    template <typename T>
    void setValue(const char* key, const T& value)
    {
        getMap<T>().mMap[key] = value;
    }

    template <typename T>
    T getValue(const char* key) const
    {
        const auto& map = getMap<T>();
        auto res = map.mMap.find(key);
        if (res != map.mMap.end())
        {
            return res->second;
        }
        else
        {
            std::cerr << "Unable to get a return value from blackboard!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

private:
    template <typename T>
    static my_unordered_map<T>& getMap()
    {
        static my_unordered_map<T> instance;
        return instance;
    }

    static Blackboard* BlackboardInstance;
};

Blackboard* Blackboard::BlackboardInstance = nullptr;