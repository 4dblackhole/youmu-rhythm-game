#include "framework.h"
#include "MusicScroll.h"

constexpr int MusicScrollX = -10;
constexpr int MusicScrollY = -10;
constexpr int MusicScrollWidth = 450;
constexpr int MusicScrollHeight = 670;

constexpr int NoMusicTextSize = 40;
constexpr int NoMusicTextX = MusicScrollX - MusicScrollWidth;
constexpr int NoMusicTextY = -MusicScrollY + 60; // Y axis of 3D and 2D is reverse

//distance from scroll image
constexpr int clipAreaOffset = 30;
constexpr int clipAreaHeight = MusicScrollHeight - clipAreaOffset * 2;

constexpr int BoxHeight = 60;
constexpr int BoxOffsetY = clipAreaOffset + 2;
constexpr int BoxEdgeX = 5;
constexpr int BoxEdgeY = 5;
constexpr int BoxWidth = MusicScrollWidth - BoxEdgeX * 2;

constexpr int PBoxOffsetX = 15;
constexpr int PBoxWidth = BoxWidth - PBoxOffsetX * 2;
constexpr int PBoxHeight = 40;
constexpr int PBoxEdgeY = BoxEdgeY;

constexpr int TextEdgeX = 5;
constexpr int TextWidth = MusicScrollWidth - TextEdgeX * 2;
constexpr int TextHeight = 40;
constexpr int TextAreaHeight = clipAreaHeight;

MusicScroll::MusicScroll() : MusicScroll(MusicScrollX, MusicScrollY, MusicScrollWidth, MusicScrollHeight)
{
}

MusicScroll::MusicScroll(float x, float y, float w, float h) :
	scrollImg(x, y, w, h),
	clipArea(x + w * 0.5f, y - clipAreaOffset + h * 0.5f,
		w, h - clipAreaOffset * 2),
	textArea(x - (BoxEdgeX + TextEdgeX) + w * 0.5f, y - clipAreaOffset + h * 0.5f,
		w - (TextEdgeX + BoxEdgeX) * 2, h - clipAreaOffset * 2),
	patternBox({ -PBoxWidth / 2,-PBoxHeight / 2,PBoxWidth / 2,PBoxHeight / 2 })
{
	InitMusicScroll();

	//area to draw boxes
	clipArea.alignX = scrollImg.GetAlignX();
	clipArea.alignY = scrollImg.GetAlignY();

	//area to draw texts
	textArea.alignX = clipArea.alignX;
	textArea.alignY = clipArea.alignY;

}

MusicScroll::~MusicScroll()
{
	for (auto*& it : musicList)
	{
		for (auto*& patternIter : it->patternList)
		{
			delete patternIter;
		}
		delete it;
	}
	for (auto*& it : musicBoxList) delete it;
	for (auto*& it : musicTextList) delete it;
}

void MusicScroll::OnBeginScene()
{
	PlayMusic(currentSelectMusic);
}

void MusicScroll::OnEndScene()
{
	StopMusic(currentSelectMusic);
}

void MusicScroll::PlayMusic(size_t idx)
{
	if (!musicList.empty()) FmodSystem::GetInstance().System()->playSound(musicList[idx]->music, nullptr, false, &musicList[idx]->channel);
}

void MusicScroll::StopMusic(size_t idx)
{
	if (!musicList.empty()) FmodSystem::GetInstance().System()->playSound(musicList[idx]->music, nullptr, true, &musicList[idx]->channel);
}

void MusicScroll::World2DResize(float newW, float newH)
{
	noMusicText->Resize(newW, newH);

	/*	BoxWorld(Parent) ¦¨¦¡ TextWorld(Child)
	*					 ¦¦¦¡ PatternBox(Child)
	* 
	so once boxworld has changed then the child worlds will be changed automatically.	*/
	for (auto*& it : musicBoxList) it->Resize(newW, newH);
	for (auto*& it : musicTextList) it->Resize(newW, newH); // it->desc.world2d.UpdateGlobalWorld();

	patternBox.GetWorld2d().Resize(newW,newH);

	int asdf = 3;
}

