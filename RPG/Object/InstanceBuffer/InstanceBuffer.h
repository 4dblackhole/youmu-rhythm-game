#pragma once
#include "framework.h"

class InstanceBuffer : public ComPtr<ID3D11Buffer>
{
public:
	InstanceBuffer();
	~InstanceBuffer();

	void InitBuffer(ID3D11Device*, UINT);

private:
	size_t bufferSize;
	
};