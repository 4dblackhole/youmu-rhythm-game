#include "framework.h"
#include "InstanceBuffer.h"

InstanceBuffer::InstanceBuffer() :
	ComPtr<ID3D11Buffer>(nullptr),
	instanceCount(0)
{
}

InstanceBuffer::~InstanceBuffer()
{
}

void InstanceBuffer::InitBuffer(ID3D11Device* device, UINT instanceCount)
{
	Reset();
	D3D11_BUFFER_DESC instbd{};
	instbd.Usage = D3D11_USAGE_DYNAMIC;
	instbd.ByteWidth = GetBufferSizeFromCount(instanceCount);
	instbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceCount = instbd.ByteWidth;

	HR(device->CreateBuffer(&instbd, nullptr, GetAddressOf()));
}
