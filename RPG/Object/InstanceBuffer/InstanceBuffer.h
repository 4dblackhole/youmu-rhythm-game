#pragma once
#include "libraries.h"

class InstanceBuffer 
{
public:
	InstanceBuffer();
	virtual ~InstanceBuffer();

	void InitBuffer(ID3D11Device*, UINT);

	const UINT& GetBufferSize() const { return instanceCount; }

private:
	virtual UINT GetBufferSizeFromCount(UINT count) = 0;
	ID3D11Buffer* buffer = nullptr;
	UINT instanceCount;
};