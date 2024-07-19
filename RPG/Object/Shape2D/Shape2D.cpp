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
	world2d.UpdateWorld();
	if(world2d.GetParentWorld()==nullptr) D2D.GetRenderTarget()->SetTransform(world2d.GetWorld());
	else D2D.GetRenderTarget()->SetTransform(world2d.GetWorld() * (*world2d.GetParentWorld()));
}

void Shape2D::Reposition(float newW, float newH)
{
	world2d.Reposition(newW, newH);
}

void Shape2D::Resize(float newW, float newH)
{
	world2d.Resize(newW, newH);
}