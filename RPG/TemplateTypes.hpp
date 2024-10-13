#pragma once
#include "libraries.h"

_EXPORT_STD template <class T>
concept arithmetic = std::is_arithmetic_v<T>; 

struct ColorOperators
{
	static XMFLOAT4 AddColor(const XMFLOAT4& c, const XMFLOAT4& v)
	{
		return XMFLOAT4(c.x + v.x, c.y + v.y, c.z + v.z, c.w + v.w );
	}

	static XMFLOAT4 SubtractColor(const XMFLOAT4& c, const XMFLOAT4& v)
	{
		return XMFLOAT4(c.x - v.x, c.y - v.y, c.z - v.z, c.w - v.w );
	}

	template<arithmetic A>
	static XMFLOAT4 MultiplyColor(const XMFLOAT4& c, const A& v)
	{
		using Type = decltype(c.x);
		const Type& operateVal = (Type)v;
		return XMFLOAT4
		(
			c.x * v,
			c.y * v, 
			c.z * v, 
			c.w
		);
	}

	template<arithmetic A>
	static XMFLOAT4 DivideColor(const XMFLOAT4& c, const A& v)
	{
		using Type = decltype(c.x);
		const Type& operateVal = (Type)v;
		return XMFLOAT4
		(
			c.x / v,
			c.y / v,
			c.z / v,
			c.w
		);
	}
};
