#include "framework.h"
#include "Shape2D.h"

using namespace D2D1;

Shape2D::Shape2D()
{
}

Shape2D::~Shape2D()
{
}

void Shape2D::Draw()
{
	D2D.GetRenderTarget()->SetTransform(world2d.GetGlobalWorld());
}

void Shape2D::OnResize()
{
	GetWorld2d().OnParentWorldUpdate();
}