void MusicScroll::UpdateScrollMatrix()
{
	const float scrollStartPosOffset = -scrollImg.GetLocalScale().y * 0.5f;
	const float scrollYpos = scrollStartPosOffset + scrollPos * 20.0f;

	scrollMatrix = D2D1::Matrix3x2F::Translation(0, scrollYpos);

	Music*& currentMusic = musicList[currentSelectMusic];
	if (!currentMusic->patternList.empty())
	{
		UpdatePatternHeightMatrix(currentSelectMusic);
	}
	else scrollWithPatternMatrix = scrollMatrix;

	for (auto& it : musicBoxList)
	{
		it->GetWorld2d().UpdateGlobalWorld();

	}
	for (auto& it : musicTextList)
	{
		it->desc.world2d.UpdateGlobalWorld();
	}
	patternBox.GetWorld2d().UpdateGlobalWorld();
}

void MusicScroll::OnResize(float newW, float newH)
{
	UpdateScrollMatrix();
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
		if (scrollPos != -40)scrollPos -= 1.0f;
	}
	else //wheel up
	{
		if (scrollPos != 40)scrollPos += 1.0f;
	}
	UpdateScrollMatrix();
}

void MusicScroll::ChangeSelectMusic(size_t newIdx)
{
	previousSelectMusic = currentSelectMusic;
	currentSelectMusic = newIdx;

	//bordercolor Change
	musicBoxList[previousSelectMusic]->BorderColor = MyColorF::GhostGreen;
	musicBoxList[currentSelectMusic]->BorderColor = MyColorF::CherryPink;

	//change preview music
	StopMusic(previousSelectMusic);
	PlayMusic(currentSelectMusic);

	//move the music boxes as height of pattern boxes
	UpdatePatternHeightMatrix(currentSelectMusic);
	ChangeTargetScrollMatrix();

	//default select pattern
	currentSelectPattern = 0;
}

void MusicScroll::Update(float dt)
{
	
	if (KEYBOARD.Down(VK_LEFT))
	{
		if (currentSelectMusic > 0)
		{
			FMODSYSTEM.Play(FmodSystem::Name::button01a);
			ChangeSelectMusic(currentSelectMusic - 1);
		}
	}
	if (KEYBOARD.Down(VK_RIGHT))
	{
		if (currentSelectMusic < musicList.size() - 1)
		{
			FMODSYSTEM.Play(FmodSystem::Name::button01a);
			ChangeSelectMusic(currentSelectMusic + 1);
		}
	}

	if (KEYBOARD.Down(VK_UP))
	{
		FMODSYSTEM.Play(FmodSystem::Name::button01a);
		ChangeSelectMusic(0);
	}

	if (KEYBOARD.Down(VK_DOWN))
	{

		FMODSYSTEM.Play(FmodSystem::Name::button01a);
		ChangeSelectMusic(2);
	}

}

void MusicScroll::UpdatePatternHeightMatrix(size_t musicIdx)
{
	Music*& currentMusic = musicList[musicIdx];
	const float patternHeight = float(musicList[currentSelectMusic]->patternList.size() * (size_t)(PBoxHeight + PBoxEdgeY));
	patternHeightMatrix = D2D1::Matrix3x2F::Translation(0, patternHeight);
	scrollWithPatternMatrix = patternHeightMatrix * scrollMatrix;
}

void MusicScroll::ChangeTargetScrollMatrix()
{
	size_t startPos = previousSelectMusic;
	size_t endPos = currentSelectMusic;
	bool changeToPatternMatrixFlag = false;
	if (startPos > endPos)
	{
		changeToPatternMatrixFlag = true;
		std::swap(startPos, endPos);
	}

	if (changeToPatternMatrixFlag)
	{
		for (size_t boxIdx = startPos + 1; boxIdx <= endPos; ++boxIdx)
		{
			musicBoxList[boxIdx]->GetWorld2d().SetParentWorld(&scrollWithPatternMatrix);
		}
	}
	else
	{
		for (size_t boxIdx = startPos; boxIdx <= endPos; ++boxIdx)
		{
			musicBoxList[boxIdx]->GetWorld2d().SetParentWorld(&scrollMatrix);
		}
	}

	for (auto& it : musicBoxList)
	{
		it->GetWorld2d().UpdateGlobalWorld();
	}
	for (auto& it : musicTextList)
	{
		it->desc.world2d.UpdateGlobalWorld();
	}
}

