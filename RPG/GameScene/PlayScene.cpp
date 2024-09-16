#include "framework.h"
#include "PlayScene.h"

constexpr float LayoutSize = 4.0f;
constexpr int LayoutBaseX = -60;
constexpr int LayoutBaseY = 200;
constexpr int LayoutDistanceY = 100;

constexpr int TriangleOffsetX = -8;
constexpr int TriangleOffsetY = 11;

//mode related
constexpr float LaneWidth = 180.0f;
constexpr float CircleDiameter = 90.0f;
constexpr float LargeCircleDiameter = 144.0f;

constexpr int LaneMaxLength = 10000;

PlayScene::PlayScene(Music* m, Pattern* p) :
	music(m), pattern(p),
	prevSceneSprite(0, 0, (float)StandardWidth, (float)StandardHeight, { 1,1,1,0.2f }, false),
	noteSprite(0, 0)
{
	timer.Reset();

	InitPauseOptionLayoutList();
	InitLoadingText();
	InitCurrentTimeText();

	InitTextures();
	InitLanes();
	InitSprites();
}

PlayScene::~PlayScene()
{
	StopThread();
	SafeDelete(musicScore);
	ReleaseTextures();
	ReleasePauseBackground();

}

void PlayScene::InitLanes()
{
	testLane.Sprite.GetWorld3d().SetObjectScale({ LaneWidth, LaneMaxLength });
	testLane.HitPosition = 120.0;

	testLane.AddNoteType(1);
	testLane.AddNoteType(2);
	testLane.AddNoteType(3);
	testLane.AddNoteType(4);

	testLane.AddNoteDrawDesc(1, { MyColor4::MyRed, CircleDiameter });
	testLane.AddNoteDrawDesc(2, { MyColor4::MyBlue, CircleDiameter });
	testLane.AddNoteDrawDesc(3, { MyColor4::MyRed, LargeCircleDiameter });
	testLane.AddNoteDrawDesc(4, { MyColor4::MyBlue, LargeCircleDiameter });

}

bool PlayScene::LoadTextureArray(map<const string, Texture*>& container, const string& keyStr, const vector<LPCWSTR>& fileList)
{
	if (fileList.empty()) return false;
	//Texture Array
	vector<ID3D11Texture2D*> tempTextureList;
	vector<LPCWSTR>::const_iterator cIter = fileList.begin();
	while (cIter!= fileList.cend())
	{
		ID3D11Texture2D* tempTexture2D{};

		//get file name
		try
		{
			const BOOL& result = ShortCut::FileExists(*cIter);
			if (result == FALSE) throw result;

			D3DX11_IMAGE_LOAD_INFO info{};
			info.MipLevels = 1;
			HR(D3DX11CreateTextureFromFile(App->GetDevice(), *cIter, &info, nullptr, (ID3D11Resource**)&tempTexture2D, nullptr));
			tempTextureList.emplace_back(tempTexture2D);
		}
		catch (BOOL b)
		{
			UNREFERENCED_PARAMETER(b);
		}
		++cIter;
	}

	bool result = LoadTextureArrayFromResource(container, keyStr, tempTextureList);

	for (auto& it : tempTextureList) ReleaseCOM(it);
	tempTextureList.clear();
	return result;
}

bool PlayScene::LoadTextureArrayIndexed(map<const string, Texture*>& container, const string& keyStr, const wstring& fileName)
{	
	//in case there is only one texture
	if (ShortCut::FileExists(fileName.c_str()) != FALSE)
	{
		Texture* tempTexture = new Texture;
		tempTexture->CreateTexture(App->GetDevice(), fileName);
		container.emplace(make_pair(keyStr, tempTexture));
		return true;
	}

	//Texture Array
	int textureIdx = 0;
	vector<ID3D11Texture2D*> tempTextureList;
	while (true)
	{
		ID3D11Texture2D* tempTexture2D{};
		
		//get file name
		const size_t& commaPos = fileName.find_last_of('.');
		const wstring_view& ext = wstring_view(fileName.c_str() + commaPos);
		const wstring_view& name = wstring_view(fileName.c_str(), commaPos);
		const wstring& idxStr = L"-" + to_wstring(textureIdx);
		const wstring& resultFileName = (wstring)name + idxStr.data() + ext.data();
		const BOOL& result = ShortCut::FileExists(resultFileName.c_str());
		if (result == FALSE) break;
		
		D3DX11_IMAGE_LOAD_INFO info{};
		info.MipLevels = 1;
		D3DX11CreateTextureFromFile(App->GetDevice(), resultFileName.c_str(),&info, nullptr, (ID3D11Resource**)&tempTexture2D, nullptr);
		tempTextureList.emplace_back(tempTexture2D);
		++textureIdx;
	}

	bool result = LoadTextureArrayFromResource(container, keyStr, tempTextureList);

	for (auto& it : tempTextureList) ReleaseCOM(it);
	tempTextureList.clear();

	return result;
}

