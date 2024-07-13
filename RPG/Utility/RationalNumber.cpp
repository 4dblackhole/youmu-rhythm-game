#include "framework.h"
#include "RationalNumber.h"

RationalNumber& RationalNumber::operator=(const RationalNumber& r)
{
    this->numerator = r.numerator;
    this->denominator= r.denominator;
    return *this;
}

RationalNumber& RationalNumber::operator=(RationalNumber&& r) noexcept
{
    this->numerator = r.numerator;
    this->denominator = r.denominator;
    return *this;
}

RationalNumber RationalNumber::operator+() const
{
    return *this;
}

RationalNumber RationalNumber::operator-() const
{
    return RationalNumber(-this->numerator, this->denominator);
}

RationalNumber RationalNumber::operator+(const RationalNumber& r) const
{
    /*
    incase 1/6 + 1/10,
    newDenominator = 30,
    LeftRate = 5, RightRate = 3,
    newNumerator = 5x1 + 3x1 = 8
    result = 8/30

    gcd of 8 and 30 => 2
    so result should be (8/2)/(30/2) => 4/15
    */

    UINT64 newDenominator
        = Math::LCM<UINT64>(this->denominator, r.denominator);

    const UINT64 dLeftRate = newDenominator / this->denominator;
    const UINT64 dRightRate = newDenominator / r.denominator;

    INT64 newNumerator = this->numerator * dLeftRate + r.numerator * dRightRate;

    //reduction of a fraction
    const UINT64 gcd = Math::GCD<UINT64>(abs(newNumerator), newDenominator);
    if (gcd != 1)
    {
        newDenominator /= gcd;
        newNumerator /= (INT64)gcd;
    }

    return { newNumerator, newDenominator };
}
RationalNumber RationalNumber::operator-(const RationalNumber& r) const
{
    return *this + (-r);
}

//absolute function
// function<UINT64(INT64)>abs = [=](INT64 v) -> UINT64 { return v < 0 ? -v : v; };

RationalNumber RationalNumber::operator*(const RationalNumber& r) const
{
    // a/b * c/d
    // reduction first.
    // gcd of a,d
    const UINT64 gcdTop = Math::GCD<UINT64>(abs(this->numerator), r.denominator);
    // gcd of b,c
    const UINT64 gcdBot = Math::GCD<UINT64>(this->denominator, abs(r.numerator));

    const INT64 newNumerator = this->numerator / (INT64)gcdTop * r.numerator / (INT64)gcdBot;
    const UINT64 newDenominator = this->denominator / gcdBot * r.denominator / gcdTop;

    return { newNumerator, newDenominator };
    
}

RationalNumber RationalNumber::operator/(const RationalNumber& r) const
{
    return *this * r.operatorInverse();
}

RationalNumber RationalNumber::operatorInverse() const
{
    if (this->numerator < 0)    return { -((INT64)this->denominator), UINT64(-this->numerator) };
    else                        return {  ((INT64)this->denominator), UINT64( this->numerator) };
}

