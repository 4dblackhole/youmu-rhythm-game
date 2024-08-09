#include "framework.h"
#include "Sprite.h"

ComPtr<ID3D11Buffer> Sprite::mVB;
ComPtr<ID3D11Buffer> Sprite::mIB;
ComPtr<ID3D11InputLayout> Sprite::mInputLayout;

struct VertexColorTexture
{
public:
	VertexColorTexture();
	VertexColorTexture(const VertexColorTexture&);
	VertexColorTexture(VertexColorTexture&&) noexcept;
	VertexColorTexture& operator=(const VertexColorTexture&);
	VertexColorTexture& operator=(VertexColorTexture&&) noexcept;

	VertexColorTexture(const XMFLOAT3&, const XMFLOAT4&, const XMFLOAT2&);
	VertexColorTexture(XMFLOAT3&&, XMFLOAT4&&, XMFLOAT2&&) noexcept;

	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT2 Uv;
	float Padding[2]{};

	struct InputLayoutDesc
	{
		static constexpr UINT Length = 3;
		static constexpr D3D11_INPUT_ELEMENT_DESC desc[Length] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(VertexColorTexture::Pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(VertexColorTexture::Pos) + sizeof(VertexColorTexture::Color), D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};
};

VertexColorTexture::VertexColorTexture() : Pos(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f, 1.0f), Uv(0.0f, 0.0f) {}
VertexColorTexture::VertexColorTexture(const VertexColorTexture& p) : Pos(p.Pos), Color(p.Color), Uv(p.Uv) {}
VertexColorTexture::VertexColorTexture(VertexColorTexture&& p) noexcept : Pos(p.Pos), Color(p.Color), Uv(p.Uv) {}

VertexColorTexture& VertexColorTexture::operator=(const VertexColorTexture& p)
{
	this->Pos = p.Pos;
	this->Color = p.Color;
	this->Uv = p.Uv;
	return *this;
}

VertexColorTexture& VertexColorTexture::operator=(VertexColorTexture&& p) noexcept
{
	this->Pos = p.Pos;
	this->Color = p.Color;
	this->Uv = p.Uv;
	return *this;
}

VertexColorTexture::VertexColorTexture(const XMFLOAT3& p, const XMFLOAT4& c, const XMFLOAT2& u) : Pos(p), Color(c), Uv(u) {}
VertexColorTexture::VertexColorTexture(XMFLOAT3&& p, XMFLOAT4&& c, XMFLOAT2&& u) noexcept : Pos(p), Color(c), Uv(u) {}

Sprite::Sprite()
{
	Init(0, 0, 1, 1, XMFLOAT4(1, 1, 1, 1), false);
}

Sprite::Sprite(float w, float h)
{
	Init(0, 0, w, h, XMFLOAT4(1, 1, 1, 1), false);
}

Sprite::Sprite(float x, float y, float w, float h)
{
	Init(x, y, w, h, XMFLOAT4(1, 1, 1, 1), false);
}

Sprite::Sprite(float x, float y, float w, float h, const XMFLOAT4 diffuse, const bool colormode = false)
{
	Init(x, y, w, h, diffuse, colormode);
}

Sprite::~Sprite()
{
}

void Sprite::Init(float _x, float _y, float _w, float _h, const XMFLOAT4 diffuse, const bool colormode = false)
{
	this->ColorMode = colormode;

	XMMATRIX I = ::XMMatrixIdentity();

	//XMStoreFloat4x4(&mParentWorld, I);

	Diffuse = diffuse;

	ObjectScale = { _w, _h };
	ObjectRotation = { 0, 0, 0 };
	ObjectPosition = { 0, 0, 0 };
	UpdateObjectWorld();

	LocalScale = { 1.0f, 1.0f };
	LocalRotation = { 0, 0, 0 };
	LocalPosition = { _x, _y, 0 };
	DrawPos = LocalPosition;
	UpdateLocalWorld();


	UpdateGlobalWorld();

	UvScale = { 1.0f,1.0f };
	UvRotation = { 0,0,0 };
	UvPosition = { 0,0 };
	XMStoreFloat4x4(&mUvWorld, I);
}

//center coordinate system to other system
void Sprite::AdjustDrawPos()
{
	XMFLOAT3 tempPos{};
	const float localWidth = ShortCut::GetOrthoWidth((float)App->GetWidth(), (float)App->GetHeight());
	switch (alignX)
	{
	case AlignModeX::Left:
		tempPos.x = (LocalScale.x - localWidth) * 0.5f;
		break;
	case AlignModeX::Right:
		tempPos.x = (localWidth - LocalScale.x) * 0.5f;
		break;
	}
	switch (alignY)
	{
	case AlignModeY::Top:
		tempPos.y = ((float)StandardHeight - LocalScale.y) * 0.5f;
		break;
	case AlignModeY::Bottom:
		tempPos.y = (LocalScale.y - (float)StandardHeight) * 0.5f;
		break;
	}
	tempPos.x += LocalPosition.x;
	tempPos.y += LocalPosition.y;

	DrawPos = tempPos;
	UpdateDrawArea();
}

void Sprite::UpdateDrawArea()
{
	//screen dimensions
	const float orthoWidth = ShortCut::GetOrthoWidth((float)App->GetWidth(), (float)App->GetHeight());
	const float centerW = App->GetWidth() * 0.5f;
	const float centerH = App->GetHeight() * 0.5f;
	const float rateX = App->GetWidth() / orthoWidth;
	const float rateY = App->GetHeight() / (float)StandardHeight;

	drawArea = {
		centerW + (DrawPos.x - LocalScale.x * 0.5f) * rateX, centerH + (DrawPos.y + LocalScale.y * 0.5f) * -rateY,
		centerW + (DrawPos.x + LocalScale.x * 0.5f) * rateX, centerH + (DrawPos.y - LocalScale.y * 0.5f) * -rateY };

}

const XMFLOAT4X4& Sprite::GetObjectWorld()
{
	UpdateObjectWorld();
	return mObjectWorld;
}

const XMFLOAT4X4& Sprite::GetLocalWorld()
{
	UpdateLocalWorld();
	return mLocalWorld;	
}

const XMFLOAT4X4& Sprite::GetGlobalWorld()
{
	UpdateGlobalWorld();
	return mGlobalWorld;
}

const XMFLOAT4X4& Sprite::GetUvWorld()
{
	UpdateUvWorld();
	return mUvWorld;
}

void Sprite::UpdateObjectWorld()
{
	if (mObjectWorldUpdateFlag)
	{
		XMMATRIX S = XMMatrixScalingFromVector(XMLoadFloat2(&ObjectScale));
		XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&ObjectRotation));
		XMMATRIX T = XMMatrixTranslationFromVector(XMLoadFloat3(&ObjectPosition));
		XMStoreFloat4x4(&mObjectWorld, S * R * T);
		mGlobalWorldUpdateFlag = true;

