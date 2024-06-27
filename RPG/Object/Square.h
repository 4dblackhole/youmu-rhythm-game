#pragma once

class Camera;

class Square
{
public:
	Square();
	Square(float w, float h);
	Square(float x, float y, float w, float h);
	~Square();

	void SetTexture(ID3D11ShaderResourceView* ptr) { textureSRV = ptr; }
	void Render(ID3D11DeviceContext*, const Camera&) const;

	static void BulidBuffer(ID3D11Device*);
	static void BuildLayout(ID3D11Device*);


private:
	void Init(float x, float y, float w, float h);

	float x, y, w, h;

	XMFLOAT2 Scale;
	XMFLOAT3 Rotation;
	XMFLOAT3 Position;

	XMFLOAT4X4 mWorld;
	
	ID3D11ShaderResourceView* textureSRV; //weak reference

	static ComPtr<ID3D11Buffer> mVB;
	static ComPtr<ID3D11Buffer> mIB;
	static ComPtr<ID3D11InputLayout> mInputLayout;

};

