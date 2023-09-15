#pragma once


template<typename... Ts>
struct SystemsGroup
{
	constexpr SystemsGroup(TemplatePack<Ts...>) {}
	SystemsGroup() = default;

	constexpr static void Init()
	{
		(Ts::Instance().Init(), ...);
	}

	constexpr static void Update(float dt, std::function<void(ISystem*)> func)
	{
		(func(&Ts::Instance()), ...);
	}

	constexpr static void Update(float dt)
	{
		(Ts::Instance().Update(dt), ...);
	}

	template<typename... T1s>
	constexpr static void ExitHelper(TemplatePack<T1s...> pack)
	{
		(T1s::Instance().Exit(), ...);
	}

	constexpr static void Exit()
	{
		ExitHelper(RevertTypeSeq<Ts...>::type());
	}
};