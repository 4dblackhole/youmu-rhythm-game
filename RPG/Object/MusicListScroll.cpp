#include "framework.h"
#include "MusicListScroll.h"

constexpr int MusicScrollCenterX = -235;
constexpr int MusicScrollCenterY = -345;
constexpr int MusicScrollWidth = 450;
constexpr int MusicScrollHeight = 670;
constexpr int MusicScrollRightX = (MusicScrollCenterX + MusicScrollWidth / 2);
constexpr int MusicScrollTopY = (MusicScrollCenterY + MusicScrollHeight / 2);

constexpr float NoMusicTextSize = 40.0f / D2Ddevice::DefaultFontSize;
constexpr int NoMusicTextX = MusicScrollRightX - MusicScrollWidth;
constexpr int NoMusicTextY = -MusicScrollTopY + 60; // Y axis of 3D and 2D is reverse

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
constexpr int TextWidth = BoxWidth;
constexpr int TextHeight = 40;
constexpr int TextAreaHeight = clipAreaHeight;

constexpr int PTextWidth = PBoxWidth;

MusicListScroll::MusicListScroll() : MusicListScroll(MusicScrollCenterX, MusicScrollCenterY, MusicScrollWidth, MusicScrollHeight)
{
}

MusicListScroll::MusicListScroll(float x, float y, float w, float h) :
	scrollImg(x, y, w, h)
{
	InitMusicListScroll();
}

MusicListScroll::~MusicListScroll()
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

	ReleasePatternBox();
}

void MusicListScroll::OnBeginScene()
{
	if (!musicList.empty()) 
	{
		UpdateScrollMatrix();
		ChangeSelectMusic(currentSelectMusic);
	}
}

void MusicListScroll::OnEndScene()
{
	StopMusic(currentSelectMusic);
}

void MusicListScroll::PlayMusic(size_t idx)
{
	if (!musicList.empty()) FMODSYSTEM.System()->playSound(musicList[idx]->music, nullptr, false, &musicList[idx]->channel);
}

void MusicListScroll::StopMusic(size_t idx)
{
	if (!musicList.empty()) FMODSYSTEM.System()->playSound(musicList[idx]->music, nullptr, true, &musicList[idx]->channel);
}

void MusicListScroll::UpdateScrollMatrix()
{
	const float scrollStartPosOffset = -scrollImg.GetWorld3d().GetObjectScale().y * 0.5f;
	const float scrollYpos = scrollStartPosOffset + scrollPos * 20.0f;

	scrollMatrix.SetPosition({ 0, scrollYpos });

	if (!musicList.empty())
	{
		Music*& currentMusic = musicList[currentSelectMusic];
		if (!currentMusic->patternList.empty())
		{
			UpdatePatternHeightMatrix(currentSelectMusic);
		}
	}
	NotifyScrollMatrixUpdate();
}

void MusicListScroll::OnResize(float newW, float newH)
{
	scrollImg.OnResize();

	noMusicText->GetWorld2d().OnParentWorldUpdate();
	scrollMatrix.OnParentWorldUpdate();
	patternHeightMatrix.OnParentWorldUpdate();
	NotifyScrollMatrixUpdate();

}

void MusicListScroll::OnMouseDown(WPARAM btnState, int x, int y)
{

}

void MusicListScroll::OnMouseWheel(WPARAM wState, int x, int y)
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

void MusicListScroll::ChangeSelectMusic(size_t musicIdx)
{
	previousSelectMusic = currentSelectMusic;
	currentSelectMusic = (int)musicIdx;

	//bordercolor Change
	musicBoxList[previousSelectMusic]->BorderColor = MyColorF::GhostGreen;
	musicBoxList[currentSelectMusic]->BorderColor = MyColorF::CherryPink;

	//change preview music
	StopMusic(previousSelectMusic);
	PlayMusic(currentSelectMusic);

	//move the music boxes as height of pattern boxes
	UpdatePatternHeightMatrix(currentSelectMusic);
	ChangeTargetScrollMatrix();

	ChangePatternBox(musicIdx);

	if (!musicList[currentSelectMusic]->patternList.empty())
	{
		size_t patternIdx = min(currentSelectPattern, musicList[currentSelectMusic]->patternList.size() - 1);
		previousSelectPattern = 0;
		patternBoxList[patternIdx]->BorderColor = MyColorF::CherryPink;
		patternTextList[patternIdx]->Color = MyColorF::CherryPink;
	}
}

