#include "UUID.h"
#include <random>

namespace
{
	static std::uniform_int_distribution<uint64_t> uniformDistribution;
	static std::random_device randomDevice;
	static std::mt19937_64 randomiser(randomDevice());
}

namespace Engine
{
	UUID CreateUUID()
	{
		return uniformDistribution(randomiser);
	}
}