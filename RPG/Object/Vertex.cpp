#include "framework.h"
#include "Vertex.h"

Vertex::Vertex() : Pos({ 0.0f,0.0f,0.0f }) {}

Vertex::Vertex(const Vertex& p) : Pos(p.Pos) {}
Vertex::Vertex(Vertex&& p) noexcept : Pos(p.Pos) {}

Vertex& Vertex::operator=(const Vertex& p)
{
	this->Pos = p.Pos;
	return *this;
}

Vertex& Vertex::operator=(Vertex&& p) noexcept
{
	this->Pos = p.Pos;
	return *this;
}

Vertex::Vertex(const XMFLOAT3& p) : Pos(p) {}
Vertex::Vertex(XMFLOAT3&& p) noexcept : Pos(p)  {}

Vertex& Vertex::operator=(const XMFLOAT3&p)
{
	this->Pos = p;
	return *this;
}

Vertex& Vertex::operator=(XMFLOAT3&&p) noexcept
{
	this->Pos = p;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////

VertexColor::VertexColor() : Vertex(), Color(0.0f, 0.0f, 0.0f, 1.0f) {}
VertexColor::VertexColor(const VertexColor& p) : Vertex(p.Pos), Color(p.Color) {}
VertexColor::VertexColor(VertexColor&& p) noexcept : Vertex(p.Pos), Color(p.Color) {}

VertexColor& VertexColor::operator=(const VertexColor& p)
{
	this->Pos = p.Pos;
	this->Color = p.Color;
	return *this;
}

VertexColor& VertexColor::operator=(VertexColor&& p) noexcept
{
	this->Pos = p.Pos;
	this->Color = p.Color;
	return *this;
}

VertexColor::VertexColor(const XMFLOAT3& p, const XMFLOAT4& c) : Vertex(p), Color(c) {}
VertexColor::VertexColor(XMFLOAT3&& p, XMFLOAT4&& c) noexcept : Vertex(p), Color(c) {}

///////////////////////////////////////////////////////////////////////////////

VertexTexture::VertexTexture() : Vertex(), Uv(0.0f, 0.0f) {}
VertexTexture::VertexTexture(const VertexTexture& p) : Vertex(p.Pos), Uv(p.Uv) {}
VertexTexture::VertexTexture(VertexTexture&& p) noexcept : Vertex(p.Pos), Uv(p.Uv) {}

VertexTexture& VertexTexture::operator=(const VertexTexture& p)
{
	this->Pos = p.Pos;
	this->Uv = p.Uv;
	return *this;
}

VertexTexture& VertexTexture::operator=(VertexTexture&& p) noexcept
{
	this->Pos = p.Pos;
	this->Uv = p.Uv;
	return *this;
}

VertexTexture::VertexTexture(const XMFLOAT3& p, const XMFLOAT2& c) : Vertex(p), Uv(c) {}
VertexTexture::VertexTexture(XMFLOAT3&& p, XMFLOAT2&& c) noexcept : Vertex(p), Uv(c) {}