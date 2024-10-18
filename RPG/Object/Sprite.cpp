#include "framework.h"
#include "Sprite.h"
#include "Vertex.h"

ID3D11Buffer* Sprite::mVB = nullptr;
ID3D11Buffer* Sprite::mIB = nullptr;
ID3D11InputLayout* Sprite::mInputLayout = nullptr;
ID3D11InputLayout* Sprite::mInsatncedInputLayout = nullptr;

///////////////////////////////////////////////////////////////////////////////
/*
SpriteDesc::SpriteDesc() : Diffuse(1, 1, 1, 1), ColorMode(false), world3d(), textureSRV(nullptr)
{
}

SpriteDesc::~SpriteDesc()
{
}
*/
///////////////////////////////////////////////////////////////////////////////

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

UINT Sprite::GetTextureID(UINT id) const
{
	return TextureID;
}

void Sprite::SetTextureID(UINT id)
{
	const UINT& arrSize = texture->arrSize;
	if (arrSize == 0) TextureID = 0;
	else TextureID = min(id, arrSize - 1);
	
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
	deviceContext->IASetInputLayout(mInputLayout);
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
		deviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		deviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		CXMMATRIX& world = XMLoadFloat4x4(&GetWorld3d().GetTotalDrawWorld());
		CXMMATRIX& uvWorld = XMLoadFloat4x4(&GetWorld3d().GetUvWorld());

		EffectList::SpriteFX->mfxWorld->SetMatrix(reinterpret_cast<const float*>(&world));
		EffectList::SpriteFX->mfxView->SetMatrix(reinterpret_cast<const float*>(&view));
		EffectList::SpriteFX->mfxProj->SetMatrix(reinterpret_cast<const float*>(&proj));
		EffectList::SpriteFX->mfxUvWorld->SetMatrix(reinterpret_cast<const float*>(&uvWorld));
		if (texture != nullptr) EffectList::SpriteFX->mfxTexture->SetResource(texture->GetRefSRVConst());
		EffectList::SpriteFX->mfxTextureDiffuse->SetFloatVector(reinterpret_cast<const float*>(&Diffuse));
		EffectList::SpriteFX->mfxTextureID->SetInt(TextureID);

		currentTech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(6, 0, 0);

	}
}

void Sprite::RenderInstanced(ID3D11DeviceContext* deviceContext, const Camera& cam, ID3D11Buffer* instancedBuffer,
	size_t instanceOffset, size_t instanceCount, ID3D11ShaderResourceView* srv)
{
	deviceContext->IASetInputLayout(Sprite::mInsatncedInputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CXMMATRIX& view = cam.View();
	CXMMATRIX& proj = cam.Proj();

	constexpr size_t numBuffers = 2;
	UINT stride[numBuffers] = { sizeof(VertexColorTexture), sizeof(SpriteInstanceData) };
	UINT offset[numBuffers] = { 0, 0 };
	ID3D11Buffer* buffers[numBuffers] = { mVB, instancedBuffer };

	ID3DX11EffectTechnique*& currentTech = EffectList::SpriteFX->mTechTextureInstanced;
	D3DX11_TECHNIQUE_DESC techDesc;
	currentTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//buffer setting
		deviceContext->IASetVertexBuffers(0, numBuffers, buffers, stride, offset);
		deviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		CXMMATRIX view = cam.View();
		CXMMATRIX proj = cam.Proj();

		EffectList::SpriteFX->SetView(view);
		EffectList::SpriteFX->SetProj(proj);
		EffectList::SpriteFX->SetTexture(srv);

		currentTech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexedInstanced(6, (UINT)instanceCount, 0, 0, (UINT)instanceOffset);
	}

}

void Sprite::OnResize()
{
	GetWorld3d().OnParentWorldUpdate();
}

void Sprite::ChangeWidthToCurrentWidth(float w, float h)
{
	GetWorld3d().SetObjectScale({ ShortCut::GetOrthoWidth(w, h) , GetWorld3d().GetObjectScale().y });
	if (texture != nullptr) MakeCenterUV();

	DEBUG_BREAKPOINT;
}

void Sprite::MakeCenterUV()
{
	if (texture->GetRefSRVConst() == nullptr) return;

	const D3D11_TEXTURE2D_DESC& desc = ShortCut::GetDescFromSRV(texture->GetRefSRVConst());
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

void Sprite::ReleaseSpriteBuffer()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mInsatncedInputLayout);
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

	EffectList::SpriteFX->mTechTextureInstanced->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(SpriteInstanceData::LayoutDesc::desc, SpriteInstanceData::LayoutDesc::Length, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInsatncedInputLayout));

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