bool PlayScene::LoadTextureArrayFromResource(map<const string, Texture*>& container, const string& keyStr, const vector<ID3D11Texture2D*>& textureList)
{
	//no files
	if (textureList.empty()) return false;

	//create texture2DArr
	D3D11_TEXTURE2D_DESC texture2Ddesc;
	(*textureList.begin())->GetDesc(&texture2Ddesc);
	texture2Ddesc.ArraySize = (UINT)textureList.size();
	texture2Ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//texture2Ddesc.MipLevels = 1;
	//texture2Ddesc.SampleDesc.Count = 1;

	ID3D11Texture2D* texture2DArr;
	HRESULT hr = (App->GetDevice()->CreateTexture2D(&texture2Ddesc, nullptr, &texture2DArr));
	if (FAILED(hr)) return false;

	for (UINT i = 0; i < textureList.size(); ++i) {
		for (UINT mip = 0; mip < texture2Ddesc.MipLevels; ++mip) {
			App->GetDeviceContext()->CopySubresourceRegion(
				texture2DArr,
				D3D11CalcSubresource(mip, i, texture2Ddesc.MipLevels),
				0, 0, 0,
				textureList.at(i),
				mip,
				nullptr
			);
		}
	}

	//texture2DArr to SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texture2Ddesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = texture2Ddesc.MipLevels;
	srvDesc.Texture2DArray.ArraySize = texture2Ddesc.ArraySize;

	Texture* arrTexture = new Texture;
	hr = (App->GetDevice()->CreateShaderResourceView(texture2DArr, &srvDesc, &arrTexture->srv));
	if (FAILED(hr)) return false;

	Texture::UpdateDescFromSRV(*arrTexture);
	container.emplace(make_pair(keyStr, arrTexture));
	ReleaseCOM(texture2DArr);

	return true;
}

void PlayScene::InitTextures()
{
	ReleaseTextures();
	const wstring skinDir = SkinDir + L"test Skin/";
	Texture* tempTexture = nullptr;

	tempTexture = new Texture;
	tempTexture->CreateTexture(App->GetDevice(), skinDir + L"LaneBackground.png");
	textureList.emplace(make_pair(TextureName::LaneBackground, tempTexture));

	LoadTextureArray(
		textureList,
		TextureName::note,
		{
			(skinDir + L"note.png").c_str(), 
			(skinDir + L"noteoverlay.png").c_str() 
		});

}

void PlayScene::ReleaseTextures()
{
	for (decltype(textureList)::value_type& it : textureList)
	{
		SafeDelete(it.second);
	}
	textureList.clear();
}

void PlayScene::InitSprites()
{
	Texture* const& laneBGTexture = textureList.find(TextureName::LaneBackground)->second;
	testLane.Sprite.GetWorld3d().SetCenterPosition({ 0, 0.5f, 0 });
	testLane.Sprite.GetWorld3d().SetLocalRotation({ 0, 0, -Math::PI * 0.5f });
	testLane.Sprite.GetWorld3d().SetLocalPosition({ LaneWidth,100,0 });
	testLane.Sprite.RepeatTextureInExtraArea(laneBGTexture->width, laneBGTexture->height);
	testLane.Sprite.GetWorld3d().SetAlignX(AlignModeX::Left);
	testLane.Sprite.GetWorld3d().SetParentDrawWorld();
	testLane.Sprite.SetTexture(laneBGTexture);

	Texture* const& hitCircleTexture = textureList.find(TextureName::note)->second;
	noteSprite.GetWorld3d().SetParentWorld(&testLane.Sprite.GetWorld3d());
	noteSprite.GetWorld3d().SetObjectScale(CircleDiameter);
	noteSprite.SetTexture(hitCircleTexture);
	noteSprite.Diffuse = MyColor4::GhostGreen;
	noteSprite.SetTextureID((UINT)SpriteTextureID::Note);

	noteOverlaySprite.GetWorld3d().SetParentWorld(&noteSprite.GetWorld3d());
	noteOverlaySprite.GetWorld3d().SetObjectScale(CircleDiameter);
	noteOverlaySprite.SetTexture(hitCircleTexture);
	noteOverlaySprite.Diffuse = MyColor4::White;
	noteOverlaySprite.SetTextureID((UINT)SpriteTextureID::NoteOverlay);
}

void PlayScene::InitCurrentTimeText()
{
	IDWriteTextFormat*& currentFont = D2D.GetFont(D2Ddevice::FontName::DefaultFont);

	currentTimeText.Color = MyColorF::GhostGreen;
	currentTimeText.GetWorld2d().SetAlignX(AlignModeX::Left);
	currentTimeText.GetWorld2d().SetPosition({ 0,0 });

	UpdateCurrentTimeText();

}

void PlayScene::UpdateCurrentTimeText()
{
	using namespace chrono;
	IDWriteTextFormat*& currentFont = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	WCHAR tempStr[50];
	swprintf_s(tempStr, L"Time: %.3fs", duration_cast<duration<double, ratio<1, 1>>>(totalMusicTime).count());
	currentTimeText.SetLayout(tempStr, currentFont);
}

