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
	struct HexID
	{
		HexID();
		HexID(const size_t val)
		{
			longInt[1] = val;
			longInt[0] = 0;
		}


		explicit HexID(const std::string& hexString);

		std::string ToHexString() const
		{
			std::stringstream hex;
			hex << std::hex;
			hex << std::setfill('0');
			for (size_t i : longInt)
				hex << std::setw(16) << i;
			return hex.str();
		}

		bool operator==(const HexID& rhs) const
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

		HexID& operator=(const HexID& rhs)
		{
			memcpy(longInt, rhs.longInt, sizeof(longInt));
			return *this;
		}
		size_t longInt[2];
	};

	template <typename T>
	struct GUID : HexID
	{
		GUID() : HexID::HexID() {}
		GUID(const size_t val) : HexID::HexID(val){}
		GUID(const GUID& rhs)
		{
			memcpy(longInt, rhs.longInt, sizeof(longInt));
		}
		GUID(const HexID& rhs)
		{
			memcpy(longInt, rhs.longInt, sizeof(longInt));
		}
		explicit GUID(const std::string& hexString) : HexID::HexID(hexString){}


		bool operator==(const HexID& rhs) const { return HexID::operator==(rhs); }
	private:
		friend std::hash<Engine::GUID<T>>;
	};
}

namespace std 
{
	template <typename T>
	struct hash<Engine::GUID<T>> {
		std::size_t operator()(const Engine::GUID<T>& obj) const
		{
			return obj.longInt[0] ^ obj.longInt[1];  // Combine the hashes
		}
	};
}