#pragma once
#include "framework.h"

class InstanceBuffer : public ComPtr<ID3D11Buffer>
{
public:
	InstanceBuffer();
	virtual ~InstanceBuffer();

	void InitBuffer(ID3D11Device*, UINT);

	const UINT& GetBufferSize() const { return instanceCount; }

private:
	virtual UINT GetBufferSizeFromCount(UINT count) = 0;
	UINT instanceCount;
};