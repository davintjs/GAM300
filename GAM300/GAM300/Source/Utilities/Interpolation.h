/*!***************************************************************************************
\file			Interpolation.h
\project
\author         Sean Ngo

\par			Course: GAM300
\date           17/10/2023

\brief
	This file contains the declarations of the following:
	1. Interpolation functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#pragma warning( disable : 4505 )

enum class EASINGTYPE
{
	LINEAR,
	EASEIN,
	EASEOUT,
	BEZIER,
	PARAMETRIC
};

static float EaseIn(const float& t)
{
	return t * t;
}

static float EaseOut(const float& t)
{
	return 1 - (1 - t) * (1 - t);
}

static float BezierBlend(const float& t)
{
	return t * t * (3.0f - 2.0f * t);
}

static float ParametricBlend(const float& t)
{
	float sqt = t * t;
	return (sqt / (2.0f * (sqt - t) + 1.0f));
}

template <typename T, size_t SZ = sizeof(T) / sizeof(float)>
static T Interpolate(T _start, T _end, float _value, const float& _duration, const EASINGTYPE& _type)
{
	_value /= _duration;

	switch (_type)
	{
	case EASINGTYPE::EASEIN:
		_value = EaseIn(_value);
		break;
	case EASINGTYPE::EASEOUT:
		_value = EaseOut(_value);
		break;
	case EASINGTYPE::BEZIER:
		_value = BezierBlend(_value);
		break;
	case EASINGTYPE::PARAMETRIC:
		_value = ParametricBlend(_value);
		break;
	case EASINGTYPE::LINEAR:
	default:
		break;
	}
	
	float* fStart = reinterpret_cast<float*>(&_start);
	float* fEnd = reinterpret_cast<float*>(&_end);
	T container{};
	float* fContainer = reinterpret_cast<float*>(&container);
	for (size_t i = 0; i < SZ; ++i)
	{
		*fContainer = (1.f - _value) * (*fStart) + _value * (*fEnd);
		++fContainer;
		++fStart;
		++fEnd;
	}

	return container;
}

#endif // !Interpolation_H