void PlayScene::InitLoadingText()
{
	IDWriteTextFormat*& currentFont = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	constexpr LPCWSTR loadingStr = L"Now Loading";
	constexpr LPCWSTR loadingCompleteStr = L"Loading Complete";

	DWRITE_TEXT_METRICS mt;
	DwLayout2D::GetLayoutMetrics(loadingStr, currentFont, &mt);

	loadingText.GetWorld2d().SetScale(2.0f);
	loadingText.Color = MyColorF::GhostGreen;
	loadingText.GetWorld2d().SetAlignX(AlignModeX::Mid);
	loadingText.GetWorld2d().SetPosition({ -mt.width * loadingText.GetWorld2d().GetScale().x * 0.5f,100 });
	loadingText.SetLayout(loadingStr, D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	DwLayout2D::GetLayoutMetrics(loadingCompleteStr, currentFont, &mt);
	loadingCompleteText.GetWorld2d().SetScale(2.0f);
	loadingCompleteText.Color = MyColorF::GhostGreen;
	loadingCompleteText.GetWorld2d().SetAlignX(AlignModeX::Mid);
	loadingCompleteText.GetWorld2d().SetPosition({ -mt.width * loadingCompleteText.GetWorld2d().GetScale().x * 0.5f,100 });
	loadingCompleteText.SetLayout(loadingCompleteStr, D2D.GetFont(D2Ddevice::FontName::DefaultFont));

}

void PlayScene::InitPauseOptionLayoutList()
{
	IDWriteTextFormat*& defaultFont = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	for (int i = 0; i < (int)PauseOption::MAX; ++i)
	{
		PauseOptLayoutList[i].GetWorld2d().SetAlignX(AlignModeX::Mid);
		PauseOptLayoutList[i].GetWorld2d().SetScale(LayoutSize);
		PauseOptLayoutList[i].GetWorld2d().SetPosition({ (float)LayoutBaseX,(float)(LayoutBaseY + (i * LayoutDistanceY)) });
		PauseOptLayoutList[i].SetLayout(pauseOptTxtMap[i], defaultFont);
		PauseOptLayoutList[i].GetWorld2d().SetParentDrawWorld();
	}

	pauseOptSelectTriangle.GetWorld2d().SetScale(3.0f);
	pauseOptSelectTriangle.GetWorld2d().SetParentWorld(&PauseOptLayoutList[pauseOptionKey].GetWorld2d());
	pauseOptSelectTriangle.FillColor = MyColorF::GhostGreen;
	pauseOptSelectTriangle.BorderSize = 0.5f;
	pauseOptSelectTriangle.GetWorld2d().SetPosition({ (float)TriangleOffsetX, (float)TriangleOffsetY });
}

void PlayScene::InitPauseBackground()
{
	ReleasePauseBackground();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (UINT)App->GetWidth();
	textureDesc.Height = (UINT)App->GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;


	HR(App->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pauseBG));
	HR(App->GetDevice()->CreateRenderTargetView(pauseBG, nullptr, &pauseBgRTV));
	HR(App->GetDevice()->CreateShaderResourceView(pauseBG, nullptr, &pauseBgTexture.srv));

	D2D.ReleaseBackBuffer();
	D2D.ResetBackBuffer(pauseBG);

	App->GetDeviceContext()->OMSetRenderTargets(1, &pauseBgRTV, nullptr);

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	App->GetDeviceContext()->ClearRenderTargetView(pauseBgRTV, (const float*)&App->GetBgColor());

	D2D.BeginDraw();
	RenderStatus(prevSceneStatus, App->GetDeviceContext(), App->GetCamera());
	D2D.EndDraw();

	//D3DX11SaveTextureToFile(App->GetDeviceContext(), pauseBG, D3DX11_IFF_PNG, L"asdf.png");
	prevSceneSprite.SetTexture(&pauseBgTexture);

	App->ResetRenderTarget();
	D2D.ReleaseBackBuffer();
	D2D.ResetBackBufferFromSwapChain(App->GetSwapChain());
}

void PlayScene::ReleasePauseBackground()
{
	ReleaseCOM(pauseBG);
	ReleaseCOM(pauseBgRTV);
	ReleaseCOM(pauseBgTexture.GetRefSRV());
}

static constexpr LPCWSTR OffsetIdc = L"Pattern Offset";
static constexpr LPCWSTR TimeSignatureIdc = L"[Time Signature]";
static constexpr LPCWSTR PatternIdc = L"[Pattern]";
static constexpr LPCWSTR EndlineIdc = L"\r\n";
static constexpr LPCWSTR BarlineIdc = L"--";
static constexpr LPCWSTR EffectTypeIdc = L"#";
static constexpr LPCWSTR measureIdc = L"measure";
static constexpr LPCWSTR BaseBpmIdc = L"Base BPM";
static constexpr LPCWSTR bpmIdc = L"bpm";
static constexpr LPCWSTR CommonTimeIdc = L"C";

