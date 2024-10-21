#include "framework.h"
#include "HitConditionNormal.h"

HitConditionNormal::HitConditionNormal(int hitC) : maxHitCount(hitC)
{
	Reset();
}

HitConditionNormal::~HitConditionNormal()
{
}

void HitConditionNormal::OnHit()
{
	--hitCount;
}

bool HitConditionNormal::IsHitted()
{
	return hitCount == 0;
}

void HitConditionNormal::Reset()
{
	hitCount = maxHitCount;
}

std::optional<int> HitConditionNormal::GetHitCount() const
{
	return hitCount;
}
