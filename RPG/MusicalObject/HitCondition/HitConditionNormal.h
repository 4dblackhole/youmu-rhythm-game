#pragma
#include "HitCondition.h"

class HitConditionNormal : public HitCondition
{
public:
	HitConditionNormal(int maxC = 1);
	virtual ~HitConditionNormal();

	virtual void OnHit() override;
	virtual bool IsHitted() override;

};