void MusicScroll::InitBoxListParentWorld()
{
	for (size_t boxIdx = 0; boxIdx < musicList.size(); ++boxIdx)
	{
		if (boxIdx > currentSelectMusic) musicBoxList[boxIdx]->GetWorld2d().SetParentWorld(&scrollWithPatternMatrix);
		else musicBoxList[boxIdx]->GetWorld2d().SetParentWorld(&scrollMatrix);
	}
	for (size_t textIdx = 0; textIdx < musicTextList.size(); ++textIdx)
	{
		musicTextList[textIdx]->desc.world2d.SetParentWorld(&musicBoxList[textIdx]->GetWorld2d().GetGlobalWorld());
	}
}

void MusicScroll::CreateMusicBoxes()
{
	musicBoxList.clear();
	for (size_t i = 0; i < musicList.size(); ++i)
	{
		// Box =======================================================================
		const XMFLOAT2& scale = scrollImg.GetLocalScale();
		const XMFLOAT3& pos = scrollImg.GetPosition();

		//Right-Top based position
		FLOAT boxLeft = pos.x - scale.x * 0.5f + BoxEdgeX;
		FLOAT boxRight = pos.x + scale.x * 0.5f - BoxEdgeX;
		FLOAT boxTop = -pos.y + BoxOffsetY;
		FLOAT boxBottom = -pos.y + BoxOffsetY + BoxHeight;

		Math::LTRB2CenterXYWH<FLOAT>(boxLeft, boxTop, boxRight, boxBottom);

		const FLOAT& boxCenterX = boxLeft;
		const FLOAT& boxCenterY = boxTop;
		const FLOAT& boxWidth = boxRight;
		const FLOAT& boxHeight = boxBottom;

		Rectangle2D* tempBox = new Rectangle2D({
			-boxWidth * 0.5f	 , -boxHeight * 0.5f ,
			boxWidth * 0.5f , boxHeight * 0.5f });
		tempBox->GetWorld2d().alignX = scrollImg.GetAlignX();
		tempBox->GetWorld2d().alignY = scrollImg.GetAlignY();
		tempBox->IsRound = true;
		tempBox->SetRadius(10.0f);
		tempBox->GetWorld2d().SetPosition({ boxCenterX , boxCenterY + (BoxHeight + BoxEdgeY) * i });
		tempBox->BorderSize = 2.0f;
		tempBox->BorderColor = MyColorF::GhostGreen;
		musicBoxList.emplace_back(tempBox);

		// Text ====================================================================
		{
			
			wstring musicDesc = L"Artist: " + musicList[i]->ArtistList[0] + L"\nTitle: " + musicList[i]->MusicNameList[0];
			IDWriteTextFormat*& tempFormat = D2D.GetFont(D2Ddevice::FontName::DefaultFont);

			DWRITE_TEXT_METRICS mt;
			DwLayout::GetLayoutMetrics(musicDesc, tempFormat, &mt);

			const float defaultSize = 1.5f;
			const float maximumLayoutWidth = TextWidth / defaultSize * 2;

			const float musicTextSize = defaultSize;// mt.width >= maximumLayoutWidth ? defaultSize * sqrtf(maximumLayoutWidth / mt.width) : defaultSize;
			const float textLeft = TextEdgeX - BoxWidth * 0.5f;
			const float textTop = -BoxHeight * 0.5f;

			LayoutDesc tempDesc(musicTextSize, MyColorF::GhostGreen, { 0, 0 });
			tempDesc.world2d.alignX = tempBox->GetWorld2d().alignX;
			tempDesc.world2d.alignY = tempBox->GetWorld2d().alignY;
			tempDesc.maxW = maximumLayoutWidth;
			tempDesc.maxH = TextHeight;
			tempDesc.world2d.SetPosition({ textLeft,textTop });
			tempDesc.world2d.SetScale(defaultSize);
			DwLayout* tempLayout = new DwLayout(tempDesc);
			tempLayout->SetLayout(musicDesc, tempFormat);
			musicTextList.emplace_back(tempLayout);
			
		}
	}

	//default select first music
	if (!musicBoxList.empty())
	{
		InitBoxListParentWorld();
		musicBoxList[currentSelectMusic]->BorderColor = MyColorF::CherryPink;
	}
}

