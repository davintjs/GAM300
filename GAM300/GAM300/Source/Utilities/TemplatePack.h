#ifndef TEMPLATE_PACK_H
#define TEMPLATE_PACK_H

template<typename T, typename... Ts>
static constexpr bool contains()
{
	return std::disjunction_v<std::is_same<T, Ts>...>;
}

template <typename... Ts>
struct TemplatePack
{
	template<typename T>
	static constexpr bool Has()
	{
		return contains<T, Ts...>();
	}

	template <typename... T1s>
	constexpr TemplatePack<Ts..., T1s...> Concatenate(TemplatePack<T1s...> pack)
	{
		return TemplatePack<Ts..., T1s...>();
	}
};

#endif // !TEMPLATE_PACK_H