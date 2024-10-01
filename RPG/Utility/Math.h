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
	
	static int FindHighestPowerOfTwo(int n);
	static int FindHighestPowerOfTwoLinearSearch(int n);
	static int FindHighestPowerOfTwoBinarySearch(int n);
	
	static float Clamp(float value, float min, float max);

	static XMMATRIX InverseTranspose(CXMMATRIX M);
	static XMVECTOR RandUnitVec3();
	static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);

	template <std::integral _Integer>
	static constexpr _Integer GCD(_Integer a, _Integer b)
	{
		if (b == 0) return a;	// found the GCD
		else return GCD<_Integer>(b, a % b);
	}
	
	template <std::integral _Integer>
	static constexpr _Integer LCM(_Integer a, _Integer b)
	{
		return  (a / GCD<_Integer>(b, a % b)) * b;
	}

	// ltrb -> xywh (left top right bottom -> centerX centerY width height
	template <typename T>
	static void LTRB2CenterXYWH(_Inout_ T& l, _Inout_ T& t, _Inout_ T& r, _Inout_ T& b)
	{
		const T width = r - l;
		const T height = b - t;
		const T centerX = (T)((l + r) * 0.5);
		const T centerY = (T)((t + b) * 0.5);
		l = centerX;
		t = centerY;
		r = width;
		b = height;
	}

	// reverse of the above
	template <typename T>
	static void CenterXYWH2LTRB(_Inout_ T& x, _Inout_ T& y, _Inout_ T& w, _Inout_ T& h)
	{
		const T halfWidth = w * 0.5f;
		const T halfHeight = h * 0.5f;

		const T left = x - halfWidth;
		const T right = x + halfWidth;
		const T top = y - halfHeight;
		const T bottom = y + halfHeight;

		x = left;
		y = top;
		w = right;
		h = bottom;
	}
};