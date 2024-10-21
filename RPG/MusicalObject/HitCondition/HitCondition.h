#pragma once
#include "libraries.h"

class HitCondition
{
public:
	HitCondition(int maxC = 1);
	virtual ~HitCondition();

	virtual void OnHit() = 0;
	virtual bool IsHitted() = 0;

	inline int GetHitCount() const { return hitCount; }

protected:
	const int maxHitCount;
	int hitCount = 0;
}; 