void MusicScroll::CreatePatternBoxes()
{
	patternBox.GetWorld2d().alignX = scrollImg.GetAlignX();
	patternBox.GetWorld2d().alignY = scrollImg.GetAlignY();
	patternBox.GetWorld2d().SetPosition({ PBoxOffsetX,(BoxHeight + PBoxHeight )*0.5f + BoxEdgeY });
	patternBox.GetWorld2d().SetParentWorld( &musicBoxList[0]->GetWorld2d().GetGlobalWorld());
	patternBox.IsRound = true;
	patternBox.SetRadius(5.0f);
	patternBox.BorderSize = 1.5f;
	patternBox.BorderColor = MyColorF::GhostGreen;
	int asdf = 3;
}

void MusicScroll::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	scrollImg.Render(deviceContext, cam);

	D2D.GetRenderTarget()->SetTransform(D2D1::Matrix3x2F::Identity());
	//D2D.GetRenderTarget()->PushAxisAlignedClip(clipArea.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	if (musicList.empty()) noMusicText->Draw();
	else
	{
		for (auto& it : musicBoxList) it->Draw();

		D2D.GetRenderTarget()->SetTransform(D2D1::Matrix3x2F::Identity());
	//	D2D.GetRenderTarget()->PushAxisAlignedClip(textArea.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		for (auto*& it : musicTextList) it->Draw();
		//D2D.GetRenderTarget()->PopAxisAlignedClip();
	}
	//D2D.GetRenderTarget()->PopAxisAlignedClip();

	patternBox.Draw();
}

//ymm
const wstring musicFileIdc = L"File";
const wstring musicNameIdc = L"Music Name";
const wstring artistIdc = L"Artist";

//ymp
const wstring metadataIdc = L"[Metadata]";
const wstring timeSignatureIdc = L"[Time Signature]";
const wstring pattermIdc = L"[Pattern]";
const wstring musicMetadataIdc = L"Music metadata";
const wstring patternMakerIdc = L"Pattern Maker";
const wstring patternNameIdc = L"Pattern Name";

//Music,Pattern common
const wstring endLineIdc = L"\r\n";
const wstring tagIdc = L"Tags";

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