		mObjectWorldUpdateFlag = false;
	}
}

//reflects the align states and updates world matrix
void Sprite::UpdateLocalWorld()
{
	if (mLocalWorldUpdateFlag)
	{
		XMMATRIX S = XMMatrixScalingFromVector(XMLoadFloat2(&LocalScale));
		XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&LocalRotation));

		AdjustDrawPos();
		XMMATRIX T = XMMatrixTranslationFromVector(XMLoadFloat3(&DrawPos));
		XMStoreFloat4x4(&mLocalWorld, S * R * T);
		mGlobalWorldUpdateFlag = true;

		mLocalWorldUpdateFlag = false;
	}
}

void Sprite::UpdateGlobalWorld()
{
	UpdateObjectWorld();
	UpdateLocalWorld();
	if (mGlobalWorldUpdateFlag)
	{
		if (mParentWorld == nullptr) XMStoreFloat4x4(&mGlobalWorld, XMLoadFloat4x4(&mObjectWorld) * XMLoadFloat4x4(&mLocalWorld));
		else XMStoreFloat4x4(&mGlobalWorld, XMLoadFloat4x4(&mObjectWorld) * XMLoadFloat4x4(&mLocalWorld) * XMLoadFloat4x4(mParentWorld));
		mGlobalWorldUpdateFlag = false;
	}
}

