#pragma once
#include "framework.h"
#include "Utility/Math.h"

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

	constexpr RationalNumber() : numerator(0), denominator(1) {}
	constexpr RationalNumber(const RationalNumber& r) : numerator(r.numerator), denominator(r.denominator) {}
	constexpr RationalNumber(RationalNumber&& r) noexcept : numerator(r.numerator), denominator(r.denominator) {}

	SignedType Numerator() const { return numerator; }
	UnsignedType Denominator() const { return denominator; }

	//divide with GCD of n and d, positive integer
	//ex) if you initiate with 2/8 then the result should be 1/4
	constexpr RationalNumber(SignedType n, UnsignedType d)
	{
		numerator = (n / Math::GCD<UnsignedType>((UnsignedType)(n < 0 ? -n : n), d));
		denominator = (d / Math::GCD<UnsignedType>((UnsignedType)(n < 0 ? -n : n), d));
	}

	constexpr RationalNumber(SignedType n) :
		numerator(n), denominator((UnsignedType)1)
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

	RationalNumber& operator +=(const RationalNumber& r)
	{
		(*this) = (*this) + r;
		return *this;
	}

	RationalNumber& operator -=(const RationalNumber& r)
	{
		(*this) = (*this) - r;
		return *this;
	}

	RationalNumber& operator *=(const RationalNumber& r)
	{
		(*this) = (*this) * r;
		return *this;
	}

	RationalNumber& operator /=(const RationalNumber& r)
	{
		(*this) = (*this) / r;
		return *this;
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

	constexpr bool operator<(const RationalNumber& r) const
	{
		if ((*this - r).numerator < 0) return true;
		else return false;
	}

	constexpr bool operator>(const RationalNumber& r) const
	{
		return r < *this;
	}

	constexpr bool operator<=(const RationalNumber& r) const
	{
		return !(*this > r);
	}

	constexpr bool operator>=(const RationalNumber& r) const
	{
		return !(*this < r);
	}

	constexpr bool operator==(const RationalNumber& r) const
	{
		return (*this - r).numerator == 0;
	}

	constexpr bool operator!=(const RationalNumber& r) const
	{
		return !(*this == r);
	}

	template <typename T,
		typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
	RationalNumber& operator=(const T& val) noexcept
	{
		this->numerator = val;
		this->denominator = 1;
		return *this;
	}

	template <typename T,
		typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
	RationalNumber& operator=(T&& val) noexcept
	{
		this->numerator = val;
		this->denominator = 1;
		return *this;
	}

	template <typename T,
		typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
	RationalNumber operator*(const T& value) const
	{
		return (*this) * RationalNumber(value);
	}

	template <typename Flt,
		typename std::enable_if_t<std::is_floating_point<Flt>::value>* = nullptr>
	friend Flt operator*(Flt value, const RationalNumber& myVal)
	{
		return value * (Flt)myVal.numerator / (Flt)myVal.denominator;
	}

	template <typename T,
		typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
	constexpr bool operator==(const T& value) const
	{
		return ((*this).numerator - (value * ((*this).denominator))) == 0;
	}

	template <typename T,
		typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
	constexpr bool operator!=(const T& value) const
	{
		return !(*this == value);
	}

	template <typename Flt,
		typename std::enable_if_t<std::is_floating_point<Flt>::value>* = nullptr>
	operator Flt() const { return (Flt)numerator / (Flt)denominator; }

private:
	SignedType numerator;
	UnsignedType denominator;
};

