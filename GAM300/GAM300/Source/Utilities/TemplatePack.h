/*!***************************************************************************************
\file			TemplatePack.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			02/09/2023

\brief
	This file contains a template pack type to help with ECS type deductions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/



#ifndef TEMPLATE_PACK_H
#define TEMPLATE_PACK_H
#include <tuple>
#include <type_traits>
#include <variant>

//Checks if Ts... has T
template<typename T, typename... Ts>
static constexpr bool contains()
{
	return std::disjunction_v<std::is_same<T, Ts>...>;
}

template <typename... Ts>
struct TemplatePack
{
	//Default constructor
	constexpr TemplatePack(){};
	
	//Construct from variant
	constexpr TemplatePack(std::variant<Ts...> pack){};

	//Checks if template pack has this type
	template<typename T1>
	static constexpr bool Has()
	{
		return contains<T1,Ts...>();
	}

	//Gets the amount of elements in a template pack
	static constexpr const size_t Size()
	{
		return sizeof...(Ts);
	}

	//Combines two template packs together
	template <typename... T1s>
	constexpr static auto Concatenate(TemplatePack<T1s...>)
	{
		return TemplatePack<Ts..., T1s...>();
	}

	//Remove first element from a template pack
	constexpr static auto Pop() {
		return TemplatePack<Ts...>();
	}

};

//Reverse the order of elements in a template pack
template<typename, typename>
struct AppendToTypeSeq { };

template<typename T, typename... Ts>
struct AppendToTypeSeq<T, TemplatePack<Ts...>>
{
	using type = TemplatePack<Ts..., T>;
};

template<typename... Ts>
struct RevertTypeSeq
{
	using type = TemplatePack<>;
};

template<typename T, typename... Ts>
struct RevertTypeSeq<T, Ts...>
{
	using type = typename AppendToTypeSeq<T,typename RevertTypeSeq<Ts...>::type>::type;
};

#endif // !TEMPLATE_PACK_H