void Sprite::UpdateUvWorld()
{
	if (mUvWorldUpdateFlag)
	{
		XMMATRIX S = XMMatrixScalingFromVector(XMLoadFloat2(&UvScale));
		XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&UvRotation));
		XMMATRIX T = XMMatrixTranslationFromVector(XMLoadFloat2(&UvPosition));
		XMStoreFloat4x4(&mUvWorld, S * R * T);
		mUvWorldUpdateFlag = false;
	}
}

void Sprite::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	deviceContext->IASetInputLayout(mInputLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VertexColorTexture);
	UINT offset = 0;

	XMMATRIX view = cam.View();
	XMMATRIX proj = cam.Proj();

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique*& currentTech = ColorMode ? Effects::SpriteFX->mTechColor : Effects::SpriteFX->mTechTexture;
	currentTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//
		// Draw the hills.
		//
		deviceContext->IASetVertexBuffers(0, 1, mVB.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		CXMMATRIX world = XMLoadFloat4x4(&GetGlobalWorld());

		CXMMATRIX uvWorld = XMLoadFloat4x4(&GetUvWorld());
		CXMMATRIX WVP = world * cam.View() * cam.Proj();

		Effects::SpriteFX->mfxWorld->SetMatrix(reinterpret_cast<const float*>(&world));
		Effects::SpriteFX->mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&WVP));
		Effects::SpriteFX->mfxUvWorld->SetMatrix(reinterpret_cast<const float*>(&uvWorld));
		Effects::SpriteFX->mfxTexture->SetResource(textureSRV);
		Effects::SpriteFX->mfxTextureDiffuse->SetFloatVector(reinterpret_cast<const float*>(&Diffuse));


		currentTech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(6, 0, 0);
	}
}

void Sprite::OnResize()
{
	mLocalWorldUpdateFlag = true;
}

void Sprite::ChangeWidthToCurrentWidth(float w, float h)
{
	LocalScale.x = ShortCut::GetOrthoWidth(w,h)/ (float)ObjectScale.x;
	mLocalWorldUpdateFlag = true;
}

void Sprite::BulidBuffer(ID3D11Device* device)
{
	VertexColorTexture vertex[4] =
	{
		{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 0.5f,  0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }
	};
	

	D3D11_BUFFER_DESC vbd{};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexColorTexture) * 4;
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

void Sprite::BuildLayout(ID3D11Device* device)
{
	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	Effects::SpriteFX->mTechTexture->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(VertexColorTexture::InputLayoutDesc::desc, VertexColorTexture::InputLayoutDesc::Length, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}

void Sprite::SetLocalScale(const XMFLOAT2 s)
{
	LocalScale = s;
	mLocalWorldUpdateFlag = true;
}

void Sprite::SetLocalRotation(const XMFLOAT3 s)
{
	LocalRotation = s;
	mLocalWorldUpdateFlag = true;
}

void Sprite::SetLocalPosition(const XMFLOAT3 s)
{
	LocalPosition = s;
	mLocalWorldUpdateFlag = true;
}

void Sprite::MoveLocalPosition(float x, float y, float z)
{
	if (x) LocalPosition.x += x;
	if (y) LocalPosition.y += y;
	if (z) LocalPosition.z += z;
	mLocalWorldUpdateFlag = true;
}

void Sprite::SetUvScale(const XMFLOAT2 s)
{
	UvScale = s;
	mUvWorldUpdateFlag = true;
}

void Sprite::SetUvRotation(const XMFLOAT3 s)
{
	UvRotation = s;
	mUvWorldUpdateFlag = true;
}

void Sprite::SetUvPosition(const XMFLOAT2 s)
{
	UvPosition = s;
	mUvWorldUpdateFlag = true;
}

void Sprite::SetAlignX(AlignModeX m)
{
	alignX = m;
}

void Sprite::SetAlignY(AlignModeY m)
{
	alignY = m;
}