static Pattern* ParseYmpFile(const wstring& fileDir, const wstring& file)
{
	Pattern* resultPattern = new Pattern;
	try
	{
		//indicator check
		size_t metadataPos = file.find(metadataIdc);
		if (metadataPos == wstring::npos) throw 0;

		size_t signaturePos = file.find(timeSignatureIdc, metadataPos);
		if (signaturePos == wstring::npos) throw 0;

		size_t patternPos = file.find(timeSignatureIdc, signaturePos);
		if (patternPos == wstring::npos) throw 0;

		//get ymm file name reference
		size_t startPos = file.find(musicMetadataIdc, metadataPos);
		size_t endPos = file.find(endLineIdc, startPos);
		wstring lineStr = file.substr(startPos, endPos - startPos);
		wstring ymmRefName;
		ShortCut::WordSeparateW(lineStr, L":", nullptr, &ymmRefName);
		resultPattern->ymmRefFileName = SongDir + ymmRefName;

		//patern maker informations
		startPos = file.find(patternMakerIdc, endPos);
		endPos = file.find(endLineIdc, startPos);
		lineStr = file.substr(startPos, endPos - startPos);
		wstring val;
		ShortCut::WordSeparateW(lineStr, L":", nullptr, &val);
		wstringstream wss;
		size_t makerCount;
		wss << val << endl;
		wss >> makerCount;

		resultPattern->MakerNameList.reserve(makerCount);
		for (size_t idx = 0; idx < makerCount; ++idx)
		{
			startPos = file.find(patternMakerIdc, endPos);
			if (startPos == wstring::npos) break;
			endPos = file.find(endLineIdc, startPos);
			lineStr = file.substr(startPos, endPos - startPos);
			wstring tempMakerName;
			ShortCut::WordSeparateW(lineStr, L":", nullptr, &tempMakerName);
			resultPattern->MakerNameList.emplace_back(tempMakerName);
		}

		//Pattern  name
		startPos = file.find(patternNameIdc, endPos);
		if (startPos == wstring::npos) throw 0;
		endPos = file.find(endLineIdc, startPos);

		lineStr = file.substr(startPos, endPos - startPos);
		ColumnSeparate(lineStr, nullptr, &resultPattern->DifficultyName);

		//pos of "Tags"
		startPos = file.find(tagIdc, endPos);
		if (startPos == wstring::npos) throw 0;
		endPos = file.find(endLineIdc, startPos);

		lineStr = file.substr(startPos, endPos - startPos);
		wstring tagList;
		ColumnSeparate(lineStr, nullptr, &tagList); //get whitespace separated wstring

		size_t tagStartPos = 0;
		while (true)
		{
			size_t whitespacePos = tagList.find(L' ', tagStartPos);
			wstring tag = tagList.substr(tagStartPos, whitespacePos - tagStartPos);
			if (tag.length() > 0)resultPattern->TagList.emplace(tag);
			tagStartPos = whitespacePos;
			if (tagStartPos != wstring::npos) while (tagList[++tagStartPos] == L' ');
			else break;
		}

	}
	catch (int v) //ymp file contents error
	{
		if (v == 0)
		{
			delete resultPattern;
			return nullptr;
		}
	}

	return resultPattern;
}

static Music* ParseYmmFile(const wstring& fileDir, const wstring& file)
{
	Music* resultMusic = new Music;
	try
	{
		// pos of "File: xxx;
		size_t filePos = file.find(musicFileIdc);
		if (filePos == wstring::npos) throw 0;

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
		if (musicNamePos == wstring::npos) throw 0;
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
		if (artistPos == wstring::npos) throw 0;
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
		if (tagPos == wstring::npos) throw 0;
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
	catch (int v) //ymm file contents error
	{
		if (v == 0)
		{
			delete resultMusic;
			return nullptr;
		}
	}

	return resultMusic;

}

void MusicScroll::LoadPattern()
{
	vector<wstring> ympList;
	ympList.reserve(128);

	ShortCut::GetFileList(ympList, PatternDir, ExtPattern);

	for (const wstring& it : ympList)
	{
		std::ifstream fin(it, std::ios::binary);

		fin.seekg(0, std::ios_base::end);
		int size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		string utf8Str;
		utf8Str.resize(size);
		fin.read(&utf8Str[0], size);
		wstring uniFile = ShortCut::UTF8ToWstring(utf8Str); //ymm file road complete

		Pattern* pattern = ParseYmpFile(it, uniFile);
		if (pattern != nullptr)
		{
			auto musicIdxIter = musicIndexMap.find(pattern->ymmRefFileName.c_str());
			if (musicIdxIter == musicIndexMap.end()) //no Music File
			{
				delete pattern;
				continue;
			}

			pattern->ympFileName = it;
			size_t musicIdx = musicIdxIter->second;
			musicList[musicIdx]->patternList.emplace_back(pattern); //add pattern to the corresponding music
		}

		fin.close();
	}
}

void MusicScroll::LoadMusic()
{
	vector<wstring> ymmList;
	ymmList.reserve(128);

	ShortCut::GetFileList(ymmList, MusicDir, ExtMusic);

	musicList.clear();
	musicList.reserve(ymmList.size());
	size_t musicIdx = 0;
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
			musicIndexMap.emplace(make_pair(it, musicIdx++));
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
	LoadPattern();
	CreateMusicBoxes();
	CreatePatternBoxes();

}
