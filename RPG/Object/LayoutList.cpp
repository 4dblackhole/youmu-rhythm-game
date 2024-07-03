#include <dxgidebug.h>
#include <DxErr.h>

#include "System/DirectWrite.h"
#include "Utility/ShortCut.h"
#include "LayoutList.h"

using namespace std;

LayoutList::LayoutList()
{
}

LayoutList::~LayoutList()
{
	for (std::pair<const UINT, IDWriteTextLayout*>& it : layoutList) ReleaseCOM(it.second);
}

void LayoutList::CreateLayout()
{
	IDWriteTextLayout* tempLayout;
	HR(DIRECTWRITE.GetDwFactory()->CreateTextLayout(L"ASDF", 4, DIRECTWRITE.GetFont(DirectWrite::FontName::DefaultFont), 100, 20, &tempLayout));
	layoutList.insert(make_pair(0, tempLayout));
}

IDWriteTextLayout* LayoutList::GetLayout(const UINT key)
{
	if (layoutList.find(key) != layoutList.end()) return layoutList[key];
	else
	{
		MessageBox(NULL, _T("layout not found"), _T("Error"), MB_OK);
		return nullptr;
	}
}
