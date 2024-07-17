#include "framework.h"
#include "MusicScroll.h"

constexpr int MusicScrollX = -10;
constexpr int MusicScrollY = -10;
constexpr float MusicScrollWidth = 450.0f;
constexpr float MusicScrollHeight = 670.0f;

constexpr float NoMusicTextSize = 40.0f;
constexpr int NoMusicTextX = MusicScrollX - (int)MusicScrollWidth;
constexpr int NoMusicTextY = -MusicScrollY + 60; // each Y axis of 3D and 2D is reverse

constexpr int BoxHeight = 60;
constexpr int BoxOffsetY = -(int)MusicScrollY + 30;
constexpr int BoxEdgeX = 5;
constexpr int BoxEdgeY = 5;

MusicScroll::MusicScroll()
	: box({ (MusicScrollX - MusicScrollWidth + BoxEdgeX), (float)BoxOffsetY,
		(float)MusicScrollX - BoxEdgeX, float(BoxOffsetY + BoxHeight) }),
	scrollImg(MusicScrollX, MusicScrollY, MusicScrollWidth, MusicScrollHeight)
{
	InitMusicScroll();
}

MusicScroll::~MusicScroll()
{
	for (Music*& it : musicList) delete it;
}

void MusicScroll::OnResize(float newW, float newH)
{
	scrollImg.OnResize();
	noMusicText->Resize(newW, newH);
	box.Resize(newW, newH);
}

void MusicScroll::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	scrollImg.Render(deviceContext, cam);
	if (musicList.empty()) noMusicText->Draw();
	else box.Draw();
}

const wstring musicFileIdc = L"File";
const wstring musicNameIdc = L"Music Name";
const wstring artistIdc = L"Artist";
const wstring tagIdc = L"Tags";
const wstring endLineIdc = L"\r\n";

static bool ColumnSeparate(const wstring& source, wstring* first, wstring* second)
{
	size_t columnPos = source.find(L':'); // key: filename
	if (columnPos == wstring::npos) return false; //not valid desc

	if (first!=nullptr)*first = source.substr(0, columnPos);

	if (second!=nullptr)
	{
		size_t secondStartPos = columnPos;
		while (source[++secondStartPos] == L' ');
		*second = source.substr(secondStartPos);
	}
	return true;
}

