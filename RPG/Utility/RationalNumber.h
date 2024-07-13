#pragma once
#include "framework.h"

template <UINT32 Bits>
struct IntegerSelector;

template <>
struct IntegerSelector<8> {
	using Signed = int8_t;
	using Unsigned = uint8_t;
};

template <>
struct IntegerSelector<16> {
	using Signed = int16_t;
	using Unsigned = uint16_t;
};

template <>
struct IntegerSelector<32> {
	using Signed = int32_t;
	using Unsigned = uint32_t;
};

template <>
struct IntegerSelector<64> {
	using Signed = int64_t;
	using Unsigned = uint64_t;
};

template<UINT32 Bits>
struct RationalNumber
{
public:
	using SignedType = typename IntegerSelector<Bits>::Signed;
	using UnsignedType = typename IntegerSelector<Bits>::Unsigned;

	RationalNumber() : numerator(0), denominator(1) {}
	RationalNumber(const RationalNumber& r) : numerator(r.numerator), denominator(r.denominator) {}
	RationalNumber(RationalNumber&& r) noexcept : numerator(r.numerator), denominator(r.denominator) {}

	constexpr RationalNumber(INT64 n, UINT64 d) noexcept :
		numerator(n / Math::GCD<UINT64>((UINT64)(n < 0 ? -n : n), d)),
		denominator(d / Math::GCD<UINT64>((UINT64)(n < 0 ? -n : n), d))
	{}

	RationalNumber& operator=(const RationalNumber& r)
	{
		this->numerator = r.numerator;
		this->denominator = r.denominator;
		return *this;
	}

	RationalNumber& operator=(RationalNumber&& r) noexcept
	{
		this->numerator = r.numerator;
		this->denominator = r.denominator;
		return *this;
	}

	RationalNumber operator+() const { return *this; }
	RationalNumber operator-() const
	{
		return RationalNumber<Bits>(-this->numerator, this->denominator);
	}

	RationalNumber operator+(const RationalNumber& r) const
	{
		UnsignedType newDenominator = Math::LCM<UnsignedType>(this->denominator, r.denominator);

		const UnsignedType dLeftRate = newDenominator / this->denominator;
		const UnsignedType dRightRate = newDenominator / r.denominator;

		SignedType newNumerator = this->numerator * dLeftRate + r.numerator * dRightRate;

		//reduction of a fraction
		const UnsignedType gcd = Math::GCD<UnsignedType>(abs(newNumerator), newDenominator);
		if (gcd != 1)
		{
			newDenominator /= gcd;
			newNumerator /= (SignedType)gcd;
		}

		return { newNumerator, newDenominator };
	}
	RationalNumber operator-(const RationalNumber& r) const
	{
		return *this + (-r);
	}

	RationalNumber operator*(const RationalNumber& r) const
	{
		// a/b * c/d
		// reduction first.
		// gcd of a,d
		const UnsignedType gcdTop = Math::GCD<UnsignedType>(abs(this->numerator), r.denominator);
		// gcd of b,c
		const UnsignedType gcdBot = Math::GCD<UnsignedType>(this->denominator, abs(r.numerator));

		const SignedType newNumerator = this->numerator / (SignedType)gcdTop * r.numerator / (SignedType)gcdBot;
		const UnsignedType newDenominator = this->denominator / gcdBot * r.denominator / gcdTop;

		return { newNumerator, newDenominator };

	}
	RationalNumber operator/(const RationalNumber& r) const
	{
		return *this * r.operatorInverse();
	}

	RationalNumber operatorInverse() const
	{
		if (this->numerator < 0)    return { -((SignedType)this->denominator), UnsignedType(-this->numerator) };
		else                        return { ((SignedType)this->denominator), UnsignedType(this->numerator) };
	}

	operator float() { return (float)numerator / (float)denominator; }
	operator double() { return (double)numerator / (double)denominator; }

private:
	SignedType numerator;
	UnsignedType denominator;
};