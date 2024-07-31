#include "framework.h"
#include "Shape2D.h"

using namespace D2D1;

Shape2D::Shape2D(bool isChild)
{
	if (isChild) world2d = new ChildWorld2D;
	else world2d = new World2D;
}

Shape2D::~Shape2D()
{
	delete world2d;
}

void Shape2D::Draw()
{
	D2D.GetRenderTarget()->SetTransform(world2d->GetGlobalWorld());
}

void Shape2D::Reposition(float newW, float newH)
{
	world2d->Reposition(newW, newH);
}

void Shape2D::Resize(float newW, float newH)
{
	world2d->Resize(newW, newH);
}