void MusicListScroll::ChangeSelectPattern(size_t idx)
{
	previousSelectPattern = currentSelectPattern;
	currentSelectPattern = (int)idx;

	//bordercolor Change
	patternBoxList[previousSelectPattern]->BorderColor = MyColorF::GhostGreen;
	patternTextList[previousSelectPattern]->Color = MyColorF::GhostGreen;

	patternBoxList[currentSelectPattern]->BorderColor = MyColorF::CherryPink;
	patternTextList[currentSelectPattern]->Color = MyColorF::CherryPink;
}

int MusicListScroll::GetAdjustedCurrentPatternIdx() const
{
	if (musicList[currentSelectMusic]->patternList.empty()) return 0;
	return min(currentSelectPattern, ((int)musicList[currentSelectMusic]->patternList.size() - 1));
}

void MusicListScroll::Update(float dt)
{

	if (!musicList.empty())
	{
		if (KEYBOARD.Down(VK_LEFT))
		{
			if (currentSelectMusic > 0)
			{
				FMODSYSTEM.Play(FmodSystem::Name::button01a);
				ChangeSelectMusic((size_t)currentSelectMusic - 1);
			}
		}
		if (KEYBOARD.Down(VK_RIGHT))
		{
			if (currentSelectMusic < musicList.size() - 1)
			{
				FMODSYSTEM.Play(FmodSystem::Name::button01a);
				ChangeSelectMusic((size_t)currentSelectMusic + 1);
			}
		}

		if (KEYBOARD.Down(VK_UP))
		{
			currentSelectPattern = GetAdjustedCurrentPatternIdx();

			if (currentSelectPattern > 0)
			{
				FMODSYSTEM.Play(FmodSystem::Name::button01a);
				ChangeSelectPattern((size_t)currentSelectPattern - 1);
			}
		}

		if (KEYBOARD.Down(VK_DOWN))
		{
			currentSelectPattern = GetAdjustedCurrentPatternIdx();

			if (currentSelectPattern < (int)musicList[currentSelectMusic]->patternList.size() - 1)
			{
				FMODSYSTEM.Play(FmodSystem::Name::button01a);
				ChangeSelectPattern((size_t)currentSelectPattern + 1);
			}
		}
	}
}

void MusicListScroll::UpdatePatternHeightMatrix(size_t musicIdx)
{
	Music*& currentMusic = musicList[musicIdx];
	const float patternHeight = float(musicList[currentSelectMusic]->patternList.size() * (size_t)(PBoxHeight + PBoxEdgeY));
	patternHeightMatrix.SetPosition({ 0, patternHeight });
}

void MusicListScroll::ChangeTargetScrollMatrix()
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
			musicBoxList[boxIdx]->GetWorld2d().SetParentWorld(&patternHeightMatrix);
		}
	}
	else
	{
		for (size_t boxIdx = startPos; boxIdx <= endPos; ++boxIdx)
		{
			musicBoxList[boxIdx]->GetWorld2d().SetParentWorld(&scrollMatrix);
		}
	}

	NotifyScrollMatrixUpdate();
}

void MusicListScroll::NotifyScrollMatrixUpdate()
{
	// music boxes' root world is scrollMatrix
	// and all of boxes and texts are referencing the music boxes' world
	for (auto& it : musicBoxList) it->GetWorld2d().OnParentWorldUpdate();
	for (auto& it : musicTextList) it->GetWorld2d().OnParentWorldUpdate();

	for (auto& it : patternBoxList) it->GetWorld2d().OnParentWorldUpdate();
	for (auto& it : patternTextList) it->GetWorld2d().OnParentWorldUpdate();
}

void MusicListScroll::InitBoxListParentWorld()
{
	for (size_t boxIdx = 0; boxIdx < musicList.size(); ++boxIdx)
	{
		if (boxIdx > currentSelectMusic) musicBoxList[boxIdx]->GetWorld2d().SetParentWorld(&patternHeightMatrix);
		else musicBoxList[boxIdx]->GetWorld2d().SetParentWorld(&scrollMatrix);
	}
	for (size_t textIdx = 0; textIdx < musicTextList.size(); ++textIdx)
	{
		musicTextList[textIdx]->GetWorld2d().SetParentWorld(&musicBoxList[textIdx]->GetWorld2d());
	}
}