static constexpr size_t EndlineIdcLength = 2;

const wstring MeasureEffect = wstring(EffectTypeIdc) + measureIdc;
const wstring BpmEffect = wstring(EffectTypeIdc) + bpmIdc;

void PlayScene::ParseBarLine(const wstring_view& lineStr, const RationalNumber<64>measureLength, size_t& measureIdx)
{
	musicScore->measures.emplace_back(measureLength);
	++measureIdx;
}
void PlayScene::ParseMeasure(const wstring_view& lineStr, RationalNumber<64>& measureLength)
{
	size_t signatureIdx = MeasureEffect.length() - 1;
	while (lineStr[++signatureIdx] == L' ');
	wstring val = (wstring)lineStr.substr(signatureIdx);

	if (val.compare(CommonTimeIdc) == 0) //#measure C
	{
		measureLength = RationalNumber<64>(1, 1);
	}
	else
	{
		measureLength = ShortCut::StrToRationalNumber<64>(val);
		DEBUG_BREAKPOINT;
	}
}
bool PlayScene::ParseEffect(const wstring_view& lineStr, RationalNumber<64>& resultSignature, wstring& resultEffectStr)
{
	bool validCheck = true;
	wstring signatureStr;
	wstring typeStr;
	validCheck = ShortCut::WordSeparateW(lineStr, L",", &signatureStr, &typeStr);
	if (!validCheck) return false;
	resultEffectStr = typeStr;

	//get signature
	resultSignature = ShortCut::StrToRationalNumber<64>(signatureStr);

	return true;
}
void PlayScene::ParseBPM(const wstring_view& str, const size_t measureIdx, const RationalNumber<64>& pos)
{
	wstringstream wss;
	wss << str << endl;
	double bpm;
	wss >> bpm;

	//compare with previous bpm
	double tobeCompareBpm = 0.0;
	if (musicScore->bpms.empty()) tobeCompareBpm = musicScore->baseBpm;
	else tobeCompareBpm = std::prev(musicScore->bpms.cend())->BPM();

	//if (bpm != tobeCompareBpm)
	musicScore->bpms.emplace(MusicBPM({ measureIdx, pos }, bpm));
}
void PlayScene::LoadTimeSignature(const wstring_view& content)
{
	size_t measureIdx = 0;
	RationalNumber<64> measureLength(1, 1);

	size_t startIdx = 0;
	size_t endIdx = content.find(EndlineIdc, startIdx);

	bool validCheck = true;

	auto ReadNextLine = [&]() -> bool
		{
			if (endIdx == wstring::npos) return true; //last line
			startIdx = endIdx + EndlineIdcLength;
			endIdx = content.find(EndlineIdc, startIdx);
			return false;
		};

	while(true)
	{
		wstring_view lineStr = content.substr(startIdx, endIdx - startIdx);

		// in case bar line
		if (lineStr.compare(BarlineIdc) == 0) //--
		{
			ParseBarLine(lineStr, measureLength, measureIdx);

			if (ReadNextLine()) break;
			else continue;
		}

		// Measure
		if (lineStr.find(MeasureEffect+L' ') != wstring_view::npos) //#measure n/d
		{
			ParseMeasure(lineStr, measureLength);

			if (ReadNextLine()) break;
			else continue;
		}

		wstring effectStr;
		RationalNumber<64> signature;

		validCheck = ParseEffect(lineStr, signature, effectStr);
		if (!validCheck)
		{
			if (ReadNextLine()) break;
			else continue;
		}

		//get type
		wstring effectType;
		wstring effectValue;
		validCheck = ShortCut::WordSeparateW(effectStr, L" ", &effectType, &effectValue);

		if (effectType.compare(BpmEffect) == 0) //#bpm xxx
		{
			ParseBPM(effectValue, measureIdx, signature);
		}

		if (ReadNextLine()) break; //in case last line
		else continue;
	}

	musicScore->measures.emplace_back(measureLength);

	InitTimeSignaturePrefixSum();

}
void PlayScene::LoadPattern(const wstring_view& content)
{
	size_t measureIdx = 0;

	size_t startIdx = 0;
	size_t endIdx = content.find(EndlineIdc, startIdx);

	bool validCheck = true;
	auto ReadNextLine = [&]() -> bool
		{
			if (endIdx == wstring::npos) return true; //last line
			startIdx = endIdx + EndlineIdcLength;
			endIdx = content.find(EndlineIdc, startIdx);
			return false;
		};

	while (true)
	{
		wstring_view lineStr = content.substr(startIdx, endIdx - startIdx);

		// in case bar line
		if (lineStr.compare(BarlineIdc) == 0) //--
		{
			++measureIdx;

			if (ReadNextLine()) break;
			else continue;
		}

		vector<pair<size_t, size_t>> idxArr;
		ShortCut::WordSeparateW(lineStr, L",", idxArr);

		wstring_view noteElements[(int)Note::DataOrder::MAX];
		for (int idx = 0; idx < idxArr.size(); ++idx)
		{
			if (idxArr[idx].second != wstring_view::npos)
				noteElements[idx] = wstring_view(lineStr.substr(idxArr[idx].first, idxArr[idx].second - idxArr[idx].first));
			else
				noteElements[idx] = wstring_view(lineStr.substr(idxArr[idx].first));
		}

		wstringstream wss;
		Note tempNote;
		tempNote.mp.measureIdx = measureIdx;

		//DataOrder::Beat
		tempNote.mp.position = ShortCut::StrToRationalNumber<64>(noteElements[(int)Note::DataOrder::Rhythm]);
		size_t referencingMeasureIdx = min(measureIdx, musicScore->measures.size() - 1);
		if (tempNote.mp.position >= musicScore->measures[referencingMeasureIdx].length) //out of measure
		{
			if (ReadNextLine()) break;
			else continue;
		}


		//DataOrder::Key
		wss << noteElements[(int)Note::DataOrder::Type];
		wss >> tempNote.noteType;

		//DataOrder:Action
		wss << noteElements[(int)Note::DataOrder::Action];
		wss >> tempNote.actionType;

		//DataOrder::Hitsound
		if (idxArr.size() > (int)Note::DataOrder::Hitsound)
		{
			tempNote.hitSound = noteElements[(int)Note::DataOrder::Hitsound];
		}

		wstring asdfdsaf;

		if (idxArr.size() > (int)Note::DataOrder::ExtraData)
		{
			tempNote.extraData= noteElements[(int)Note::DataOrder::ExtraData];
		}

		musicScore->notesPerTypeMap[tempNote.noteType].emplace(make_pair(tempNote.mp, tempNote));
		if (ReadNextLine()) break; //in case last line
		else continue;
	}
}
void PlayScene::StopLoadMusicScoreThread()
{
	loadMusicScoreThreadFlag = false;
	if (loadMusicScoreThread.joinable()) loadMusicScoreThread.join();
}

