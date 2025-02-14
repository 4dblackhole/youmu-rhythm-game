#include "framework.h"
#include "DwLayout2D.h"

DwLayout2D::DwLayout2D(float size, D2D1::ColorF color, D2D1_POINT_2F pos)
	: Shape2D(),
	Color(color), layout(nullptr)
{
	GetWorld2d().SetScale(size);
	GetWorld2d().SetPosition(pos);
}

DwLayout2D::~DwLayout2D()
{
	ReleaseCOM(this->layout);
}

void DwLayout2D::SetLayout(const std::wstring& txt, IDWriteTextFormat* format)
{
	SetText(txt);
	SetTextFormat(format);
}

void DwLayout2D::SetLayoutRightAlign(const std::wstring& text, IDWriteTextFormat* textFormat)
{
	SetLayout(text, textFormat);
	MoveLayoutForRightAlign(text, textFormat);
}

void DwLayout2D::GetLayoutMetrics(const std::wstring& text, IDWriteTextFormat* textFormat, DWRITE_TEXT_METRICS* out)
{
	IDWriteTextLayout* tempLayout = nullptr;
	HR(D2D.GetDwFactory()->CreateTextLayout(text.c_str(), (UINT32)text.length(), textFormat, 10000.0f, 0.0f, &tempLayout));
	tempLayout->GetMetrics(out);
	ReleaseCOM(tempLayout);
}

void DwLayout2D::MoveLayoutForRightAlign(const std::wstring& text, IDWriteTextFormat* textFormat)
{
	//Get Total Width and Height
	DWRITE_TEXT_METRICS mt;
	if (textFormat == nullptr) textFormat = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	GetLayoutMetrics(text, textFormat, &mt);

	const float& sizeRate = GetWorld2d().GetScale().x;
	GetWorld2d().SetPosition({ GetWorld2d().GetPosition().x - (int)(mt.width * sizeRate),GetWorld2d().GetPosition().y });

}

void DwLayout2D::SetText(const wstring& str)
{
	text = str;
	textUpdateFlag = true;
}

void DwLayout2D::SetTextFormat(IDWriteTextFormat* ptr)
{
	if (ptr == nullptr)ptr = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	textFormat = ptr;
	textUpdateFlag = true;
}

void DwLayout2D::Draw()
{
	__super::Draw();
	UpdateLayout();
	DrawTextLayout({ 0,0 }, layout, D2D.GetSolidBrush(Color), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void DwLayout2D::UpdateLayout()
{
	if (textUpdateFlag)
	{
		ReleaseCOM(this->layout);
		HR(D2D.GetDwFactory()->CreateTextLayout(text.c_str(), (UINT32)text.length(), textFormat, maxW, maxH, &this->layout));
		textUpdateFlag = false;
	}
}

void DwLayout2D::DrawTextLayout(D2D1_POINT_2F origin, IDWriteTextLayout* textLayout, ID2D1Brush* defaultFillBrush, D2D1_DRAW_TEXT_OPTIONS options) noexcept
{
	D2D.GetRenderTarget()->DrawTextLayout(origin, textLayout, defaultFillBrush, options);
}
