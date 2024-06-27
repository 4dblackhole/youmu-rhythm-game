#pragma once

class Camera
{
public:
	Camera(int width, int height);
	~Camera();

	inline const XMMATRIX View() const { return XMLoadFloat4x4(&mView); }
	inline const XMMATRIX Proj() const { return XMLoadFloat4x4(&mProj); }
	inline const XMMATRIX ViewProj() const { return XMMatrixMultiply(View(), Proj()); }
	inline const XMFLOAT4X4 View4x4() const { return mView; }
	inline const XMFLOAT4X4 Proj4x4() const { return mProj; }
	inline const XMFLOAT4X4 ViewProj4x4() const { 
		XMFLOAT4X4 vp4x4;
		XMStoreFloat4x4(&vp4x4, ViewProj());
		return vp4x4;
	}

	void UpdateView();
	void UpdateProj(float w, float h, float zn = 0.1f, float zf = 1000.0f);

private:
	XMFLOAT3 mPosition;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
};
