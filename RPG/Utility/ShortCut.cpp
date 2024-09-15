#include "framework.h"
#include "ShortCut.h"

using namespace std;

wstring ShortCut::UTF8ToWstring(const string& buf)
{
    wstring uni;
    int nLen = MultiByteToWideChar(CP_UTF8, 0, buf.c_str(), (int)buf.length(), NULL, NULL);
    uni.resize((size_t)nLen);
    MultiByteToWideChar(CP_UTF8, 0, buf.c_str(), (int)buf.length(), &uni[0], nLen);

    return uni;
}

string ShortCut::WstringToUTF8(const wstring& buf)
{
    int nLen = WideCharToMultiByte(CP_UTF8, 0, buf.c_str(), (int)buf.length(), NULL, 0, NULL, NULL);
    string strUtf8;
    strUtf8.resize((size_t)nLen);
    WideCharToMultiByte(CP_UTF8, 0, buf.c_str(), (int)buf.length(), &strUtf8[0], nLen, NULL, NULL);

    return strUtf8;
}

std::wstring ShortCut::ReadUTF8File(const std::wstring& fileName)
{
	std::ifstream fin(fileName, std::ios::binary);
	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	string utf8Str;
	utf8Str.resize(size);
	fin.read(&utf8Str[0], size);
	fin.close();
	return UTF8ToWstring(utf8Str);

}

void ShortCut::TraceTimingPoint()
{
    INT64 t;
    QueryPerformanceCounter((LARGE_INTEGER*)&t);
    TRACE(_T("%lfms\n"), (float)((float)(t % 10000000) / 100) * 0.01f);
}

//return internal width
//in case that the current dimensions of window are 1280x360, the actual dimensions are 2560x720,
//because StandardHeight is 720.
float ShortCut::GetOrthoWidth(float w, float h)
{
    return (float)w * ((float)StandardHeight / (float)h);
}

/*
pos to the pos to be drawed
ex) pt(0,300) RightAlign, 1200x360 => pt(1200,120) 
*/
D2D1_POINT_2F ShortCut::Resize2DtoStandardCS(const float newW, const float newH, float x, float y, const float alignPosX)
{
    const float rateY = newH / (float)StandardHeight;
    const float DrawX = alignPosX + (float)x * rateY;
    const float DrawY = (float)y * rateY;
    return { DrawX, DrawY };
}


void ShortCut::GetFileList(vector<wstring>& vList, const wstring& sPath, const wstring& ext, bool bAllDirectories)
{
	wstring sTmp = sPath + L"*";

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(sTmp.c_str(), &fd);
	if (hFind == INVALID_HANDLE_VALUE) return;

	do
	{
		//incase Directory
		if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if (bAllDirectories && fd.cFileName[0] != L'.') //excludes relative directories(./ and ../)
			{
				sTmp = sPath + fd.cFileName + L"/";
				GetFileList(vList, sTmp, ext, bAllDirectories);
			}
		}
		else
		{
			wstring fileName = fd.cFileName;
			if (fileName.substr(fileName.length() - ext.length()) == ext)
			{
				sTmp = sPath + fd.cFileName;
				vList.push_back(sTmp);
			}
		}
	} while (FindNextFile(hFind, &fd));

	FindClose(hFind);
}

bool ShortCut::WordSeparateA(const string_view& source, const string& separator, string* first, string* second)
{
	size_t separatorPos = source.find(separator); // key {separator} filename
	if (separatorPos == string::npos) return false; //not valid desc

	if (first != nullptr)*first = source.substr(0, separatorPos);

	if (second != nullptr)
	{
		size_t secondStartPos = separatorPos + separator.length() - 1;
		while (source[++secondStartPos] == ' ');
		*second = source.substr(secondStartPos);
	}
	return true;
}
bool ShortCut::WordSeparateW(const wstring_view& source, const wstring& separator, wstring* first, wstring* second)
{
	size_t separatorPos = source.find(separator); // key {separator} filename
	if (separatorPos == wstring::npos) return false; //not valid desc

	if (first != nullptr)*first = source.substr(0, separatorPos);

	if (second != nullptr)
	{
		size_t secondStartPos = separatorPos + separator.length() - 1;
		while (source[++secondStartPos] == L' ');
		*second = source.substr(secondStartPos);
	}
	return true;
}

void ShortCut::WordSeparateW(const wstring_view& source, const wstring& separator, vector<pair<size_t, size_t>>& idxList)
{
	const size_t separatorLength = separator.length();
	size_t startPos = 0;
	while(true)
	{
		size_t separatorPos = source.find(separator, startPos); // key {separator} filename
		idxList.emplace_back(make_pair(startPos, separatorPos));
		if (separatorPos == wstring::npos) break;
		startPos = separatorPos + separatorLength - 1;
		while (source[++startPos] == L' ');
	}
}

D3D11_TEXTURE2D_DESC ShortCut::GetDescFromSRV(ID3D11ShaderResourceView* ptr)
{
	ID3D11Resource* pResource = nullptr;
	ptr->GetResource(&pResource);

	ID3D11Texture2D* pTexture2D = nullptr;
	pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture2D);

	D3D11_TEXTURE2D_DESC desc;
	pTexture2D->GetDesc(&desc);

	ReleaseCOM(pTexture2D);
	ReleaseCOM(pResource);

	return desc;
}

D2D1::Matrix3x2F ShortCut::XmFloat4x4To3x2(const XMFLOAT4X4 m)
{
	return D2D1::Matrix3x2F(m._11, m._12, m._21, m._22, m._41, m._42);
}

D2D1::Matrix3x2F ShortCut::WVP3Dto2D(const XMFLOAT4X4 m, float width, float height)
{
	const float halfW = width * 0.5f;
	const float halfH = height * 0.5f;
	return D2D1::Matrix3x2F(
		m._11 * halfW, -m._12 * halfH,
		m._21 * halfW, -m._22 * halfH,
		(m._41 + 1.0f) * halfW, (1.0f - m._42) * halfH);
}
