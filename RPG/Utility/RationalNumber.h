#pragma once
#include "framework.h"

struct RationalNumber
{
public:
	RationalNumber() : numerator(0), denominator(1) {}
	RationalNumber(const RationalNumber& r) = default;
	RationalNumber(RationalNumber&& r) noexcept = default;

	constexpr RationalNumber(INT64 n, UINT64 d) noexcept :
		numerator(n / Math::GCD<UINT64>((UINT64)(n < 0 ? -n : n), d)),
		denominator(d / Math::GCD<UINT64>((UINT64)(n < 0 ? -n : n), d))
	{}

	RationalNumber& operator=(const RationalNumber& r);
	RationalNumber& operator=(RationalNumber&& r) noexcept;

	RationalNumber operator+() const;
	RationalNumber operator-() const;

	RationalNumber operator+(const RationalNumber& r) const;
	RationalNumber operator-(const RationalNumber& r) const;
	RationalNumber operator*(const RationalNumber& r) const;
	RationalNumber operator/(const RationalNumber& r) const;
	RationalNumber operatorInverse() const;

	operator float() { return (float)numerator / (float)denominator; }
	operator double() { return (double)numerator / (double)denominator; }


private:
	INT64 numerator;
	UINT64 denominator;
};