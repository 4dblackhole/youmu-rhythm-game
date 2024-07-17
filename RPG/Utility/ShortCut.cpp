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