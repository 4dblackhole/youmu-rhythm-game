#include "framework.h"
#include "MusicScroll.h"

constexpr int MusicScrollX = -10;
constexpr int MusicScrollY = -10;
constexpr float MusicScrollWidth = 450.0f;
constexpr float MusicScrollHeight = 670.0f;

constexpr float NoMusicTextSize = 40.0f;
constexpr int NoMusicTextX = MusicScrollX - (int)MusicScrollWidth;
constexpr int NoMusicTextY = -MusicScrollY + 60; // Y axis of 3D and 2D is reverse

constexpr float clipAreaOffset = 30.0f;
constexpr float clipAreaHeight = MusicScrollHeight - clipAreaOffset * 2;

constexpr float BoxHeight = 60.0f;
constexpr float BoxOffsetY = clipAreaOffset + 2.0f;
constexpr int BoxEdgeX = 5;
constexpr int BoxEdgeY = 5;
constexpr int BoxWidth = (int)MusicScrollWidth - BoxEdgeX * 2;

constexpr int TextEdgeX = 10;
constexpr int TextWidth = (int)MusicScrollWidth - TextEdgeX * 2;
constexpr int TextHeight = 40;
constexpr int TextAreaHeight = (int)clipAreaHeight;

MusicScroll::MusicScroll() : MusicScroll(MusicScrollX, MusicScrollY, MusicScrollWidth, MusicScrollHeight)
{
}

MusicScroll::MusicScroll(float x, float y, float w, float h) :
	scrollImg(x, y, w, h),
	clipArea(x, y - clipAreaOffset, w, clipAreaHeight),
	textArea(x - TextEdgeX, y - clipAreaOffset, TextWidth, TextAreaHeight)
{
	InitMusicScroll();
	clipArea.alignX = scrollImg.GetAlignX();
	clipArea.alignY = scrollImg.GetAlignY();

	textArea.alignX = clipArea.alignX;
	textArea.alignY = clipArea.alignY;

}

MusicScroll::~MusicScroll()
{
	for (auto*& it : musicList) delete it;
	for (auto*& it : musicBoxList) delete it;
	for (auto*& it : musicTextList) delete it;
}

void MusicScroll::OnBeginScene()
{
	PlayMusic();
}

void MusicScroll::OnEndScene()
{
	StopMusic();
}

void MusicScroll::PlayMusic()
{
	if (!musicList.empty()) FmodSystem::GetInstance().System()->playSound(musicList[currentSelectMusic]->music, nullptr, false, &musicList[currentSelectMusic]->channel);
}

void MusicScroll::StopMusic()
{
	if (!musicList.empty()) FmodSystem::GetInstance().System()->playSound(musicList[currentSelectMusic]->music, nullptr, true, &musicList[currentSelectMusic]->channel);
}

void MusicScroll::World2DResize(float newW, float newH)
{
	noMusicText->Resize(newW, newH);
	for (auto*& it : musicBoxList) it->Resize(newW, newH);
}

void MusicScroll::OnResize(float newW, float newH)
{
	UpdateScroll();
	scrollImg.OnResize();
	clipArea.Resize();
	textArea.Resize();
	World2DResize(newW, newH);
}

void MusicScroll::OnMouseWheel(WPARAM wState, int x, int y)
{
	int zDelta = GET_WHEEL_DELTA_WPARAM(wState);
	if (zDelta < 0) //wheel down
	{
		if (scrollPos != -10)--scrollPos;
	}
	else //wheel up
	{
		if (scrollPos != 10)++scrollPos;
	}
	UpdateScroll();
}

void MusicScroll::ChangePreviewMusic()
{
	Music*& prev = musicList[prevSelectMusic];
	Music*& newMusic = musicList[currentSelectMusic];
	FmodSystem::GetInstance().System()->playSound(prev->music, nullptr, true, &prev->channel);
	FmodSystem::GetInstance().System()->playSound(newMusic->music, nullptr, false, &newMusic->channel);
}

void MusicScroll::Update(float dt)
{
	
	if (KEYBOARD.Down(VK_UP))
	{
		if (currentSelectMusic > 0)
		{
			FMODSYSTEM.Play(FmodSystem::Name::button01a);
			prevSelectMusic = currentSelectMusic--;
			musicBoxList[prevSelectMusic]->BorderColor = MyColorF::GhostGreen;
			musicBoxList[currentSelectMusic]->BorderColor = MyColorF::CherryPink;
			ChangePreviewMusic();
		}
	}
	if (KEYBOARD.Down(VK_DOWN))
	{
		if (currentSelectMusic < musicList.size() - 1)
		{
			FMODSYSTEM.Play(FmodSystem::Name::button01a);
			prevSelectMusic = currentSelectMusic++;
			musicBoxList[prevSelectMusic]->BorderColor = MyColorF::GhostGreen;
			musicBoxList[currentSelectMusic]->BorderColor = MyColorF::CherryPink;
			ChangePreviewMusic();
		}
	}
}

void MusicScroll::UpdateScroll()
{
	const float rateY = (float)App->GetHeight() / (float)StandardHeight;
	scrollMatrix = D2D1::Matrix3x2F::Translation(0, (FLOAT)scrollPos * 20 * rateY);
	for (auto& boxWorld : musicBoxList) boxWorld->GetWorld2d().SetParentWorld(scrollMatrix);
	for (auto& boxWorld : musicBoxList) boxWorld->GetWorld2d().UpdateChildWorld();
}


