#pragma once
#include <Windows.h>
#include <unordered_map>
#include <string>

#include "Singleton/SingletonBase.h"

class StrToVK : public SingletonBase<StrToVK>
{
public:
	StrToVK();
	~StrToVK();

	static ::byte GetVK(const std::string& str);

private:
	typedef std::unordered_map<std::string, const ::byte>				vKeyMap;
	typedef std::unordered_map<std::string, const ::byte>::iterator	vKeyMapIter;
	vKeyMap vkMap;

	::byte Convert(const std::string& str);
};

