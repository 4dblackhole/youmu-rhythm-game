#include "framework.h"
#include "World2D.h"
#include "World3D.h"

World3D::World3D(XMFLOAT3 size, XMFLOAT3 rot, XMFLOAT3 position)
{
}

World3D::~World3D()
{
}

void World3D::SetScale(const XMFLOAT3)
{
}

void World3D::SetScale(const FLOAT)
{
}

void World3D::SetRotation(const XMFLOAT3)
{
}

void World3D::SetPosition(const XMFLOAT3)
{
}

const XMFLOAT4X4& World3D::GetLocalWorld()
{
	return XmFloatT4X4Identity;
}

const XMFLOAT4X4& World3D::GetGlobalWorld()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return XmFloatT4X4Identity;
}

void World3D::SetParentWorld(const XMFLOAT4X4* p)
{
}

void World3D::ParentWorldUpdate()
{
}

void World3D::SetAlignX(AlignModeX)
{
}

void World3D::UpdateLocalWorld()
{
}

void World3D::UpdateGlobalWorld()
{
}