void PlayScene::InitTimeSignaturePrefixSum()
{
	measurePrefixSum.InitMeasurePrefixSum(&musicScore->measures);
	bpmTimingPrefixSum.InitBpmTimingPrefixSum(&musicScore->bpms, measurePrefixSum);
}

void PlayScene::LoadMusicScore()
{
	musicScoreLoadFlag = false;
	musicScore = new MusicScore;
	const wstring& fileName = pattern->ympFileName;

	wstring uniFile = ShortCut::ReadUTF8File(fileName);

	try
	{
		//file contents check
		size_t offsetIdx = uniFile.find(OffsetIdc);
		if (offsetIdx == wstring::npos) throw nullptr;
		size_t timeSignatureIdx = uniFile.find(TimeSignatureIdc, offsetIdx);
		if (timeSignatureIdx == wstring::npos) throw nullptr;
		size_t patternIdx = uniFile.find(PatternIdc, timeSignatureIdx);
		if (patternIdx == wstring::npos) throw nullptr;
		
		//Offset
		size_t endIdx = uniFile.find(EndlineIdc, offsetIdx);
		wstring_view lineStr(uniFile.c_str() + offsetIdx, endIdx - offsetIdx);
		wstring val;
		ShortCut::WordSeparateW(lineStr, L":", nullptr, &val);


		using namespace chrono;
		double offset = 0;
		wstringstream wss;
		wss << val << endl;
		wss >> offset;
		//offset in text: miliseconds unit, offset in memory: microseconds unit
		MilliDouble offsetMili = MilliDouble(offset);
		microseconds offsetMicro = duration_cast<microseconds>(offsetMili);
		musicScore->offset = offsetMicro;

		//Base BPM
		size_t baseBpmIdx = uniFile.find(BaseBpmIdc, offsetIdx);
		if (baseBpmIdx != wstring::npos) 
		{
			endIdx = uniFile.find(EndlineIdc, baseBpmIdx);
			lineStr = wstring_view(uniFile.c_str() + baseBpmIdx, endIdx - baseBpmIdx);
			ShortCut::WordSeparateW(lineStr, L":", nullptr, &val);
			wss << val << endl;
			wss >> musicScore->baseBpm;
			musicScore->bpms.emplace(MusicBPM(MusicalObject(0, 0), musicScore->baseBpm));
		}

		//TODO: run Load-functions as thread

		endIdx = uniFile.find(EndlineIdc, timeSignatureIdx);
		const wstring_view timeSignatureView(uniFile.c_str() + endIdx + EndlineIdcLength, patternIdx - (endIdx + EndlineIdcLength));
		LoadTimeSignature(timeSignatureView);

		endIdx = uniFile.find(EndlineIdc, patternIdx);
		const wstring_view patternView(uniFile.c_str() + endIdx + EndlineIdcLength);
		LoadPattern(patternView);

		testLane.LoadNotes(musicScore, measurePrefixSum, bpmTimingPrefixSum);

		const Note* firstNote = musicScore->GetFirstNote();
		constexpr chrono::milliseconds waitTime(1000);
		if (firstNote != nullptr)
		{
			firstNoteTiming = Lane::GetNoteTimingPoint(measurePrefixSum, bpmTimingPrefixSum, *firstNote);
			if (firstNoteTiming < waitTime) UpdateMusicTimeOffset(firstNoteTiming - waitTime);
		}

		DEBUG_BREAKPOINT;

	}
	catch (void* p)
	{
		UNREFERENCED_PARAMETER(p);
		delete musicScore;
		musicScore = nullptr;
	}
	LoadMusicScoreComplete();
}

