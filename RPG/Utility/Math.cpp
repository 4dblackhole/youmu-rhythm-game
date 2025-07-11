#include "framework.h"
#include "Math.h"

using namespace DirectX;

float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;
	
	return r1 + val;
}

XMMATRIX Math::InverseTranspose(CXMMATRIX M)
{
	// Inverse-transpose is just applied to normals.  So zero out 
	// translation row so that it doesn't get into our inverse-transpose
	// calculation--we don't want the inverse-transpose of the translation.
	XMMATRIX A = M;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}

XMVECTOR Math::RandUnitVec3()
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(Math::Random(-1.0f, 1.0f), Math::Random(-1.0f, 1.0f), Math::Random(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		return XMVector3Normalize(v);
	}
}

XMVECTOR Math::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(Math::Random(-1.0f, 1.0f), Math::Random(-1.0f, 1.0f), Math::Random(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		// Ignore points in the bottom hemisphere.
		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}

static constexpr int searchPivot = 512;
int Math::FindHighestPowerOfTwo(int n)
{
	if (n < (1 << 7)) return FindHighestPowerOfTwoLinearSearch(n);
	else			  return FindHighestPowerOfTwoBinarySearch(n);
}

int Math::FindHighestPowerOfTwoLinearSearch(int n) {
	static int counter = 0;
	int res = 1;
	while (n > 1) {
		n >>= 1;
		res <<= 1;
		counter += 3;
	}
	return res;
}

int Math::FindHighestPowerOfTwoBinarySearch(int n) {
	int low = 0;
	int high = 31;

	static int counter = 0;
	// 지수에 대해 이진 탐색 수행
	while (low < high) {
		int mid = (low + high + 1) >> 1;  // 중간값 계산
		if ((1 << mid) <= n) {
			low = mid;
		}
		else {
			high = mid - 1;
		}
		counter += 4;
	}

	return (1 << low);  // 2^low 반환
}