void MusicScroll::CreateBoxes()
{
	musicBoxList.clear();
	for (size_t i = 0; i < musicList.size(); ++i)
	{
		const XMFLOAT2& scale = scrollImg.GetScale();
		const XMFLOAT3& pos = scrollImg.GetPosition();

		//Right-Top based position
		FLOAT boxLeft = pos.x - scale.x + BoxEdgeX;
		FLOAT boxRight = pos.x - BoxEdgeX;
		Rectangle2D* tempBox = new Rectangle2D({
			pos.x - scale.x + BoxEdgeX	,-pos.y + BoxOffsetY ,
			pos.x - BoxEdgeX			,-pos.y + BoxOffsetY + BoxHeight });
		tempBox->GetWorld2d().alignX = scrollImg.GetAlignX();
		tempBox->GetWorld2d().alignY = scrollImg.GetAlignY();
		tempBox->IsRound = true;
		tempBox->SetRadius(10.0f);
		tempBox->GetWorld2d().SetPosition({0 , (BoxHeight + BoxEdgeY) * i });
		tempBox->BorderSize = 2.0f;
		tempBox->BorderColor = MyColorF::GhostGreen;
		tempBox->GetWorld2d().SetParentWorld(scrollMatrix);
		tempBox->Resize((float)App->GetWidth(), (float)App->GetHeight());
		musicBoxList.emplace_back(tempBox);
	}
	
	if(!musicBoxList.empty())musicBoxList[0]->BorderColor = MyColorF::CherryPink;
}

void MusicScroll::CreateTexts()
{
	musicTextList.clear();

	for (size_t i = 0; i < musicBoxList.size(); ++i)
	{
		wstring musicDesc = L"Artist: " + musicList[i]->ArtistList[0] + L"\nTitle: " + musicList[i]->MusicNameList[0];
		IDWriteTextFormat*& tempFormat = D2D.GetFont(D2Ddevice::FontName::DefaultFont);

		DWRITE_TEXT_METRICS mt;
		DwLayout::GetLayoutMetrics(musicDesc, tempFormat, &mt);

		const float defaultSize = 1.5f;
		const float maximumLayoutWidth = TextWidth / defaultSize * 2;

		const float musicTextSize = defaultSize;// mt.width >= maximumLayoutWidth ? defaultSize * sqrtf(maximumLayoutWidth / mt.width) : defaultSize;
		const float textLeft = -BoxWidth +MusicScrollX;
		const float textTop = 40;

		LayoutDesc tempDesc(musicTextSize, MyColorF::GhostGreen, { textLeft , textTop });
		tempDesc.world2d.alignX = AlignModeX::Left;
		tempDesc.world2d.alignY = AlignModeY::Top;

		tempDesc.maxW = maximumLayoutWidth;
		tempDesc.maxH = TextHeight;
		DwLayout* tempLayout = new DwLayout(tempDesc);
		tempLayout->SetLayout(musicDesc, tempFormat);
		DWRITE_TRIMMING dwt{ DWRITE_TRIMMING_GRANULARITY_WORD,0,0};
		tempLayout->desc.world2d.SetParentWorld(musicBoxList[i]->GetWorld2d().GetGlobalWorld());
		musicBoxList[i]->GetWorld2d().childWorlds.emplace_back(&tempLayout->desc.world2d);
		musicTextList.emplace_back(tempLayout);

	}
}

void MusicScroll::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	scrollImg.Render(deviceContext, cam);

	D2D.GetRenderTarget()->SetTransform(D2D1::Matrix3x2F::Identity());
	D2D.GetRenderTarget()->PushAxisAlignedClip(clipArea.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	if (musicList.empty()) noMusicText->Draw();
	else
	{
		for (Rectangle2D*& it : musicBoxList) it->Draw();

		D2D.GetRenderTarget()->SetTransform(D2D1::Matrix3x2F::Identity());
		D2D.GetRenderTarget()->PushAxisAlignedClip(textArea.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		for (auto*& it : musicTextList) it->Draw();
		D2D.GetRenderTarget()->PopAxisAlignedClip();
	}

	D2D.GetRenderTarget()->PopAxisAlignedClip();
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

	if (first != nullptr)*first = source.substr(0, columnPos);

	if (second != nullptr)
	{
		size_t secondStartPos = columnPos;
		while (source[++secondStartPos] == L' ');
		*second = source.substr(secondStartPos);
	}
	return true;
}

static Music* ParseYmmFile(const wstring& fileDir, const wstring& file)
{
	Music* resultMusic = new Music;
	try
	{
		// pos of "File: xxx;
		size_t filePos = file.find(musicFileIdc);
		if (filePos == wstring::npos) throw nullptr;

		size_t endPos = file.find(endLineIdc);

		wstring lineStr = file.substr(filePos, endPos - filePos);
		ColumnSeparate(lineStr, nullptr, &resultMusic->FileName);

		const wstring musicDir = fileDir + L'/' + resultMusic->FileName;
		const string musicDirUTF8 = ShortCut::WstringToUTF8(musicDir);
		FMOD_RESULT result = FmodSystem::GetInstance().System()->createStream(musicDirUTF8.c_str(), FMOD_LOOP_OFF | FMOD_CREATESAMPLE, nullptr, &resultMusic->music);
		
		//get audio file size
		std::ifstream fin(musicDir, std::ios::binary);
		fin.seekg(0, std::ios_base::end);
		size_t size = (size_t)fin.tellg();
		resultMusic->musicFileSize = size;
		fin.close();

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
	catch (void* p) //ymm file error
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

		Music* music = ParseYmmFile(it.substr(0, it.find_last_of(L'/')), uniFile);

		if (music != nullptr)
		{
			music->ymmFileName = it;
			musicList.emplace_back(music);
		}

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

	LoadMusic();
	CreateBoxes();
	CreateTexts();
}