static Music* ParseYmmFile(const wstring& file)
{
	Music* resultMusic = new Music;
	try
	{
		// pos of "File: xxx;
		size_t filePos = file.find(musicFileIdc);
		if (filePos == wstring::npos) throw nullptr;

		size_t endPos = file.find(endLineIdc);

		wstring lineStr = file.substr(filePos, endPos - filePos);
		ColumnSeparate(lineStr, nullptr, &resultMusic->MusicName);

		// pos of "Music Name"
		size_t musicNamePos = file.find(musicNameIdc, endPos);
		if (musicNamePos == wstring::npos) throw nullptr;
		endPos = file.find(endLineIdc, musicNamePos);

		lineStr = file.substr(musicNamePos, endPos - musicNamePos);
		wstring count;
		ColumnSeparate(lineStr, nullptr, &count);

		//get counts
		wstringstream wss;
		size_t musicNameCount;
		wss << count << endl;
		wss >> musicNameCount;

		resultMusic->MusicNameList.reserve(musicNameCount);
		for (size_t idx = 0; idx < musicNameCount; ++idx)
		{
			musicNamePos = file.find(musicNameIdc, endPos);
			if (musicNamePos == wstring::npos) break;
			endPos = file.find(endLineIdc, musicNamePos);
			lineStr = file.substr(musicNamePos, endPos - musicNamePos);
			wstring tempMusicName;
			ColumnSeparate(lineStr, nullptr, &tempMusicName);
			resultMusic->MusicNameList.emplace_back(tempMusicName);
		}

		// pos of "Artist"
		size_t artistPos = file.find(artistIdc, endPos);
		if (artistPos == wstring::npos) throw nullptr;
		endPos = file.find(endLineIdc, artistPos);

		lineStr = file.substr(artistPos, endPos - artistPos);
		ColumnSeparate(lineStr, nullptr, &count);
		//get counts
		size_t artistCount;
		wss << count;
		wss >> artistCount;

		resultMusic->ArtistList.reserve(artistCount);
		for (size_t idx = 0; idx < artistCount; ++idx)
		{
			artistPos = file.find(artistIdc, endPos);
			if (artistPos == wstring::npos) break;
			endPos = file.find(endLineIdc, artistPos);
			lineStr = file.substr(artistPos, endPos - artistPos);
			wstring tempArtistName;
			ColumnSeparate(lineStr, nullptr, &tempArtistName);
			resultMusic->ArtistList.emplace_back(tempArtistName);
		}

		// pos of "Tags"
		size_t tagPos = file.find(tagIdc, endPos);
		if (tagPos == wstring::npos) throw nullptr;
		endPos = file.find(endLineIdc, tagPos);

		lineStr = file.substr(tagPos, endPos - tagPos);
		wstring tagList;
		ColumnSeparate(lineStr, nullptr, &tagList); //get whitespace separated wstring

		size_t tagStartPos = 0;
		while (true)
		{
			size_t whitespacePos = tagList.find(L' ', tagStartPos);
			wstring tag = tagList.substr(tagStartPos, whitespacePos - tagStartPos);
			if (tag.length() > 0)resultMusic->TagList.emplace(tag);
			tagStartPos = whitespacePos;
			if (tagStartPos != wstring::npos) while (tagList[++tagStartPos] == L' ');
			else break;
		}

	}
	catch (void* p)
	{
		p = p;
		delete resultMusic;
		return nullptr;
	}

	return resultMusic;

}

void MusicScroll::LoadMusic()
{
	vector<wstring> ymmList;
	ymmList.reserve(128);

	ShortCut::GetFileList(ymmList, MusicDir, ExtMusic);

	musicList.clear();
	musicList.reserve(ymmList.size());

	for (const wstring& it : ymmList)
	{
		std::ifstream fin(it, std::ios::binary);

		fin.seekg(0, std::ios_base::end);
		int size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		string utf8Str;
		utf8Str.resize(size);
		fin.read(&utf8Str[0], size);

		wstring uniFile = ShortCut::UTF8ToWstring(utf8Str); //ymm file road complete
		musicList.emplace_back(ParseYmmFile(uniFile));

		fin.close();
	}
}

void MusicScroll::InitMusicScroll()
{
	scrollImg.SetAlignX(AlignModeX::Right);
	scrollImg.SetAlignY(AlignModeY::Top);
	scrollImg.SetTexture(GETTEXTURE(TextureManager::Name::MusicScroll));

	DWRITE_TEXT_METRICS mt;
	LPCWSTR tempstr = L"No Music";
	IDWriteTextFormat*& tempFormat = D2D.GetFont(D2Ddevice::FontName::DefaultFont);

	DwLayout::GetLayoutMetrics(tempstr, tempFormat, &mt);

	LayoutDesc tempDesc(NoMusicTextSize, MyColorF::GhostGreen, { (float)NoMusicTextX + (mt.width * NoMusicTextSize / D2Ddevice::DefaultFontSize),NoMusicTextY });

	tempDesc.maxW = MusicScrollWidth;
	tempDesc.world2d.alignX = AlignModeX::Right;
	noMusicText.reset(new DwLayout(tempDesc));
	noMusicText->SetLayoutRightAlign(tempstr, tempFormat);
	noMusicText->layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	box.IsRound = true;
	box.SetRadius(10.0f);
	box.BorderSize = 2.0f;
	box.BorderColor = MyColorF::GhostGreen;
	box.GetWorld2d().alignX = AlignModeX::Right;

	LoadMusic();
}
