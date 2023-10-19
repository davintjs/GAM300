
#include <random>
#include "GUID.h"

namespace
{
	static std::uniform_int_distribution<uint64_t> uniformDistribution;
	static std::random_device randomDevice;
	static std::mt19937_64 randomiser(randomDevice());

	size_t CreateRandomNumber()
	{
		return uniformDistribution(randomiser);
	}
}

namespace Engine
{
	GUID::GUID()
	{
		longInt[0] = CreateRandomNumber();
		longInt[1] = CreateRandomNumber();
	}

	GUID::GUID(const GUID& rhs)
	{
		memcpy(longInt, rhs.longInt, sizeof(longInt));
	}

	GUID::GUID(const std::string& hexString)
	{
		if (hexString.size() == 0)
		{
			longInt[0] = 0;
			longInt[1] = 0;
			return;
		}

	    if (hexString.size() < 32)
		{
		    std::string first{ hexString.begin(),hexString.begin() + 16 };
		    std::string second{ hexString.begin() + 16,hexString.end() };
    		longInt[0] = std::stoull(first.c_str(), 0, 16);
    		longInt[1] = std::stoull(second.c_str(), 0, 16);
		}
		else
		{
		    std::string first{ hexString.begin(),hexString.begin() + 16 };
		    std::string second{ hexString.begin() + 16,hexString.begin()+32 };
    		longInt[0] = std::stoull(first.c_str(), 0, 16);
    		longInt[1] = std::stoull(second.c_str(), 0, 16);
		}
	}


}

namespace std
{
	std::size_t hash<Engine::GUID>::operator()(const Engine::GUID& obj) const
	{
		return obj.longInt[0] ^ obj.longInt[1];  // Combine the hashes
	}
}
