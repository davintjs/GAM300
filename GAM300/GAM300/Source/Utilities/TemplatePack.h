#ifndef TEMPLATE_PACK_H
#define TEMPLATE_PACK_H

template<typename T, typename... Ts>
static constexpr bool contains()
{
	return std::disjunction_v<std::is_same<T, Ts>...>;
}

struct None{};

template <auto N>
struct TemplateContainer
{};

template <typename T, typename... Ts>
struct TemplatePack
{
	template<typename T1>
	static constexpr bool Has()
	{
		return contains<T1,T, Ts...>();
	}

	static constexpr const size_t Size()
	{
		return 1 + sizeof...(Ts);
	}

	template <typename... T1s>
	constexpr static auto Concatenate(TemplatePack<T1s...> pack)
	{
		return TemplatePack<T,Ts..., T1s...>();
	}

	constexpr static auto Pop() {
		return TemplatePack<Ts...>();
	}
};

#endif // !TEMPLATE_PACK_H