#include "framework.h"
#include "defines.h"

const D2D1::ColorF MyColorF::GhostGreen(48.0f / 255.0f, 224.0f / 255.0f, 104.0f / 255.0f, 1);
const D2D1::ColorF MyColorF::CherryPink(224.0f / 255.0f, 104.0f / 255.0f, 224.0f / 255.0f, 1);
const D2D1::ColorF MyColorF::BlackHalfAlpha(0.0f, 0.0f, 0.0f, 0.5f);
const D2D1::ColorF MyColorF::WhiteHalfAlpha(1.0f, 1.0f, 1.0f, 0.5f);

const wstring FileDir = L"./Files/";
const wstring SkinDir = FileDir + L"Skins/";
const wstring SongDir = FileDir + L"Songs/";
const wstring MusicDir = SongDir + L"Music/";
const wstring PatternDir = SongDir + L"Pattern/";
const wstring TextureDir = L"../Assets/Textures/";
const wstring SystemSoundDir = L"../Assets/Hitsounds/System/";