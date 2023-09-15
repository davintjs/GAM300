#ifndef TEMPLATE_PACK_H
#define TEMPLATE_PACK_H
#include <tuple>
#include <type_traits>
#include <variant>

template<typename T, typename... Ts>
static constexpr bool contains()
{
	return std::disjunction_v<std::is_same<T, Ts>...>;
}

struct None{};

template <auto N>
struct TemplateContainer
{};

template <typename... Ts>
struct TemplatePack
{
	constexpr TemplatePack(){};
	constexpr TemplatePack(std::variant<Ts...> pack){};

	template<typename T1>
	static constexpr bool Has()
	{
		return contains<T1,Ts...>();
	}

	static constexpr const size_t Size()
	{
		return sizeof...(Ts);
	}

	template <typename... T1s>
	constexpr static auto Concatenate(TemplatePack<T1s...> pack)
	{
		return TemplatePack<Ts..., T1s...>();
	}

	constexpr static auto Pop() {
		return TemplatePack<Ts...>();
	}

};

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