#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#pragma once

namespace Engine
{
	struct GUID
	{
		GUID();
		GUID(const GUID& rhs);
		GUID(const size_t val)
		{
			longInt[1] = val;
			longInt[0] = 0;
		}
		explicit GUID(const std::string& hexString);

		bool operator==(const GUID& rhs) const
		{
			for (int i = 0; i < 2; ++i)
			{
				if (longInt[i] != rhs.longInt[i])
				{
					return false;
				}
			}
			return true;
		}

		GUID& operator=(const GUID& rhs)
		{
			memcpy(longInt, rhs.longInt, sizeof(longInt));
			return *this;
		}

		std::string ToHexString() const
		{
			std::stringstream hex;
			hex << std::hex;
			hex << std::setfill('0');
			for (size_t i : longInt)
				hex << std::setw(16) << i;
			return hex.str();
		}
	private:
		size_t longInt[2];
		friend std::hash<Engine::GUID>;
	};
}

namespace std 
{
	template <>
	struct hash<Engine::GUID> {
		std::size_t operator()(const Engine::GUID& obj) const;
	};
}