#pragma once
#include <Windowsx.h>

#include <dwrite.h>
#include <map>
class LayoutList
{
public:
	LayoutList();
	~LayoutList();

	void CreateLayout();
	IDWriteTextLayout* GetLayout(const UINT key);
private:

	std::map<const UINT, IDWriteTextLayout*> layoutList;
};