#pragma once
#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

class Math
{
public:
	static constexpr float PI = 3.14159265f;

	static float Modulo(float val1, float val2);

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);

	static float Clamp(float value, float min, float max);

	static XMMATRIX InverseTranspose(CXMMATRIX M);
	static XMVECTOR RandUnitVec3();
	static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);

	template <typename _Integer>
	static constexpr _Integer GCD(_Integer a, _Integer b) noexcept
	{
		if (b == 0) return a;	// found the GCD
		else return GCD<_Integer>(b, a % b);
	}

	template <typename _Integer>
	static constexpr _Integer LCM(_Integer a, _Integer b) noexcept
	{
		return  (a / GCD<_Integer>(b, a % b)) * b;
	}
};