void PlayScene::LoadMusicScoreComplete()
{
	musicScoreLoadFlag = true;
	//if (musicScore->offset < MilliDouble(1));
	timer.Reset();
	rhythmTimer.Reset();

	InitInstancedBuffer();
}

void PlayScene::StopThread()
{
	StopPlayMusicThread();
	StopLoadMusicScoreThread();
}

void PlayScene::BeginScene()
{
	ChangeStatusLoad();
}

void PlayScene::OnResize(float newW, float newH)
{
	//transparentBlackBG.ChangeWidthToCurrentWidth(newW, newH);
	loadingText.GetWorld2d().OnParentWorldUpdate();
	currentTimeText.GetWorld2d().OnParentWorldUpdate();

	testLane.Sprite.OnResize();
	noteSprite.OnResize();

	switch (sceneStatus)
	{
	case PlayScene::Status::Load:
		break;
	case PlayScene::Status::Start:
		break;
	case PlayScene::Status::Pause:
		prevSceneSprite.GetWorld3d().SetObjectScale({ ShortCut::GetOrthoWidth(newW, newH), (float)StandardHeight });
		InitPauseBackground();
		for (int i = 0; i < (int)PauseOption::MAX; ++i)
		{
			PauseOptLayoutList[i].GetWorld2d().OnParentWorldUpdate();
			pauseOptSelectTriangle.GetWorld2d().OnParentWorldUpdate();
		}
		break;
	case PlayScene::Status::End:
		break;
	default:
		break;
	}
}

void PlayScene::ChangeStatusLoad()
{
	sceneStatus = Status::Load;
	SafeDelete(musicScore);
	timer.Reset();
	loadMusicScoreThreadFlag = true;
	loadMusicScoreThread = thread(&PlayScene::LoadMusicScore, this);
}

void PlayScene::ExitStatusLoad()
{
	StopLoadMusicScoreThread();
}

void PlayScene::UpdateOnLoad(float dt)
{
	constexpr float enterWaitTime = 3.0f;
	if (musicScoreLoadFlag)
	{
		timer.Tick();
		if (musicScore == nullptr)
		{
			AlertBox(_T("Pattern Load Fail"));
			ExitStatusLoad();
			ChangeStatusEnd();
		}
		else if (KEYBOARD.Down(VK_RETURN) || timer.TotalTime() > enterWaitTime)
		{
			ExitStatusLoad();
			ChangeStatusReStart();
		}
	}

	if (KEYBOARD.Down(VK_ESCAPE)) ChangeStatusEnd();
}
void PlayScene::RenderOnLoad(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	if (!musicScoreLoadFlag)loadingText.Draw();
	if (musicScoreLoadFlag)loadingCompleteText.Draw();
}

void PlayScene::UpdateTotalMusicTime()
{
	using namespace std::chrono;
	totalMusicTime = MilliDouble(rhythmTimer.TotalTime() * 1000.0f + (double)(duration_cast<milliseconds>(musicTimeOffset).count()));
}

void PlayScene::StopPlayMusicThread()
{
	playMusicThreadRunFlag = false;
	if (playMusicThread.joinable())playMusicThread.join();
}

void PlayScene::PlayMusic()
{
	while (totalMusicTime.count() < 0 && playMusicThreadRunFlag); //busy wait

	music->PlayMusic();
	UpdateTotalMusicTime();
	music->channel->setPosition((unsigned int)totalMusicTime.count(), FMOD_TIMEUNIT_MS);
}

void PlayScene::ChangeStatusReStart()
{
	rhythmTimer.Reset();
	UpdateTotalMusicTime();
	
	playMusicThreadRunFlag = true;
	playMusicThread = thread(&PlayScene::PlayMusic, this);

	sceneStatus = Status::Start;
}
void PlayScene::ChangeStatusStart()
{
	rhythmTimer.Start();
	rhythmTimer.Tick();

	playMusicThreadRunFlag = true;
	playMusicThread = thread(&PlayScene::PlayMusic, this);

	sceneStatus = Status::Start;
}

void PlayScene::ExitStatusStart()
{
	StopPlayMusicThread();
	music->channel->setPaused(true);
}

