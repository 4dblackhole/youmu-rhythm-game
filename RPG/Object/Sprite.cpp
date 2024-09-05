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
	
	GetWorld3d().SetObjectScale({_w, _h});
	GetWorld3d().SetLocalPosition({ _x, _y, 0 });

}

void Sprite::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	deviceContext->IASetInputLayout(mInputLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VertexColorTexture);
	UINT offset = 0;

	CXMMATRIX& view = cam.View();
	CXMMATRIX& proj = cam.Proj();

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique*& currentTech = ColorMode ? EffectList::SpriteFX->mTechColor : EffectList::SpriteFX->mTechTexture;
	currentTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//buffer setting
		deviceContext->IASetVertexBuffers(0, 1, mVB.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		CXMMATRIX& world = XMLoadFloat4x4(&GetWorld3d().GetTotalDrawWorld());
		CXMMATRIX& uvWorld = XMLoadFloat4x4(&GetWorld3d().GetUvWorld());

		EffectList::SpriteFX->mfxWorld->SetMatrix(reinterpret_cast<const float*>(&world));
		EffectList::SpriteFX->mfxView->SetMatrix(reinterpret_cast<const float*>(&view));
		EffectList::SpriteFX->mfxProj->SetMatrix(reinterpret_cast<const float*>(&proj));
		EffectList::SpriteFX->mfxUvWorld->SetMatrix(reinterpret_cast<const float*>(&uvWorld));
		EffectList::SpriteFX->mfxTexture->SetResource(textureSRV);
		EffectList::SpriteFX->mfxTextureDiffuse->SetFloatVector(reinterpret_cast<const float*>(&Diffuse));

		currentTech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(6, 0, 0);
	}
}

void Sprite::RenderInstanced(ID3D11DeviceContext* deviceContext, const Camera& cam, const vector<World3D>& worldArr)
{
}

void Sprite::OnResize()
{
	GetWorld3d().OnParentWorldUpdate();
}

void Sprite::ChangeWidthToCurrentWidth(float w, float h)
{
	GetWorld3d().SetObjectScale({ ShortCut::GetOrthoWidth(w, h) , GetWorld3d().GetObjectScale().y });
	MakeCenterUV();

	DEBUG_BREAKPOINT;
}

void Sprite::MakeCenterUV()
{
	if (textureSRV != nullptr)
	{
		const D3D11_TEXTURE2D_DESC& desc = ShortCut::GetDescFromSRV(textureSRV);
		const float& spriteRate = GetWorld3d().GetObjectScale().y / GetWorld3d().GetObjectScale().x;
		const float& imgRate = (float)desc.Height / (float)desc.Width;
		if (spriteRate < imgRate) //sprite is more width than img
		{
			world3d.SetUvScale({ 1.0f, (spriteRate / imgRate) });
			const XMFLOAT2& uv = world3d.GetUvScale();
			world3d.SetUvPosition({ 0.0f, (1.0f - uv.y) * 0.5f });
		}
		else
		{
			world3d.SetUvScale({ (imgRate / spriteRate), 1.0f });
			const XMFLOAT2& uv = world3d.GetUvScale();
			world3d.SetUvPosition({ (1.0f - uv.x) * 0.5f , 0.0f });
		}
	}
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
	EffectList::SpriteFX->mTechTexture->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(VertexColorTexture::InputLayoutDesc::desc, VertexColorTexture::InputLayoutDesc::Length, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}

void Sprite::RepeatTexture(UINT imgWidth, UINT imgHeight, float imgRate)
{
	const float& widthRate = world3d.GetObjectScale().x / (float)imgWidth;
	const float& heightRate = world3d.GetObjectScale().y / (float)imgHeight;

	world3d.SetUvScale({ widthRate / imgRate, heightRate / imgRate });
}

void Sprite::RepeatTextureInExtraArea(UINT imgWidth, UINT imgHeight)
{
	const float& spriteW = world3d.GetObjectScale().x;
	const float& spriteH = world3d.GetObjectScale().y;

	const float& spriteRate = spriteH / spriteW;
	const float& imgRate = (float)imgHeight / (float)imgWidth;

	//in case the sprite is more width than img
	if (spriteRate < imgRate) world3d.SetUvScale({ spriteRate / imgRate, 1.0f });
	else world3d.SetUvScale({ 1.0f, spriteRate / imgRate});
}
