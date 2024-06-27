#include "framework.h"
#include "Camera.h"

Camera::Camera(int width, int height)
	: mPosition(0.0f, 0.0f, -1.0f)
{
	UpdateView();
	UpdateProj((float)width, (float)height);
}

Camera::~Camera()
{
}

void Camera::UpdateView()
{
	XMVECTOR pos = XMVectorSet(mPosition.x, mPosition.y, mPosition.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = DirectX::XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void Camera::UpdateProj(float w, float h, float zn, float zf)
{
	XMMATRIX P = DirectX::XMMatrixOrthographicLH(w, h, zn, zf);
	XMStoreFloat4x4(&mProj, P);
}