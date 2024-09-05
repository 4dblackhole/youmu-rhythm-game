#include "framework.h"
#include "World3D.h"

World3D::World3D(XMFLOAT2 size, XMFLOAT3 rot, XMFLOAT3 position) :
	ObjectScale(size), ObjectRotation(rot), LocalPosition(position)
{
	mObjectWorld = XmFloatT4X4Identity;
	mLocalWorld = XmFloatT4X4Identity;
	mGlobalWorld = XmFloatT4X4Identity;
	mTotalDrawWorld = XmFloatT4X4Identity;
	mUvWorld = XmFloatT4X4Identity;
}

World3D::~World3D()
{
}

void World3D::SetCenterPosition(const XMFLOAT3 s)
{
	CenterPosition = s;
	mObjectWorldUpdateFlag = true;
}

void World3D::SetObjectScale(const XMFLOAT2 s)
{
	ObjectScale = s;
	mObjectWorldUpdateFlag = true;
}

void World3D::SetObjectScale(const FLOAT s)
{
	SetObjectScale({ s, s });
}

void World3D::SetObjectRotation(const XMFLOAT3 s)
{
	ObjectRotation = s;
	mObjectWorldUpdateFlag = true;
}

void World3D::SetObjectPosition(const XMFLOAT3 s)
{
	ObjectPosition = s;
	mObjectWorldUpdateFlag = true;
}

void World3D::SetLocalScale(const XMFLOAT2 s)
{
	LocalScale = s;
	mLocalWorldUpdateFlag = true;
}

void World3D::SetLocalScale(const FLOAT s)
{
	SetLocalScale({ s,s });
}

void World3D::SetLocalRotation(const XMFLOAT3 s)
{
	LocalRotation = s;
	mLocalWorldUpdateFlag = true;
}

void World3D::SetLocalPosition(const XMFLOAT3 s)
{
	LocalPosition = s;
	mLocalWorldUpdateFlag = true;
}

void World3D::MoveLocalPosition(float x, float y, float z)
{
	if (x) LocalPosition.x += x;
	if (y) LocalPosition.y += y;
	if (z) LocalPosition.z += z;
	mLocalWorldUpdateFlag = true;
}

void World3D::SetUvScale(const XMFLOAT2 s)
{
	UvScale = s;
	mUvWorldUpdateFlag = true;
}

void World3D::SetUvRotation(const XMFLOAT3 s)
{
	UvRotation = s;
	mUvWorldUpdateFlag = true;
}

void World3D::SetUvPosition(const XMFLOAT2 s)
{
	UvPosition = s;
	mUvWorldUpdateFlag = true;
}

const XMFLOAT4X4& World3D::GetObjectWorld()
{
	UpdateObjectWorld();
	return mObjectWorld;
}

const XMFLOAT4X4& World3D::GetLocalWorld()
{
	UpdateLocalWorld();
	return mLocalWorld;
}

const XMFLOAT4X4& World3D::GetGlobalWorld()
{
	UpdateGlobalWorld();
	return mGlobalWorld;
}

const XMFLOAT4X4& World3D::GetTotalDrawWorld()
{
	UpdateTotalDrawWorld();
	return mTotalDrawWorld;
}

const XMFLOAT4X4& World3D::GetUvWorld()
{
	UpdateUvWorld();
	return mUvWorld;
}

void World3D::SetParentWorld(World3D* p)
{
	mParentWorld = p;
	mGlobalWorldUpdateFlag = true;
}

void World3D::SetParentDrawWorld()
{
	SetParentWorld(&App->GetDrawWorld3D(GetAlignX(), GetAlignY()));
}

void World3D::OnParentWorldUpdate()
{
	mGlobalWorldUpdateFlag = true;
}

void World3D::SetAlignX(AlignModeX x)
{
	alignX = x;
}

void World3D::SetAlignY(AlignModeY y)
{
	alignY = y;
}

void World3D::UpdateObjectWorld()
{
	if (mObjectWorldUpdateFlag)
	{
		XMMATRIX CenterPosTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&CenterPosition));
		XMMATRIX S = XMMatrixScalingFromVector(XMLoadFloat2(&ObjectScale));
		XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&ObjectRotation));
		XMMATRIX T = XMMatrixTranslationFromVector(XMLoadFloat3(&ObjectPosition));
		XMStoreFloat4x4(&mObjectWorld, CenterPosTranslation * S * R * T);
		mTotalDrawWorldUpdateFlag = true;

		mObjectWorldUpdateFlag = false;
	}
}

void World3D::UpdateLocalWorld()
{
	if (mLocalWorldUpdateFlag)
	{
		XMMATRIX S = XMMatrixScalingFromVector(XMLoadFloat2(&LocalScale));
		XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&LocalRotation));
		XMMATRIX T = XMMatrixTranslationFromVector(XMLoadFloat3(&LocalPosition));

		XMStoreFloat4x4(&mLocalWorld, S * R * T);
		mGlobalWorldUpdateFlag = true;

		mLocalWorldUpdateFlag = false;
	}
}

void World3D::UpdateGlobalWorld()
{
	UpdateLocalWorld();
	if (mGlobalWorldUpdateFlag)
	{
		if (mParentWorld == nullptr) mGlobalWorld = mLocalWorld;
		else XMStoreFloat4x4(&mGlobalWorld, XMLoadFloat4x4(&mLocalWorld) * XMLoadFloat4x4(&mParentWorld->GetGlobalWorld()));
		mTotalDrawWorldUpdateFlag = true;

		mGlobalWorldUpdateFlag = false;
	}
}

void World3D::UpdateTotalDrawWorld()
{
	UpdateObjectWorld();
	UpdateGlobalWorld();
	if (mTotalDrawWorldUpdateFlag)
	{
		XMStoreFloat4x4(&mTotalDrawWorld, XMLoadFloat4x4(&mObjectWorld) * XMLoadFloat4x4(&mGlobalWorld));
		mTotalDrawWorldUpdateFlag = false;
	}
}

void World3D::UpdateUvWorld()
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