void PlayScene::UpdateOnStart(float dt)
{
	rhythmTimer.Tick();
	UpdateTotalMusicTime();

	if (KEYBOARD.Down(VK_ESCAPE))
	{
		ExitStatusStart();
		ChangeStatusPause();
	}

	if (KEYBOARD.Hold('Z'))
	{
		noteSprite.GetWorld3d().MoveLocalPosition(0, -400 * dt, 0);
		noteOverlaySprite.GetWorld3d().OnParentWorldUpdate();
		
	}
	
	if (KEYBOARD.Hold('X'))
	{
		noteSprite.GetWorld3d().MoveLocalPosition(0, 400 * dt, 0);
		noteOverlaySprite.GetWorld3d().OnParentWorldUpdate();
		
	}

	if (KEYBOARD.Down('A'))noteSprite.Diffuse = MyColor4::MyRed;
	if (KEYBOARD.Down('S'))noteSprite.Diffuse = MyColor4::MyBlue;
	noteSprite.Diffuse.w = 0.5f;

#ifdef _DEBUG
	if (KEYBOARD.Hold('1'))
	{
		ExitStatusStart();
		ChangeStatusPause();
	}
	else if (KEYBOARD.Hold('2'))
	{
		ExitStatusStart();
		ChangeStatusPause();
	}
#endif
	
}
void PlayScene::RenderOnStart(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	UpdateCurrentTimeText();
	currentTimeText.Draw();
	testLane.Sprite.Render(deviceContext, cam);

	noteSprite.Render(deviceContext, cam);
	noteOverlaySprite.Render(deviceContext, cam);
	
	UpdateInstancedBuffer(testLane);

	Sprite::RenderInstanced(deviceContext, cam, noteInstancedBuffer.Get(), 0, instanceCount * (size_t)SpriteTextureID::MAX, textureList.at(TextureName::note)->GetRefSRV());
}


void PlayScene::ChangeStatusEnd()
{
	sceneStatus = Status::End;
}
void PlayScene::UpdateOnEnd(float dt)
{
	SCENEMANAGER.ChangeScene(SceneManager::Name::SelectMusic);
}
void PlayScene::RenderOnEnd(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
}

void PlayScene::ChangePauseOptionKey(int val)
{
	FMODSYSTEM.Play(FmodSystem::Name::button01a);
	pauseOptionKey = val;
	pauseOptSelectTriangle.GetWorld2d().SetParentWorld(&PauseOptLayoutList[pauseOptionKey].GetWorld2d());
}

void PlayScene::ChangeStatusPause()
{
	rhythmTimer.Stop();
	TRACE(_T("Pause Time: %.6lf\n"), totalMusicTime);
	prevSceneStatus = sceneStatus;
	sceneStatus = Status::Pause;

	InitPauseBackground();
}

void PlayScene::UpdateOnPause(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) ChangeStatusStart();

#ifdef _DEBUG
	if (KEYBOARD.Hold('1')) ChangeStatusStart();
	else if (KEYBOARD.Hold('2')) ChangeStatusReStart(); 
#endif

	if (KEYBOARD.Down(VK_UP))
	{
		if (pauseOptionKey > (int)PauseOption::Resume)
		{
			ChangePauseOptionKey(pauseOptionKey - 1);
		}
		else if (pauseOptionKey == (int)PauseOption::Resume)
		{
			ChangePauseOptionKey((int)PauseOption::Quit);
		}
	}
	if (KEYBOARD.Down(VK_DOWN))
	{
		if (pauseOptionKey < (int)PauseOption::Quit)
		{
			ChangePauseOptionKey(pauseOptionKey + 1);
		}
		else if (pauseOptionKey == (int)PauseOption::Quit)
		{
			ChangePauseOptionKey((int)PauseOption::Resume);
		}
	}

	if (KEYBOARD.Down(VK_RETURN))
	{
		switch (pauseOptionKey)
		{
		case (int)PauseOption::Resume:
			FMODSYSTEM.Play(FmodSystem::Name::select05);
			ChangeStatusStart();
			break;

		case (int)PauseOption::Restart:
			//TODO: Reload Pattern
			FMODSYSTEM.Play(FmodSystem::Name::select05);
			ChangeStatusReStart();
			break;

		case (int)PauseOption::Quit:
			FMODSYSTEM.Play(FmodSystem::Name::select05);
			ChangeStatusEnd();
			break;
		}
	}
}
void PlayScene::RenderOnPause(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	prevSceneSprite.Render(deviceContext, cam);

	for (auto& it : PauseOptLayoutList) it.Draw();
	pauseOptSelectTriangle.Draw();
}

void PlayScene::ChangeStatus(Status s)
{
	TRACE(_T("status %d\n"), (int)s);
	switch (s)
	{
	case PlayScene::Status::Load:
		ChangeStatusLoad();
		break;
	case PlayScene::Status::Start:
		ChangeStatusStart();
		break;
	case PlayScene::Status::Pause:
		ChangeStatusPause();
		break;
	case PlayScene::Status::End:
		ChangeStatusEnd();
		break;
	}
}

