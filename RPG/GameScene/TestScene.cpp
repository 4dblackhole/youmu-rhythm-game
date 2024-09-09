#include "framework.h"
#include "TestScene.h"

TestScene::TestScene() : sq(0.0f, 0.0f, 1280.0f, 720.0f), sq2(300, 100, 259, 224), whiteSquare(300, -100, 259, 224, MyColor4::White, true)
{
	sq.SetTexture(GETTEXTURE(TextureManager::Name::test));
	sq2.SetTexture(GETTEXTURE(TextureManager::Name::myon));

	InitInstanceData();
}

TestScene::~TestScene()
{
}

void TestScene::BeginScene()
{
	BuildBuffer();
	//BuildLayout();

	BuildInstancedBuffer();
	BuildInstancedLayout();
}

void TestScene::OnResize(float newW, float newH)
{
	sq.ChangeWidthToCurrentWidth(newW, newH);
}

void TestScene::Update(float dt)
{
	WhiteSquareLeftRightMove(dt);

	if (KEYBOARD.Down(VK_ESCAPE))
	{
		string* tempStr = new string(SceneManager::Name::Title);
		SceneManager::GetInstance().ChangeScene(*tempStr);
		delete tempStr;
	}
	if (KEYBOARD.Hold('Z'))
	{
		sq2.GetWorld3d().MoveLocalPosition(-400.0f * dt, 0.0f, 0.0f);
	}
	if (KEYBOARD.Hold('X'))
	{
		sq2.GetWorld3d().MoveLocalPosition(400.0f * dt, 0.0f, 0.0f);
	}
}

void TestScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	sq.Render(deviceContext, cam);
	sq2.Render(deviceContext, cam);
	whiteSquare.Render(deviceContext, cam);


	deviceContext->IASetInputLayout(mInstancedLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride[2] = { sizeof(VertexColor),sizeof(InstancedData) };
	UINT offset[2] = { 0, 0 };
	ID3D11Buffer* buffers[2] = {mVB, mInstancedBuffer};

	ID3DX11EffectTechnique*& currentTech = EffectList::InstancedTestFX->mTechInstanced;
	D3DX11_TECHNIQUE_DESC techDesc;
	currentTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//buffer setting
		deviceContext->IASetVertexBuffers(0, 2, buffers, stride, offset);
		deviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);


		// Set per object constants.
		CXMMATRIX world = XMLoadFloat4x4(&XmFloatT4X4Identity);
		CXMMATRIX view = cam.View();
		CXMMATRIX proj = cam.Proj();

		EffectList::InstancedTestFX->SetWorld(world);
		EffectList::InstancedTestFX->SetView(view);
		EffectList::InstancedTestFX->SetProj(proj);


		currentTech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexedInstanced(6, instanceSize, 0, 0, 0);
	}


}

void TestScene::EndScene()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mLayout);
	ReleaseCOM(mInstancedBuffer);
	ReleaseCOM(mInstancedLayout);
}

void TestScene::InitInstanceData()
{
	instances.reserve(instanceSize);
	for (int i = 0; i < instanceSize; ++i)
	{
		InstancedData temp;
		XMStoreFloat4x4(&temp.World, XMMatrixTranslation(-270.0f + 60.0f * (i % 10), 120.0f - 60.0f * (i / 10), 0.0f));
		instances.emplace_back(temp);
	}
}

void TestScene::BuildBuffer()
{
	VertexColor vertex[4] =
	{
		{ XMFLOAT3(-50.0f, -50.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)},
		{ XMFLOAT3(50.0f, -50.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 0.5f) },
		{ XMFLOAT3(-50.0f,  50.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 0.3f)},
		{ XMFLOAT3(50.0f,  50.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)}
	};

	D3D11_BUFFER_DESC vbd{};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexColor) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vinitData{};
	vinitData.pSysMem = vertex;
	HR(App->GetDevice()->CreateBuffer(&vbd, &vinitData, &mVB));


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
	HR(App->GetDevice()->CreateBuffer(&ibd, &iinitData, &mIB));
}

void TestScene::BuildLayout()
{
	D3DX11_PASS_DESC passDesc;
	EffectList::InstancedTestFX->mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(App->GetDevice()->CreateInputLayout(VertexColor::InputLayoutDesc::desc, VertexColor::InputLayoutDesc::Length, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mLayout));

}
void TestScene::BuildInstancedBuffer()
{
	D3D11_BUFFER_DESC instbd{};
	instbd.Usage = D3D11_USAGE_IMMUTABLE;
	instbd.ByteWidth = sizeof(InstancedData) * instanceSize;
	instbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA instinitData{};
	instinitData.pSysMem = &instances[0];
	HR(App->GetDevice()->CreateBuffer(&instbd, &instinitData, &mInstancedBuffer));

}

void TestScene::BuildInstancedLayout()
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
	VertexColor::InputLayoutDesc::desc[0],
	VertexColor::InputLayoutDesc::desc[1],
	{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	D3DX11_PASS_DESC passDesc;
	EffectList::InstancedTestFX->mTechInstanced->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(App->GetDevice()->CreateInputLayout(desc, ARRAYSIZE(desc), passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInstancedLayout));
}

void TestScene::WhiteSquareLeftRightMove(float dt)
{
	static bool directionRight = false;
	float resultX = whiteSquare.GetWorld3d().GetLocalPosition().x;
	constexpr float speed = 500.0f;
	if (directionRight) resultX += dt * speed;
	else				resultX -= dt * speed;

	const float orthoWidth = ShortCut::GetOrthoWidth((float)App->GetWidth(), (float)App->GetHeight());

	if (resultX <= -orthoWidth * 0.5f)
	{
		directionRight = true;
		resultX = -orthoWidth - resultX;
	}
	if (resultX >= orthoWidth * 0.5f)
	{
		directionRight = false;
		resultX = orthoWidth - resultX;
	}

	if (directionRight)
		whiteSquare.GetWorld3d().SetLocalPosition({ resultX, whiteSquare.GetWorld3d().GetLocalPosition().y, 0 });
	else
		whiteSquare.GetWorld3d().SetLocalPosition({ resultX, whiteSquare.GetWorld3d().GetLocalPosition().y, 0 });

}