void MusicListScroll::CreateMusicBox()
{
	musicBoxList.clear();
	for (size_t i = 0; i < musicList.size(); ++i)
	{
		// Box =======================================================================
		const XMFLOAT2& scale = scrollImg.GetWorld3d().GetObjectScale();
		const XMFLOAT3& pos = scrollImg.GetWorld3d().GetLocalPosition();

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
		tempBox->GetWorld2d().SetAlignX(scrollImg.GetWorld3d().GetAlignX());
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
			DwLayout2D::GetLayoutMetrics(musicDesc, tempFormat, &mt);

			const float defaultSize = 1.5f;
			const float maximumLayoutWidth = TextWidth / defaultSize * 2;

			const float musicTextSize = defaultSize;// mt.width >= maximumLayoutWidth ? defaultSize * sqrtf(maximumLayoutWidth / mt.width) : defaultSize;
			const float textLeft = TextEdgeX - BoxWidth * 0.5f;
			const float textTop = -BoxHeight * 0.5f;

			DwLayout2D* tempLayout = new DwLayout2D(musicTextSize, MyColorF::GhostGreen, { 0, 0 });
			tempLayout->GetWorld2d().SetAlignX(tempBox->GetWorld2d().GetAlignX());
			tempLayout->maxW = maximumLayoutWidth;
			tempLayout->maxH = TextHeight;
			tempLayout->GetWorld2d().SetPosition({ textLeft,textTop });
			tempLayout->GetWorld2d().SetScale(defaultSize);
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

void MusicListScroll::ReleasePatternBox()
{
	for (auto& it : patternBoxList) delete it;
	for (auto& it : patternTextList) delete it;

	patternBoxList.clear();
	patternTextList.clear();
}

void MusicListScroll::ChangePatternBox(size_t musicIdx)
{
	ReleasePatternBox();

	for (size_t i = 0; i < musicList[musicIdx]->patternList.size(); ++i)
	{
		Rectangle2D* tempBox = new Rectangle2D({ -PBoxWidth / 2,-PBoxHeight / 2,PBoxWidth / 2,PBoxHeight / 2 });

		tempBox->GetWorld2d().SetAlignX(scrollImg.GetWorld3d().GetAlignX());
		tempBox->GetWorld2d().SetPosition({ PBoxOffsetX, (BoxHeight + PBoxHeight) * 0.5f + BoxEdgeY + (PBoxHeight+PBoxEdgeY) * i });
		tempBox->GetWorld2d().SetParentWorld(&musicBoxList[musicIdx]->GetWorld2d());
		tempBox->IsRound = true;
		tempBox->SetRadius(5.0f);
		tempBox->BorderSize = 1.5f;
		tempBox->BorderColor = MyColorF::GhostGreen;
		patternBoxList.emplace_back(tempBox);

		wstring patternDesc = L"[" + musicList[musicIdx]->patternList[i]->DifficultyName + L"]";
		IDWriteTextFormat*& tempFormat = D2D.GetFont(D2Ddevice::FontName::DefaultFont);

		DWRITE_TEXT_METRICS mt;
		DwLayout2D::GetLayoutMetrics(patternDesc, tempFormat, &mt);

		constexpr float defaultSize = 1.5f;
		constexpr float maximumLayoutWidth = TextWidth / defaultSize * 2;

		constexpr float musicTextSize = defaultSize;
		constexpr float textLeft = TextEdgeX - PBoxWidth * 0.5f;
		constexpr float textTop = -(PBoxHeight - PBoxEdgeY) * 0.5f;

		DwLayout2D* tempLayout = new DwLayout2D(musicTextSize, MyColorF::GhostGreen, { 0, 0 });
		tempLayout->GetWorld2d().SetAlignX(tempBox->GetWorld2d().GetAlignX());
		tempLayout->GetWorld2d().SetParentWorld(&tempBox->GetWorld2d());
		tempLayout->maxW = maximumLayoutWidth;
		tempLayout->maxH = (float)PBoxHeight;
		tempLayout->GetWorld2d().SetPosition({ textLeft,textTop });
		tempLayout->GetWorld2d().SetScale(defaultSize);
		tempLayout->SetLayout(patternDesc, tempFormat);
		patternTextList.emplace_back(tempLayout);

	}
}

void MusicListScroll::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	scrollImg.Render(deviceContext, cam);

	XMFLOAT4X4 drawAreaWorld{};
	XMStoreFloat4x4(&drawAreaWorld,
		XMLoadFloat4x4(&scrollImg.GetWorld3d().GetGlobalWorld()) * cam.ViewProj());
	D2D1::Matrix3x2F drawAreaWorld3x2f = ShortCut::WVP3Dto2D(drawAreaWorld, (float)App->GetWidth(), (float)App->GetHeight());

	const float scrollW = scrollImg.GetWorld3d().GetObjectScale().x;
	const float scrollH = scrollImg.GetWorld3d().GetObjectScale().y;
	const float clipAreaWidth = scrollW;
	const float clipAreaHeight = scrollH - clipAreaOffset * 2;
	const float textAreaWidth = scrollW - (TextEdgeX + BoxEdgeX) * 2;
	const float textAreaHeight = scrollH - clipAreaOffset * 2;
	/*
	ID2D1Layer* pLayer = nullptr;
	D2D.GetRenderTarget()->CreateLayer(nullptr, &pLayer);

	D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters(
		Rectangle2D::DefaultRect,   // 클리핑 영역
		nullptr,                // 기하 마스크
		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		D2D1::IdentityMatrix(), //D2D1::Matrix3x2F::Scale({ clipAreaWidth, clipAreaHeight }) * drawAreaWorld3x2f, // 변환 매트릭스
		1.0f,                   // 불투명도
		nullptr,                // 불투명도 마스크
		D2D1_LAYER_OPTIONS_NONE
	);
	
	D2D.GetRenderTarget()->PushLayer(layerParams, pLayer);
	*/
	D2D.GetRenderTarget()->SetTransform(D2D1::Matrix3x2F::Scale({ clipAreaWidth, clipAreaHeight }) * drawAreaWorld3x2f);
	D2D.GetRenderTarget()->PushAxisAlignedClip(Rectangle2D::DefaultRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	if (musicList.empty()) noMusicText->Draw();
	else
	{
		for (auto& it : musicBoxList) it->Draw();
		for (auto*& it : patternBoxList) it->Draw();

		D2D.GetRenderTarget()->SetTransform(D2D1::Matrix3x2F::Scale({ textAreaWidth, textAreaHeight }) * drawAreaWorld3x2f);
		D2D.GetRenderTarget()->PushAxisAlignedClip(Rectangle2D::DefaultRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		for (auto*& it : musicTextList) it->Draw();
		for (auto*& it : patternTextList) it->Draw();
		D2D.GetRenderTarget()->PopAxisAlignedClip();
	}
	D2D.GetRenderTarget()->PopAxisAlignedClip();
	//D2D.GetRenderTarget()->PopLayer();

	//ReleaseCOM(pLayer);

}

Music* MusicListScroll::GetCurrentMusic()
{
	if (musicList.empty()) return nullptr;
	else return musicList[currentSelectMusic];
}

Pattern* MusicListScroll::GetCurrentPattern()
{
	if (GetCurrentMusic() == nullptr) return nullptr;

	if (GetCurrentMusic()->patternList.empty()) return nullptr;
	else return GetCurrentMusic()->patternList[(size_t)GetAdjustedCurrentPatternIdx()];

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

static bool ColumnSeparate(const wstring_view& source, wstring* first, wstring* second)
{
	return ShortCut::WordSeparateW(source, L":", first, second);
	/*
	size_t columnPos = source.find(L':'); // key: filename
	if (columnPos == wstring::npos) return false; //not valid desc

	if (first != nullptr)*first = source.substr(0, columnPos);

	if (second != nullptr)
	{
		size_t secondStartPos = columnPos;
		while (source[++secondStartPos] == L' ');
		*second = wstring(source.substr(secondStartPos));
	}
	return true;
	*/
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
		wstring_view lineStr(file.c_str() + startPos, endPos - startPos);
		wstring ymmRefName;
		ShortCut::WordSeparateW(lineStr, L":", nullptr, &ymmRefName);
		resultPattern->ymmRefFileName = SongDir + ymmRefName;

		//patern maker informations
		startPos = file.find(patternMakerIdc, endPos);
		endPos = file.find(endLineIdc, startPos);
		lineStr = wstring_view(file.c_str() + startPos, endPos - startPos);
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
			lineStr = wstring_view(file.c_str() + startPos, endPos - startPos);
			wstring tempMakerName;
			ShortCut::WordSeparateW(lineStr, L":", nullptr, &tempMakerName);
			resultPattern->MakerNameList.emplace_back(tempMakerName);
		}

		//Pattern  name
		startPos = file.find(patternNameIdc, endPos);
		if (startPos == wstring::npos) throw 0;
		endPos = file.find(endLineIdc, startPos);

		lineStr = wstring_view(file.c_str() + startPos, endPos - startPos);
		ColumnSeparate(lineStr, nullptr, &resultPattern->DifficultyName);

		//pos of "Tags"
		startPos = file.find(tagIdc, endPos);
		if (startPos == wstring::npos) throw 0;
		endPos = file.find(endLineIdc, startPos);

		lineStr = wstring_view(file.c_str() + startPos, endPos - startPos);
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

		wstring_view lineStr = wstring_view(file.c_str()+filePos, endPos - filePos);
		ColumnSeparate(lineStr, nullptr, &resultMusic->FileName);

		const wstring musicDir = fileDir + L'/' + resultMusic->FileName;
		const string musicDirUTF8 = ShortCut::WstringToUTF8(musicDir);
		FMOD_RESULT result = FmodSystem::GetInstance().System()->createStream(musicDirUTF8.c_str(), FMOD_LOOP_OFF | FMOD_CREATESAMPLE, nullptr, &resultMusic->music);
		
		if (result != FMOD_OK) throw 0; //no file detected

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

		lineStr = wstring_view(file.c_str()+musicNamePos, endPos - musicNamePos);
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
			lineStr = wstring_view(file.c_str()+musicNamePos, endPos - musicNamePos);
			wstring tempMusicName;
			ColumnSeparate(lineStr, nullptr, &tempMusicName);
			resultMusic->MusicNameList.emplace_back(tempMusicName);
		}

		// pos of "Artist"
		size_t artistPos = file.find(artistIdc, endPos);
		if (artistPos == wstring::npos) throw 0;
		endPos = file.find(endLineIdc, artistPos);

		lineStr = wstring_view(file.c_str()+artistPos, endPos - artistPos);
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
			lineStr = wstring_view(file.c_str()+artistPos, endPos - artistPos);
			wstring tempArtistName;
			ColumnSeparate(lineStr, nullptr, &tempArtistName);
			resultMusic->ArtistList.emplace_back(tempArtistName);
		}

		// pos of "Tags"
		size_t tagPos = file.find(tagIdc, endPos);
		if (tagPos == wstring::npos) throw 0;
		endPos = file.find(endLineIdc, tagPos);

		lineStr = wstring_view(file.c_str() + tagPos);
		wstring tagList;
		ColumnSeparate(lineStr, nullptr, &tagList); //get whitespace separated wstring

		//get tag elements
		size_t tagStartPos = 0;
		while (true)
		{
			size_t whitespacePos = tagList.find(L' ', tagStartPos);
			if (whitespacePos != wstring::npos)
			{
				wstring_view tag(tagList.c_str() + tagStartPos, whitespacePos - tagStartPos);
				if (tag.length() > 0) resultMusic->TagList.emplace(tag);
				tagStartPos = whitespacePos;
				while (tagList[++tagStartPos] == L' ');
			}
			else //last element
			{
				wstring_view tag(tagList.c_str() + tagStartPos);
				if (tag.length() > 0) resultMusic->TagList.emplace(tag);
				break;
			}
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

void MusicListScroll::LoadPattern()
{
	vector<wstring> ympList;
	ympList.reserve(128);

	ShortCut::GetFileList(ympList, PatternDir, ExtPattern);

	for (const wstring& it : ympList)
	{
		wstring uniFile = ShortCut::ReadUTF8File(it); //ymm file road complete

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
	}
}

void MusicListScroll::LoadMusic()
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


void MusicListScroll::InitMusicListScroll()
{
	textureManager.AddTexture(App->GetDevice(), TextureName::MusicScroll, TextureDir + L"SelectMusic/MusicScroll.png");
	scrollImg.GetWorld3d().SetAlignX(AlignModeX::Right);
	scrollImg.GetWorld3d().SetAlignY(AlignModeY::Top);
	scrollImg.GetWorld3d().SetParentDrawWorld();
	scrollImg.SetTexture(&textureManager.GetTexture(TextureName::MusicScroll));

	DWRITE_TEXT_METRICS mt;
	LPCWSTR tempstr = L"No Music";
	IDWriteTextFormat*& tempFormat = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	DwLayout2D::GetLayoutMetrics(tempstr, tempFormat, &mt);

	noMusicText.reset(new DwLayout2D(NoMusicTextSize, MyColorF::GhostGreen, { (float)MusicScrollCenterX + (mt.width * NoMusicTextSize * 0.5f) , NoMusicTextY }));
	noMusicText->GetWorld2d().SetAlignX(AlignModeX::Right);
	noMusicText->SetLayoutRightAlign(tempstr, tempFormat);

	LoadMusic();
	LoadPattern();
	CreateMusicBox();

	scrollMatrix.SetAlignX(scrollImg.GetWorld3d().GetAlignX());
	scrollMatrix.SetParentDrawWorld();
	patternHeightMatrix.SetParentWorld(&scrollMatrix);
}