void PlayScene::ExitStatus(Status s)
{
	TRACE(_T("Exit Status %d\n"), (int)s);
	switch (s)
	{
	case PlayScene::Status::Load:
		ExitStatusLoad();
		break;
	case PlayScene::Status::Start:
		ExitStatusStart();
		break;
	case PlayScene::Status::Pause:
		break;
	case PlayScene::Status::End:
		break;
	}
}

void PlayScene::RenderStatus(Status s, ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	switch (s)
	{
	case Status::Load:
		RenderOnLoad(deviceContext, cam);
		break;
	case Status::Start:
		RenderOnStart(deviceContext, cam);
		break;
	case Status::Pause:
		RenderOnPause(deviceContext, cam);
		break;
	case Status::End:
		RenderOnEnd(deviceContext, cam);
		break;
	}
}

void PlayScene::Update(float dt)
{
	switch (sceneStatus)
	{
	case Status::Load:
		UpdateOnLoad(dt);
		break;
	case Status::Start:
		UpdateOnStart(dt);
		break;
	case Status::Pause:
		UpdateOnPause(dt);
		break;
	case Status::End:
		UpdateOnEnd(dt);
		break;
	}
}

void PlayScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	RenderStatus(sceneStatus, deviceContext, cam);
}

void PlayScene::EndScene()
{
	SCENEMANAGER.RemoveScene(SceneManager::Name::PlayScene);
}

void PlayScene::InitInstancedBuffer()
{
	noteInstancedBuffer.Reset();
	D3D11_BUFFER_DESC instbd{};
	instbd.Usage = D3D11_USAGE_DYNAMIC;
	instbd.ByteWidth = UINT(sizeof(SpriteInstanceData) * testLane.NoteListConst().size()) * (UINT)SpriteTextureID::MAX;
	instbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//D3D11_SUBRESOURCE_DATA instinitData{};
	//instinitData.pSysMem = &noteInstanceList[0];
	HR(App->GetDevice()->CreateBuffer(&instbd, nullptr, &noteInstancedBuffer));

}

void PlayScene::UpdateInstancedBuffer(Lane& lane)
{
	using namespace chrono;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	App->GetDeviceContext()->Map(noteInstancedBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	D3D11_BUFFER_DESC d;
	noteInstancedBuffer->GetDesc(&d);
	SpriteInstanceData* dataView = reinterpret_cast<SpriteInstanceData*>(mappedData.pData);

	const vector<Lane::NoteDesc>& noteDescList = lane.NoteListConst();

	const MicroDouble& earlyBadTiming 
		= duration_cast<MicroDouble>(totalMusicTime - MilliDouble(accRange.GetAccuracyRange(AccuracyRange::RangeName::Bad)));

	vector<Lane::NoteDesc>::const_reverse_iterator rEndIter
		= upper_bound(noteDescList.rbegin(), noteDescList.rend(),
			microseconds((long long)earlyBadTiming.count()), 
			[](const microseconds& target, const Lane::NoteDesc& s) {
				return std::greater<microseconds>()(target, s.timing);
			});
	
	vector<Lane::NoteDesc>::const_reverse_iterator rcIter = noteDescList.rbegin();

	constexpr pair<double, double> drawArea = { -256.0, (double)LaneMaxLength };
	constexpr milliseconds quarterBeatOfBpm60(15000);
	constexpr double distanceQuarterRhythm = CircleDiameter * 0.92;
	const double msQuarterInv = musicScore->baseBpm / (double)quarterBeatOfBpm60.count();

	instanceCount = 0;
	while (rcIter != rEndIter)
	{
		const microseconds& noteTiming = (*rcIter).timing;
		const double noteRelativeTiming = (double)(duration_cast<milliseconds>(noteTiming).count()) - totalMusicTime.count();
		const double notePos = distanceQuarterRhythm * noteRelativeTiming * msQuarterInv;

		SpriteInstanceData tempInst;

		World3D tempWorld3d;
		const Lane::NoteDrawDesc& tempNoteDrawDesc = lane.GetNoteDrawDesc(rcIter->note->noteType);

		tempWorld3d.SetParentWorld(&lane.Sprite.GetWorld3d());
		tempWorld3d.SetObjectScale(tempNoteDrawDesc.diameter);
		tempWorld3d.SetLocalPosition({ 0, (float)notePos, 0 });

		tempInst.Diffuse = tempNoteDrawDesc.color;
		tempInst.uvworld = tempWorld3d.GetUvWorld();
		tempInst.world = tempWorld3d.GetTotalDrawWorld();
		tempInst.TextureID = (UINT)SpriteTextureID::Note;
		dataView[2 * instanceCount] = tempInst;

		tempInst.Diffuse = MyColor4::White;
		tempInst.TextureID = (UINT)SpriteTextureID::NoteOverlay;
		dataView[2 * instanceCount + 1] = tempInst;
		DEBUG_BREAKPOINT;

		++instanceCount;
		++rcIter;
	}

	App->GetDeviceContext()->Unmap(noteInstancedBuffer.Get(), 0);
}
