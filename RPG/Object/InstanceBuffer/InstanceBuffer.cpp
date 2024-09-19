#include "framework.h"
#include "InstanceBuffer.h"

InstanceBuffer::InstanceBuffer() :
	ComPtr<ID3D11Buffer>(nullptr),
	bufferSize(0)
{
}

InstanceBuffer::~InstanceBuffer()
{
}

void InstanceBuffer::InitBuffer(ID3D11Device* device, UINT buflen)
{
	Reset();
	D3D11_BUFFER_DESC instbd{};
	instbd.Usage = D3D11_USAGE_DYNAMIC;
	instbd.ByteWidth = buflen;
	instbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HR(device->CreateBuffer(&instbd, nullptr, GetAddressOf()));
}
