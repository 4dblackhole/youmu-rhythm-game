#pragma once
#include "libraries.h"

template<typename POS, typename VAL>
VAL Interpolation_Linear(const POS& startPos, const POS& endPos, const VAL& startVal, const VAL& endVal, const POS& currentPos)
{
	const POS& area = endPos - startPos;
	const POS& currentPosInArea = currentPos - startPos;

	const VAL& left = startVal * (VAL)(area - currentPosInArea);
	const VAL& right = endVal * (VAL)currentPosInArea;
	return (left + right) / area;
}

template<typename POS, typename VAL>
VAL Interpolation_Exponential(const POS& startPos, const POS& endPos, const VAL& startVal, const VAL& endVal, const POS& currentPos)
{
	const POS& area = endPos - startPos;
	const POS& currentPosInArea = currentPos - startPos;

	const VAL& left = pow(startVal, (VAL)(area - currentPosInArea) / area);
	const VAL& right = pow(endVal, (VAL)currentPosInArea / area);
	return left * right;
}

template<typename POS, typename VAL>
VAL Interpolation_Harmonic(const POS& startPos, const POS& endPos, const VAL& startVal, const VAL& endVal, const POS& currentPos)
{
	const POS& area = endPos - startPos;
	const POS& currentPosInArea = currentPos - startPos;

	const VAL& focusing = (VAL)area * endVal / startVal;
	const VAL& resultA = focusing + (area - focusing) * (currentPosInArea / area);

	return endVal * area / resultA;
}