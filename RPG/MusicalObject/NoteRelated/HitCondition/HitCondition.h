#pragma once
#include "libraries.h"

interface HitCondition
{
public:

	virtual void OnHit() = 0;
	virtual bool IsHitted() = 0;
	virtual void Init() = 0;

	virtual std::optional<int> GetHitCount() const = 0;
}; 
