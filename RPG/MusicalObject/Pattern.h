#pragma once
#include "framework.h"
#include "Note.h"

class Pattern
{
public:
	Pattern();
	~Pattern();

	wstring ympFileName;
	wstring ymmRefFileName;
	std::vector<std::wstring> MakerNameList;
	std::set<std::wstring> TagList;
	wstring DifficultyName;

};