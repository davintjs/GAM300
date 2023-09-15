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
    void Init() {};
    void Update(float dt) {};
    void Exit() {};

private:

    template <typename T>
    static my_unordered_map<T>& getMap()
    {
        static my_unordered_map<T> instance;
        return instance;
    }

};