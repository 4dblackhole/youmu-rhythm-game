#pragma once
#include "libraries.h"

template<arithmetic POS, typename VAL>
VAL Interpolation_Linear(const POS& startPos, const POS& endPos, const VAL& startVal, const VAL& endVal, const POS& currentPos)
{
	const POS& area = endPos - startPos;
	const POS& currentPosInArea = currentPos - startPos;

	const VAL& left = startVal * (area - currentPosInArea);
	const VAL& right = endVal * currentPosInArea;
	return (left + right) / area;
}

template<arithmetic POS, typename VAL>
VAL Interpolation_Exponential(const POS& startPos, const POS& endPos, const VAL& startVal, const VAL& endVal, const POS& currentPos)
{
	const POS& area = endPos - startPos;
	const POS& currentPosInArea = currentPos - startPos;

	const VAL& left = pow(startVal, (area - currentPosInArea) / area);
	const VAL& right = pow(endVal, currentPosInArea / area);
	return left * right;
}

template<arithmetic POS, typename VAL>
VAL Interpolation_Harmonic(const POS& startPos, const POS& endPos, const VAL& startVal, const VAL& endVal, const POS& currentPos)
{
	const POS& area = endPos - startPos;
	const POS& currentPosInArea = currentPos - startPos;

	const VAL& focusing = endVal / startVal * area;
	const VAL& resultA = focusing - (focusing - area) * (currentPosInArea / area);

	return endVal * (area / resultA);
}

enum class InterpolationType
{
	No,
	Linear,
	Exponential,
	Harmonic,
	Ect,
	MAX
};

template<arithmetic POS, typename VAL>
VAL Interpolation(const InterpolationType& type, const POS& startPos, const POS& endPos, const VAL& startVal, const VAL& endVal, const POS& currentPos)
{
	switch (type)
	{
	case InterpolationType::Linear: return Interpolation_Linear(startPos, endPos, startVal, endVal, currentPos);
	case InterpolationType::Exponential: return Interpolation_Exponential(startPos, endPos, startVal, endVal, currentPos);
	case InterpolationType::Harmonic: return Interpolation_Harmonic(startPos, endPos, startVal, endVal, currentPos);
	default: return endVal;
	}
}
