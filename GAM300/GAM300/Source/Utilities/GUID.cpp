
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
		std::string first{ hexString.begin(),hexString.begin() + 16 };
		std::string second{ hexString.begin() + 16,hexString.end() };
		longInt[0] = std::stoull(first.c_str(), 0, 16);
		longInt[1] = std::stoull(second.c_str(), 0, 16);
	}
}