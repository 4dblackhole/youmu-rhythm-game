#pragma once

#ifndef DECLARE_VARIABLE_STRING
#define DECLARE_VARIABLE_STRING(x) static constexpr LPCSTR x = #x
#endif


#if defined(DEBUG) | defined(_DEBUG)
#define DEBUG_BREAKPOINT int asdf__ASDfdsf11 = 3
#else
#define DEBUG_BREAKPOINT
#endif


//all coordinates are based on these numbers.
constexpr UINT StandardWidth = 1280;
constexpr UINT StandardHeight = 720;

namespace MyColorF
{
	extern const D2D1::ColorF BlackHalfAlpha;
	extern const D2D1::ColorF WhiteHalfAlpha;
	extern const D2D1::ColorF GhostGreen;
	extern const D2D1::ColorF CherryPink;
}
namespace MyColor4
{
	constexpr DirectX::XMFLOAT4 Blank(0, 0, 0, 0);
	constexpr DirectX::XMFLOAT4 Black(0, 0, 0, 1);
	constexpr DirectX::XMFLOAT4 White(1, 1, 1, 1);
	constexpr DirectX::XMFLOAT4 MyRed(1, 56.0f / 255.0f, 40.0f / 255.0f, 1);
	constexpr DirectX::XMFLOAT4 MyBlue(64.0f / 255.0f, 152.0f / 255.0f, 1, 1);
	constexpr DirectX::XMFLOAT4 GhostGreen(48.0f / 255.0f, 224.0f / 255.0f, 104.0f / 255.0f, 1);
}

constexpr DirectX::XMFLOAT4 DefaultBgColor = MyColor4::Black;

extern const wstring SkinDir;
extern const wstring SongDir;
extern const wstring MusicDir;
extern const wstring PatternDir;
extern const wstring TextureDir;
extern const wstring SystemSoundDir;

constexpr LPCWSTR ExtMusic = L".ymm";
constexpr LPCWSTR ExtPattern = L".ymp";

constexpr chrono::milliseconds BPM1oneBeat(60000);
constexpr chrono::milliseconds BPM1constantMeasure(BPM1oneBeat * 4);

constexpr int LaneMaxLength = 10000;

constexpr XMFLOAT4X4 XmFloatT4X4Identity
{
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};