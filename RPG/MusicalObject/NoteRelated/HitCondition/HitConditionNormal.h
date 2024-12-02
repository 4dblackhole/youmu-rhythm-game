#pragma
#include "HitCondition.h"

class HitConditionNormal : public HitCondition
{
public:
	HitConditionNormal(int hitC);
	virtual ~HitConditionNormal();

	virtual void OnHit() override;
	virtual bool IsHitted() override;
	virtual void Init() override;

	virtual std::optional<int> GetHitCount() const override;

private:
	const int maxHitCount;
	int hitCount;
};