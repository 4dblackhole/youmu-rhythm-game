#include "framework.h"
#include "HitConditionNormal.h"

HitConditionNormal::HitConditionNormal(int hitC) : maxHitCount(hitC)
{
	Init();
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

void HitConditionNormal::Init()
{
	hitCount = maxHitCount;
}

std::optional<int> HitConditionNormal::GetHitCount() const
{
	return hitCount;
}
