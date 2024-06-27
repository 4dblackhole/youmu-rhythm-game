#include "framework.h"
#include "Vertex.h"
#include "Square.h"

ComPtr<ID3D11Buffer> Square::mVB;
ComPtr<ID3D11Buffer> Square::mIB;
ComPtr<ID3D11InputLayout> Square::mInputLayout;

Square::Square()
{
	Init(0, 0, 1, 1);
}

Square::Square(float w, float h)
{
	Init(0, 0, w, h);
}

Square::Square(float x, float y, float w, float h)
{
	Init(x, y, w, h);
}

Square::~Square()
{
}

void Square::Init(float _x, float _y, float _w, float _h)
{
	XMMATRIX I = ::XMMatrixIdentity();
	x = _x;
	y = _y;
	w = _w;
	h = _h;

	Scale = { w,h };
	Rotation = { 0,0,0 };
	Position = { x,y,0 };

	XMMATRIX S = XMMatrixScalingFromVector(XMLoadFloat2(&Scale));
	XMMATRIX T = XMMatrixTranslationFromVector(XMLoadFloat3(&Position));
	XMStoreFloat4x4(&mWorld, S * T);
}

void Square::Render(ID3D11DeviceContext* deviceContext, const Camera& cam) const
{
	deviceContext->IASetInputLayout(mInputLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	XMMATRIX view = cam.View();
	XMMATRIX proj = cam.Proj();

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::SquareFX->mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//
		// Draw the hills.
		//
		deviceContext->IASetVertexBuffers(0, 1, mVB.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		CXMMATRIX world = XMLoadFloat4x4(&mWorld);
		CXMMATRIX WVP = world * cam.View() * cam.Proj();

		Effects::SquareFX->mfxWorld->SetMatrix(reinterpret_cast<const float*>(&world));
		Effects::SquareFX->mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&WVP));
		Effects::SquareFX->mfxTexture->SetResource(textureSRV);


		Effects::SquareFX->mTech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(6, 0, 0);
	}
}

void Square::BulidBuffer(ID3D11Device* device)
{
	VertexTexture vertex[4] =
	{
		{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 0.5f,  0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f) }
	};
	

	D3D11_BUFFER_DESC vbd{};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexTexture) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vinitData{};
	vinitData.pSysMem = vertex;
	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//
	UINT indices[] =
	{
		0,3,1,
		0,2,3
	};

	D3D11_BUFFER_DESC ibd{};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA iinitData{};
	iinitData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Square::BuildLayout(ID3D11Device* device)
{
	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	Effects::SquareFX->mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(VertexTexture::InputLayoutDesc::desc, VertexTexture::InputLayoutDesc::Length, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}
