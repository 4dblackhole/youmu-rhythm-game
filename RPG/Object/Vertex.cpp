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

VertexColor::VertexColor() : Pos(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f, 1.0f) {}
VertexColor::VertexColor(const VertexColor& p) : Pos(p.Pos), Color(p.Color) {}
VertexColor::VertexColor(VertexColor&& p) noexcept : Pos(p.Pos), Color(p.Color) {}

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

VertexColor::VertexColor(const XMFLOAT3& p, const XMFLOAT4& c) : Pos(p), Color(c) {}
VertexColor::VertexColor(XMFLOAT3&& p, XMFLOAT4&& c) noexcept : Pos(p), Color(c) {}

///////////////////////////////////////////////////////////////////////////////

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