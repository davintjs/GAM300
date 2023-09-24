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
        E_ASSERT(res == map.mMap.end(), "Unable to get a return value from blackboard!");

        return res->second;
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