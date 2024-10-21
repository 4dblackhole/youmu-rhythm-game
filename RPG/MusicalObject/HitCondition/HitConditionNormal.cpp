#include "framework.h"
#include "HitConditionNormal.h"

HitConditionNormal::HitConditionNormal(int maxC) : HitCondition(maxC)
{
}

HitConditionNormal::~HitConditionNormal()
{
}

void HitConditionNormal::OnHit()
{
	++hitCount;
}

bool HitConditionNormal::IsHitted()
{
	return maxHitCount <= hitCount;
}
