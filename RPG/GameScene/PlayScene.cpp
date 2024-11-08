#include "framework.h"
#include "PlayScene.h"
#include "ResultScene.h"

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

constexpr double JudgeLinePosition = 80.0;

constexpr UINT REFTIME_DEBUG = FALSE;

PlayScene::PlayScene(Music* m, Pattern* p) :
	music(m), pattern(p),
	prevSceneSprite(0, 0, (float)StandardWidth, (float)StandardHeight, { 1,1,1,0.2f }, false), resultWaitFlag(false)
	//,noteSprite(0, 0)
{
	loadWaitTimer.Reset();

	InitPauseOptionLayoutList();
	InitLoadingText();
	InitCurrentTimeText();
	InitDebugText();

	InitTaikoModeTextures();
	InitTaikoModeLanes();
	InitTaikoModeSprites();
	InitTaikoModeKeyNoteTypeMap();
	InitTaikoModeHitSounds();
}

PlayScene::~PlayScene()
{
	StopThread();
	SafeDelete(musicScore);
	ReleaseTaikoModeTextures();
	ReleasePauseBackground();
	ReleaseTaikoModeHitSounds();

	ReleaseCOM(noteInstancedBuffer);
	ReleaseCOM(measureLineInstancedBuffer);
}


void PlayScene::InitTaikoModeLanes()
{
	testLane.AddNoteType(1);
	testLane.AddNoteType(2);
	testLane.AddNoteType(3);
	testLane.AddNoteType(4);

	testLane.AddNoteDrawDesc(1, { MyColor4::MyRed, CircleDiameter, (UINT)NoteTextureArrID::Note, (UINT)NoteTextureArrID::NoteOverlay });
	testLane.AddNoteDrawDesc(2, { MyColor4::MyBlue, CircleDiameter ,(UINT)NoteTextureArrID::Note, (UINT)NoteTextureArrID::NoteOverlay });
	testLane.AddNoteDrawDesc(3, { MyColor4::MyRed, LargeCircleDiameter, (UINT)NoteTextureArrID::BigNote, (UINT)NoteTextureArrID::NoteOverlay });
	testLane.AddNoteDrawDesc(4, { MyColor4::MyBlue, LargeCircleDiameter, (UINT)NoteTextureArrID::BigNote, (UINT)NoteTextureArrID::NoteOverlay });

}

void PlayScene::InitTaikoModeKeyNoteTypeMap()
{
	//TODO: load from config file
	ReleaseTaikoModeKeyNoteTypeMap();

	keyNoteTypeMap[LeftD].emplace_back(StrToVK::GetVK("Z"));
	keyNoteTypeMap[LeftD].emplace_back(StrToVK::GetVK("X"));
	keyNoteTypeMap[RightD].emplace_back(StrToVK::GetVK("VK_OEM_2"));
	keyNoteTypeMap[RightD].emplace_back(StrToVK::GetVK("VK_OEM_PERIOD"));
	keyNoteTypeMap[LeftK].emplace_back(StrToVK::GetVK("A"));
	keyNoteTypeMap[LeftK].emplace_back(StrToVK::GetVK("S"));
	keyNoteTypeMap[RightK].emplace_back(StrToVK::GetVK("VK_OEM_7"));
	keyNoteTypeMap[RightK].emplace_back(StrToVK::GetVK("VK_OEM_1"));
}

void PlayScene::ReleaseTaikoModeKeyNoteTypeMap()
{
	keyNoteTypeMap.clear();
}

void PlayScene::InitTaikoModeHitSounds()
{
	FMOD::Sound* tempSound = nullptr;
	const wstring skinDir = SkinDir + L"test Skin/";

	FMODSYSTEM.System()->createStream(ShortCut::WstringToUTF8((skinDir +L"don.wav")).c_str(), FMOD_LOOP_OFF | FMOD_CREATESAMPLE, nullptr, &tempSound);
	defaultTaikoModeHitSoundList.insert(make_pair((size_t)DefaultHitSound::Don, tempSound));
	FMODSYSTEM.System()->createStream(ShortCut::WstringToUTF8((skinDir + L"kat.wav")).c_str(), FMOD_LOOP_OFF | FMOD_CREATESAMPLE, nullptr, &tempSound);
	defaultTaikoModeHitSoundList.insert(make_pair((size_t)DefaultHitSound::Kat, tempSound));
	FMODSYSTEM.System()->createStream(ShortCut::WstringToUTF8((skinDir + L"bigdon.wav")).c_str(), FMOD_LOOP_OFF | FMOD_CREATESAMPLE, nullptr, &tempSound);
	defaultTaikoModeHitSoundList.insert(make_pair((size_t)DefaultHitSound::BigDon, tempSound));
	FMODSYSTEM.System()->createStream(ShortCut::WstringToUTF8((skinDir + L"bigkat.wav")).c_str(), FMOD_LOOP_OFF | FMOD_CREATESAMPLE, nullptr, &tempSound);
	defaultTaikoModeHitSoundList.insert(make_pair((size_t)DefaultHitSound::BigKat, tempSound));
}

void PlayScene::ReleaseTaikoModeHitSounds()
{
	for (auto& it : defaultTaikoModeHitSoundList)
	{
		it.second->release();
	}
}

void PlayScene::InitTaikoModeTextures()
{
	ReleaseTaikoModeTextures();
	const wstring skinDir = SkinDir + L"test Skin/";
	
	textureList.AddTexture(App->GetDevice(), TextureName::LaneBackground, skinDir + L"LaneBackground.png");
	textureList.AddTexture(App->GetDevice(), TextureName::LaneLight, skinDir + L"LaneLight.png");
	textureList.AddTexture(App->GetDevice(), TextureName::JudgeLine, skinDir + L"JudgeLine.png");
	textureList.AddTexture(App->GetDevice(), TextureName::MeasureLine, skinDir + L"MeasureLine.png");
	
	bool val = textureList.AddTextureArr(
		TextureName::note,
		{
			(skinDir + L"note.png").c_str(),
			(skinDir + L"bignote.png").c_str(),
			(skinDir + L"noteoverlay.png").c_str()
		});
	assert(val);
	
}

void PlayScene::ReleaseTaikoModeTextures()
{
	textureList.Clear();
}

void PlayScene::InitTaikoModeSprites()
{
	//TODO: move this function to Lane class

	const Texture& laneBGTexture = textureList.GetTexture(TextureName::LaneBackground);
	testLane.laneSprite.GetWorld3d().SetObjectScale({ LaneWidth, (float)LaneMaxLength });
	testLane.laneSprite.GetWorld3d().SetCenterPosition({ 0, 0.5f, 0 });
	testLane.laneSprite.GetWorld3d().SetLocalRotation({ 0, 0, -Math::PI * 0.5f });
	testLane.laneSprite.GetWorld3d().SetLocalPosition({ LaneWidth,100,0 });
	testLane.laneSprite.GetWorld3d().SetAlignX(AlignModeX::Left);
	testLane.laneSprite.GetWorld3d().SetParentDrawWorld();
	testLane.laneSprite.SetTexture(&laneBGTexture);
	testLane.laneSprite.RepeatTextureInExtraArea(laneBGTexture.width, laneBGTexture.height);
	testLane.SetJudgePosition(JudgeLinePosition);

	const Texture& laneLightTexture = textureList.GetTexture(TextureName::LaneLight);
	testLane.laneLightSprite.GetWorld3d().SetObjectScale({ LaneWidth,laneLightTexture.height * LaneWidth / laneLightTexture.width });
	testLane.laneLightSprite.Diffuse.w = 0;
	testLane.laneLightSprite.GetWorld3d().SetCenterPosition({ 0, 0.5f, 0 });
	testLane.laneLightSprite.GetWorld3d().SetParentWorld(&testLane.laneSprite.GetWorld3d());
	testLane.laneLightSprite.SetTexture(&laneLightTexture);

	const Texture& judgeLineTexture = textureList.GetTexture(TextureName::JudgeLine);
	testLane.judgeLineSprite.GetWorld3d().SetParentWorld(&testLane.laneSprite.GetWorld3d());
	testLane.judgeLineSprite.GetWorld3d().SetLocalPosition({ 0, (float)testLane.GetJudgePosition(), 0 });
	testLane.judgeLineSprite.GetWorld3d().SetObjectScale(LargeCircleDiameter);
	testLane.judgeLineSprite.SetTexture(&judgeLineTexture);
	
}

void PlayScene::InitCurrentTimeText()
{
	currentTimeText.Color = MyColorF::GhostGreen;
	currentTimeText.GetWorld2d().SetAlignX(AlignModeX::Left);
	currentTimeText.GetWorld2d().SetPosition({ 0,0 });

	IDWriteTextFormat*& currentFont = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	currentTimeText.SetTextFormat(currentFont);

	UpdateCurrentTimeText();
}

void PlayScene::UpdateCurrentTimeText()
{
	using namespace chrono;
	WCHAR tempStr[50];
	swprintf_s(tempStr, L"Time: %.3fs", duration_cast<duration<double, ratio<1, 1>>>(totalMusicTime).count());
	currentTimeText.SetText(tempStr);
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

void PlayScene::InitDebugText()
{
	debugText.GetWorld2d().SetAlignX(AlignModeX::Left);
	debugText.GetWorld2d().SetPosition({ 0,20 });
	debugText.Color = MyColorF::GhostGreen;
	debugText.SetLayout(L"");
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


	ID3D11Texture2D* tempPauseBG = nullptr;
	HR(App->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &tempPauseBG));
	HR(App->GetDevice()->CreateRenderTargetView(tempPauseBG, nullptr, &pauseBgRTV));
	HR(App->GetDevice()->CreateShaderResourceView(tempPauseBG, nullptr, &pauseBgTexture.GetRefSRV()));

	D2D.ReleaseBackBuffer(); //for change 3d target, 2d surfaces must be relased
	D2D.ResetBackBuffer(tempPauseBG);

	App->GetDeviceContext()->OMSetRenderTargets(1, &pauseBgRTV, nullptr);

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	App->GetDeviceContext()->ClearRenderTargetView(pauseBgRTV, (const float*)&App->GetBgColor());

	D2D.BeginDraw();
	RenderStatus(prevSceneStatus, App->GetDeviceContext(), App->GetCamera());
	D2D.EndDraw();

	//D3DX11SaveTextureToFile(App->GetDeviceContext(), pauseBG, D3DX11_IFF_PNG, L"asdf.png");
	prevSceneSprite.SetTexture(&pauseBgTexture);

	ReleaseCOM(tempPauseBG);
	App->ResetRenderTarget();
	D2D.ReleaseBackBuffer();
	D2D.ResetBackBufferFromSwapChain(App->GetSwapChain());
}

void PlayScene::ReleasePauseBackground()
{
	ReleaseCOM(pauseBgRTV);
	ReleaseCOM(pauseBgTexture.GetRefSRV());
}

void PlayScene::ReSetting()
{
	rhythmTimer.Reset();
	UpdateTotalMusicTime();
	testLane.Reset();
	scorePercent.Init();
	resultWaitFlag = false;
}

static constexpr LPCWSTR OffsetIdc = L"Pattern Offset";
static constexpr LPCWSTR TimeSignatureIdc = L"[Time Signature]";
static constexpr LPCWSTR PatternIdc = L"[Pattern]";
static constexpr LPCWSTR EndlineIdc = L"\r\n";
static constexpr LPCWSTR BarlineIdc = L"--";
static constexpr LPCWSTR EffectTypeIdc = L"#";
static constexpr LPCWSTR measureIdc = L"measure";
static constexpr LPCWSTR measureLineVisible = L"measureLineVisible";
static constexpr LPCWSTR BaseBpmIdc = L"Base BPM";
static constexpr LPCWSTR bpmIdc = L"bpm";
static constexpr LPCWSTR CommonTimeIdc = L"C";

static constexpr size_t EndlineIdcLength = 2;

const wstring MeasureEffect = wstring(EffectTypeIdc) + measureIdc;
const wstring MeasureLineVisibleEffect = wstring(EffectTypeIdc) + measureLineVisible;
const wstring BpmEffect = wstring(EffectTypeIdc) + bpmIdc;

void PlayScene::ParseBarLine(const wstring_view& lineStr, const Measure& measureRef, size_t& measureIdx)
{
	musicScore->measures.emplace_back(measureRef);
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
	bool isReadingFile = true;
	bool lineVisible = true;
	const auto& ReadTimeSignatureLine = [&](const wstring_view& lineStr)
		{
			// in case bar line
			if (lineStr.compare(BarlineIdc) == 0) //--
			{
				ParseBarLine(lineStr, { measureLength, lineVisible }, measureIdx);
				return;
			}

			// Measure
			if (lineStr.find(MeasureEffect + L' ') != wstring_view::npos) //#measure n/d
			{
				ParseMeasure(lineStr, measureLength);
				return;
			}

			// Measure
			if (lineStr.find(MeasureLineVisibleEffect + L' ') != wstring_view::npos) //#measureLineVisible ON/OFF
			{
				const wstring_view effectType = lineStr.substr(MeasureLineVisibleEffect.length() + 1);
				constexpr LPCWSTR effectON = L"ON";
				constexpr LPCWSTR effectOFF = L"OFF";
				if (effectType.find(effectON) != wstring_view::npos) lineVisible = true;
				else if (effectType.find(effectOFF) != wstring_view::npos) lineVisible = false;
				return;
			}

			wstring effectStr;
			RationalNumber<64> signature;

			validCheck = ParseEffect(lineStr, signature, effectStr);
			if (!validCheck) return;

			//get type
			wstring effectType;
			wstring effectValue;
			validCheck = ShortCut::WordSeparateW(effectStr, L" ", &effectType, &effectValue);
			if (!validCheck) return;

			if (effectType.compare(BpmEffect) == 0) //#bpm xxx
			{
				ParseBPM(effectValue, measureIdx, signature);
			}
		};

	while(isReadingFile)
	{
		const wstring_view& lineStr = content.substr(startIdx, endIdx - startIdx);
		
		ReadTimeSignatureLine(lineStr);
		isReadingFile = GetNextLineIdxPair(content, EndlineIdc, startIdx, endIdx);
	}

	musicScore->measures.emplace_back(measureLength,lineVisible);

	musicScore->InitTimeSignaturePrefixSum();

}
void PlayScene::LoadPattern(const wstring_view& content)
{
	size_t startIdx = 0;
	size_t endIdx = content.find(EndlineIdc, startIdx);

	bool isReadingFile = true;
	size_t measureIdx = 0;

	bool isAnnotationArea = false;
	const auto& ReadPatternLine = [&](const wstring_view& lineStr)
		{
			//annotation check
			if (lineStr.substr(0, 2) == L"//") return;
			if (lineStr.substr(0, 2) == L"/*")
			{
				isAnnotationArea = true;
				return;
			}
			if (lineStr.substr(0, 2) == L"*/")
			{
				isAnnotationArea = false;
				return;
			}
			if (isAnnotationArea) return;

			// in case bar line
			if (lineStr.compare(BarlineIdc) == 0) //--
			{
				++measureIdx;
				return;
			}

			//note (N/D,Type,Action,Hitsound(Opt),ExtraData(Opt))
			vector<pair<size_t, size_t>> idxArr;
			ShortCut::WordSeparateW(lineStr, L",", idxArr);

			if (idxArr.size() < (size_t)MusicalNote::DataOrder::Action + 1) //invalid string
			{
				return;
			}

			wstring_view noteElements[(int)MusicalNote::DataOrder::MAX];
			for (int idx = 0; idx < idxArr.size(); ++idx)
			{
				if (idxArr[idx].second != wstring_view::npos)
					noteElements[idx] = wstring_view(lineStr.substr(idxArr[idx].first, idxArr[idx].second - idxArr[idx].first));
				else
					noteElements[idx] = wstring_view(lineStr.substr(idxArr[idx].first));
			}

			wstringstream wss;
			MusicalNote tempNote;
			tempNote.mp.measureIdx = measureIdx;

			//DataOrder::Beat
			tempNote.mp.position = ShortCut::StrToRationalNumber<64>(noteElements[(int)MusicalNote::DataOrder::Rhythm]);
			size_t referencingMeasureIdx = min(measureIdx, musicScore->measures.size() - 1);

			//out of measure
			if (tempNote.mp.position >= musicScore->measures[referencingMeasureIdx].length) return;

			//DataOrder::Type
			wss << noteElements[(int)MusicalNote::DataOrder::Type];
			wss >> tempNote.noteType;

			//DataOrder:Action
			wss << noteElements[(int)MusicalNote::DataOrder::Action];
			wss >> tempNote.actionType;

			//DataOrder::Hitsound
			if (idxArr.size() > (int)MusicalNote::DataOrder::Hitsound)
			{
				tempNote.hitSound = noteElements[(int)MusicalNote::DataOrder::Hitsound];
			}

			wstring asdfdsaf;

			if (idxArr.size() > (int)MusicalNote::DataOrder::ExtraData)
			{
				tempNote.extraData = noteElements[(int)MusicalNote::DataOrder::ExtraData];
			}

			musicScore->notesPerTypeMap[tempNote.noteType].emplace(make_pair(tempNote.mp, tempNote));
		};

	while (isReadingFile)
	{
		const wstring_view& lineStr = content.substr(startIdx, endIdx - startIdx);
		
		ReadPatternLine(lineStr);
		isReadingFile = GetNextLineIdxPair(content, EndlineIdc, startIdx, endIdx);
	}
}
//returns false if it's list line
bool PlayScene::GetNextLineIdxPair(const wstring_view& content, const wstring_view& endStr, size_t& startIdx, size_t& endIdx)
{
	if (endIdx == wstring::npos) return false; //last line
	startIdx = endIdx + endStr.length();
	endIdx = content.find(endStr, startIdx);
	return true;
}

void PlayScene::StopLoadMusicScoreThread()
{
	loadMusicScoreThreadFlag = false;
	if (loadMusicScoreThread.joinable()) loadMusicScoreThread.join();
}

void PlayScene::LoadMusicScore()
{
	musicScoreLoadFlag = false;
	musicScore = new MusicScore;
	const wstring& fileName = pattern->ympFileName;

	wstring uniFile = ShortCut::ReadUTF8File(fileName);

	const auto& LoadMusicScoreFromFile = [&]()-> bool
		{
			//file contents check
			size_t offsetIdx = uniFile.find(OffsetIdc);
			if (offsetIdx == wstring::npos) return false;

			size_t timeSignatureIdx = uniFile.find(TimeSignatureIdc, offsetIdx);
			if (timeSignatureIdx == wstring::npos) return false;

			size_t patternIdx = uniFile.find(PatternIdc, timeSignatureIdx);
			if (patternIdx == wstring::npos) return false;

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

			testLane.LoadNotes(musicScore);

			const MusicalNote* firstNote = musicScore->GetFirstNote();
			constexpr chrono::milliseconds waitTime(1000);
			if (firstNote != nullptr)
			{
				firstNoteTiming = musicScore->GetNoteTimingPoint(firstNote->mp);
				if (firstNoteTiming < waitTime) musicTimeOffset = (firstNoteTiming - waitTime);
			}

			DEBUG_BREAKPOINT;

			return true;
		};

	if (LoadMusicScoreFromFile() == false) SafeDelete(musicScore);

	LoadMusicScoreComplete();

}

void PlayScene::LoadMusicScoreComplete()
{
	musicScoreLoadFlag = true;
	loadWaitTimer.Reset();

	InitInstancedBuffer(noteInstancedBuffer, UINT(sizeof(SpriteInstanceData) * testLane.NoteListConst().size()) * (UINT)NoteTextureInstanceID::MAX);

	const size_t& maximumMeasureLineCount = testLane.NoteListConst().back().NoteRef()->mp.measureIdx + 1;
	InitInstancedBuffer(measureLineInstancedBuffer, UINT(sizeof(SpriteInstanceData) * maximumMeasureLineCount));
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
	prevSceneSprite.GetWorld3d().SetObjectScale({ ShortCut::GetOrthoWidth(newW, newH), (float)StandardHeight });

	for (int i = 0; i < (int)PauseOption::MAX; ++i)
	{
		PauseOptLayoutList[i].GetWorld2d().OnParentWorldUpdate();
		pauseOptSelectTriangle.GetWorld2d().OnParentWorldUpdate();
	}

	loadingText.GetWorld2d().OnParentWorldUpdate();
	currentTimeText.GetWorld2d().OnParentWorldUpdate();
	debugText.GetWorld2d().OnParentWorldUpdate();

	testLane.OnResize();

	//noteSprite.OnResize();
	//noteOverlaySprite.OnResize();

	switch (sceneStatus)
	{
	case PlayScene::Status::Load:
		break;
	case PlayScene::Status::Start:
		break;
	case PlayScene::Status::Pause:
		InitPauseBackground();
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
	loadWaitTimer.Reset();
	loadMusicScoreThreadFlag = true;
	loadMusicScoreThread = thread(&PlayScene::LoadMusicScore, this);

	StopLoadMusicScoreThread();

}

void PlayScene::ExitStatusLoad()
{
	StopLoadMusicScoreThread();
	rhythmTimer.Reset();
}

void PlayScene::UpdateOnLoad(float dt)
{
	constexpr float enterWaitTime = 3.0f;
	if (musicScoreLoadFlag)
	{
		loadWaitTimer.Tick();
		if (musicScore == nullptr)
		{
			AlertBox(_T("Pattern Load Fail"));
			ExitStatusLoad();
			ChangeStatusEnd();
		}
		else if (KEYBOARD.Down(VK_RETURN) || loadWaitTimer.TotalTime() > enterWaitTime)
		{
			ExitStatusLoad();
			ChangeStatusStart();
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
	if (playMusicThread.joinable()) playMusicThread.join();
	//music->PlayMusic(true);
}

void PlayScene::PlayMusic()
{
	while (totalMusicTime.count() < 0 && playMusicThreadRunFlag)
	{
		(std::this_thread::sleep_for(std::chrono::milliseconds(1)));
	}

	if (totalMusicTime < music->GetMusicLength())
	{
		music->PlayMusic();
		music->channel->setPosition((unsigned int)totalMusicTime.count(), FMOD_TIMEUNIT_MS);
	}
}

void PlayScene::ChangeStatusReStart()
{
	ReSetting();
	
	playMusicThreadRunFlag = true;
	playMusicThread = thread(&PlayScene::PlayMusic, this);

	sceneStatus = Status::Start;
}
void PlayScene::ChangeStatusStart()
{
	rhythmTimer.Start();
	rhythmTimer.Tick();
	UpdateTotalMusicTime();

	playMusicThreadRunFlag = true;
	playMusicThread = thread(&PlayScene::PlayMusic, this);

	sceneStatus = Status::Start;
}

void PlayScene::ExitStatusStart()
{
	StopPlayMusicThread();
	music->channel->setPaused(true);
}

void PlayScene::UpdateDebugText()
{
	if (testLane.CurrentNoteConst() == testLane.NoteList().cend())
	{
		debugText.SetText(L"No Target Note");
		return;
	}

	const MusicalNote& note = *testLane.CurrentNoteConst()->NoteRef();
	wstringstream wss;
	wss << L"Measure: " << note.mp.measureIdx << "  Pos: " << note.mp.position.Numerator() << "/" << note.mp.position.Denominator()
		<< L" Type: " << note.noteType << L" HitCount: ";
		
	const auto& hitCountOpt = testLane.CurrentNoteConst()->GetHitCondition()->GetHitCount();
	if (hitCountOpt.has_value()) wss << hitCountOpt.value();
	else wss << L"No";
	wss << L" diff: " << differenceFromTime.count()
		<< L"\nScore:" << std::fixed << std::setprecision(2) << scorePercent.GetRate();
	debugText.SetText(wss.str());
}

void PlayScene::UpdateOnStart(float dt)
{
	if (testLane.CurrentNoteConst() == testLane.NoteList().cend())
	{
		constexpr float waitTime = 5.0f;
		if (!resultWaitFlag)
		{
			resultWaitTimer.Reset();
			resultWaitFlag = true;
		}
		resultWaitTimer.Tick();
		music->channel->setVolume(1.0f - (resultWaitTimer.TotalTime() / waitTime));
		if (KEYBOARD.Down(VK_ESCAPE) || resultWaitTimer.TotalTime() > waitTime)
		{
			music->PlayMusic(true);
			SCENEMANAGER.AddScene(SceneManager::Name::ResultScene, new ResultScene());
			SCENEMANAGER.ChangeScene(SceneManager::Name::ResultScene);
			return;
		}
	}

	rhythmTimer.Tick();
	UpdateTotalMusicTime();

	if (KEYBOARD.Down(VK_ESCAPE))
	{
		ExitStatusStart();
		ChangeStatusPause();
	}
	

#ifdef _DEBUG

	static double debugMsSpeed = 1000.0;
	if (KEYBOARD.Hold('1'))
	{
		//noteSprite.GetWorld3d().MoveLocalPosition(0, -400 * dt, 0);
		//noteOverlaySprite.GetWorld3d().OnParentWorldUpdate();
		debugMs = MilliDouble(debugMs.count() - debugMsSpeed * dt);
		//noteSprite.Diffuse = MyColor4::MyRed;
		//noteSprite.Diffuse.w = 0.5f;
	}

	if (KEYBOARD.Hold('2'))
	{
		//noteSprite.GetWorld3d().MoveLocalPosition(0, 400 * dt, 0);
		//noteOverlaySprite.GetWorld3d().OnParentWorldUpdate();
		//noteSprite.Diffuse = MyColor4::MyBlue;
		//noteSprite.Diffuse.w = 0.5f;
		debugMs = MilliDouble(debugMs.count() + debugMsSpeed * dt);
	}

	if (KEYBOARD.Down('3'))
	{
		//noteSprite.GetWorld3d().MoveLocalPosition(0, -400 * dt, 0);
		//noteOverlaySprite.GetWorld3d().OnParentWorldUpdate();
		debugMs = MilliDouble(debugMs.count() - 1.0); 
	}

	if (KEYBOARD.Down('4'))
	{
		//noteSprite.GetWorld3d().MoveLocalPosition(0, 400 * dt, 0);
		//noteOverlaySprite.GetWorld3d().OnParentWorldUpdate();
		debugMs = MilliDouble(debugMs.count() + 1.0);
	}

	if (KEYBOARD.Hold(VK_OEM_MINUS))debugMsSpeed = max(200, debugMsSpeed - 4000.0 * dt);
	if (KEYBOARD.Hold(VK_OEM_PLUS))debugMsSpeed = min(4000, debugMsSpeed + 4000.0 * dt);

	if (KEYBOARD.Hold(VK_F1))
	{
		ExitStatusStart();
		ChangeStatusPause();
	}
	else if (KEYBOARD.Hold(VK_F1))
	{
		ExitStatusStart();
		ChangeStatusPause();
	}
#endif

	testLane.Update(dt);

	MilliDouble refTime;
	if constexpr (REFTIME_DEBUG == 0) refTime = totalMusicTime;
	else refTime = debugMs;

	NoteUpdateTaikoMode(refTime);

}

void PlayScene::NoteUpdateTaikoMode(const MilliDouble& refTime)
{
	MoveTargetNote(refTime, AccuracyRange::RangeName::Good);

	constexpr UINT donList[] = {(UINT)TaikoNoteType::Don, (UINT)TaikoNoteType::BigDon};
	constexpr UINT katList[] = {(UINT)TaikoNoteType::Kat, (UINT)TaikoNoteType::BigKat};

	for (const auto& key : keyNoteTypeMap.at(LeftD))
	{
		if (KEYBOARD.Down(key))
		{
			PlayTaikoModeDonSound(refTime);
			NoteProcessTaikoMode(refTime, donList);
		
		}
	}
	for (const auto& key : keyNoteTypeMap.at(RightD))
	{
		if (KEYBOARD.Down(key))
		{
			PlayTaikoModeDonSound(refTime);
			NoteProcessTaikoMode(refTime, donList);
			
		}
	}
	for (const auto& key : keyNoteTypeMap.at(LeftK))
	{
		if (KEYBOARD.Down(key))
		{
			PlayTaikoModeKatSound(refTime);
			NoteProcessTaikoMode(refTime, katList);
	
		}
	}
	for (const auto& key : keyNoteTypeMap.at(RightK))
	{
		if (KEYBOARD.Down(key))
		{
			PlayTaikoModeKatSound(refTime);
			NoteProcessTaikoMode(refTime, katList);
		
		}
	}

	UpdateDebugText();
}

void PlayScene::NoteProcessTaikoMode(const MilliDouble& refTime, const std::span<const UINT>& targetTypeList)
{
	testLane.laneLightSprite.Diffuse = MyColor4::White; //default LaneLight Color

	if (testLane.CurrentNote() == testLane.NoteListConst().end()) return;
	NoteDesc& currentNote = *testLane.CurrentNote();

	const AccuracyRange::Info* const& range = accRange.GetRangeInfo(refTime, currentNote.Timing());
	if (range == nullptr) return; //out of range

	const AccuracyRange::ScoreInfo& sInfo = range->GetInterpolatedScoreInfo(refTime, currentNote.Timing());
	testLane.laneLightSprite.Diffuse = sInfo.color;

	if (range->id > AccuracyRange::RangeName::Good) //BAD JUDGE
	{
		currentNote.IsInaccurate() = true;
		return;
	}

	if (!CheckNoteType(targetTypeList)) // wrong note type
	{
		testLane.laneLightSprite.Diffuse = MyColor4::MyRed;
		currentNote.IsInaccurate() = true;
		return;
	}

	// Hit successed, MAX ~ GOOD Judge
	constexpr double inaccuratePenalty = 0.5;
	const double resultPercentage = currentNote.IsInaccurateConst() ? sInfo.percentage * inaccuratePenalty : sInfo.percentage;
	scorePercent.AddScoreRate(resultPercentage);
	currentNote.GetHitCondition()->OnHit();
	if (currentNote.GetHitCondition()->IsHitted()) testLane.MoveCurrentNoteForward();

	
}

bool PlayScene::CheckNoteType(const std::span<const UINT>& targetTypeList)
{
	if (testLane.CurrentNote() == testLane.NoteListConst().end()) return false;
	const NoteDesc& currentNote = *testLane.CurrentNote();
	
	bool isFind = false;
	for (const UINT& it : targetTypeList)
	{
		if (it == currentNote.NoteRef()->noteType)
		{
			isFind = true;
			break;
		}
	}
	return isFind;
}

void PlayScene::MoveTargetNote(const MilliDouble refTime, const AccuracyRange::RangeName& judgepriority)
{
	const vector<NoteDesc>::const_iterator& currentNote = testLane.CurrentNoteConst();
	if (currentNote == testLane.NoteListConst().cend()) return;

	using namespace chrono;

	const vector<NoteDesc>& noteDescList = testLane.NoteListConst();

	const microseconds noteWholeJudgeAreaMs = microseconds((microseconds::rep)duration_cast<MicroDouble>
		(accRange.GetEarlyJudgeTiming(refTime, AccuracyRange::RangeName::MAX)).count());

	const microseconds earlyJudgeTimeUs = microseconds((microseconds::rep)duration_cast<MicroDouble>
		(accRange.GetEarlyJudgeTiming(refTime, judgepriority)).count());

	const microseconds lateJudgeTimeUs = microseconds((microseconds::rep)duration_cast<MicroDouble>
		(accRange.GetLateJudgeTiming(refTime, judgepriority)).count());

	vector<NoteDesc>::const_iterator targetNoteIter;
	//FindTargetNoteIter
	[&]()
		{
			const vector<NoteDesc>::const_iterator& targetNotePriorityIter
				= lower_bound
				(
					testLane.NoteList().begin(),
					testLane.NoteList().end(),
					earlyJudgeTimeUs,
					NoteDesc::CompareLowerBound<microseconds, std::less<>>
				);

			if (targetNotePriorityIter != testLane.NoteList().end())
			{
				if (targetNotePriorityIter->Timing() <= lateJudgeTimeUs)
				{
					targetNoteIter = targetNotePriorityIter;
					return;
				}
			}

			const vector<NoteDesc>::const_iterator& targetNoteWholeAreaIter
				= lower_bound
				(
					testLane.NoteList().begin(),
					testLane.NoteList().end(),
					noteWholeJudgeAreaMs,
					NoteDesc::CompareLowerBound<microseconds, std::less<>>
				);
			targetNoteIter = targetNoteWholeAreaIter;
			

		}();

	//MoveCurrentNoteIter
	while (currentNote < targetNoteIter)
	{
		//note Miss
		[&]() {
			scorePercent.AddScoreRate(0.0);
			testLane.MoveCurrentNoteForward();
			}();
	}

#ifdef _DEBUG
	if (testLane.CurrentNoteConst() != testLane.NoteList().cend())
	{
		differenceFromTime = milliseconds(milliseconds::rep(refTime.count())) - duration_cast<milliseconds>(currentNote->Timing());
	}
#endif
}

bool PlayScene::CheckNoteTypeForHitSound(const MilliDouble& refTime, UINT targetType, int targetHitCount)
{
	if (testLane.CurrentNoteConst() == testLane.NoteListConst().cend()) return false;

	const NoteDesc& currentNote = *testLane.CurrentNoteConst();

	//not bignote
	if (currentNote.NoteRef()->noteType != targetType ||
		currentNote.GetHitCondition()->GetHitCount().value() != targetHitCount) return false;

	//not in the judge range
	if (accRange.RangeCheck(refTime, currentNote.Timing(), AccuracyRange::RangeName::Good) == false) return false;

	return true;
}

void PlayScene::PlayTaikoModeDonSound(const MilliDouble& refTime)
{
	const bool& isBigNote = CheckNoteTypeForHitSound(refTime, (UINT)TaikoNoteType::BigDon, (int)HitCount::BigNote);
	if (isBigNote) FMODSYSTEM.System()->playSound(defaultTaikoModeHitSoundList.at((size_t)DefaultHitSound::BigDon), nullptr, false, nullptr);
	else FMODSYSTEM.System()->playSound(defaultTaikoModeHitSoundList.at((size_t)DefaultHitSound::Don), nullptr, false, nullptr);
}

void PlayScene::PlayTaikoModeKatSound(const MilliDouble& refTime)
{
	const bool& isBigNote = CheckNoteTypeForHitSound(refTime, (UINT)TaikoNoteType::BigKat, (int)HitCount::BigNote);
	if (isBigNote) FMODSYSTEM.System()->playSound(defaultTaikoModeHitSoundList.at((size_t)DefaultHitSound::BigKat), nullptr, false, nullptr);
	else FMODSYSTEM.System()->playSound(defaultTaikoModeHitSoundList.at((size_t)DefaultHitSound::Kat), nullptr, false, nullptr);
}

void PlayScene::RenderOnStart(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	UpdateCurrentTimeText();
	currentTimeText.Draw();
	debugText.Draw();

	testLane.Render(deviceContext, cam);

	//noteSprite.Render(deviceContext, cam);
	//noteOverlaySprite.Render(deviceContext, cam);

#ifdef REFTIME_DEBUG
	const MilliDouble& refTime = debugMs;
#else
	const MilliDouble& refTime = totalMusicTime;
#endif 

	UpdateInstancedBuffer_MeasureLine(measureLineInstancedBuffer, musicScore->measures, refTime, testLane);
	Sprite::RenderInstanced(deviceContext, cam, measureLineInstancedBuffer, 0, measureLineInstanceCount, textureList.GetTexture(TextureName::MeasureLine).GetRefSRV());

	UpdateInstancedBuffer_TaikoModeNote(noteInstancedBuffer, refTime, testLane);
	Sprite::RenderInstanced(deviceContext, cam, noteInstancedBuffer, 0, noteInstanceCount * (size_t)NoteTextureInstanceID::MAX, textureList.GetTexture(TextureName::note).GetRefSRV());
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
	if (KEYBOARD.Hold(VK_F9)) ChangeStatusStart();
	else if (KEYBOARD.Hold(VK_F10)) ChangeStatusReStart(); 
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

void PlayScene::InitInstancedBuffer(ID3D11Buffer*& buffer, UINT bufLen)
{
	ReleaseCOM(buffer);
	D3D11_BUFFER_DESC instbd{};
	instbd.Usage = D3D11_USAGE_DYNAMIC;
	instbd.ByteWidth = bufLen;
	//instbd.ByteWidth = UINT(sizeof(SpriteInstanceData) * testLane.NoteListConst().size()) * (UINT)NoteTextureInstanceID::MAX;
	instbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//D3D11_SUBRESOURCE_DATA instinitData{};
	//instinitData.pSysMem = &noteInstanceList[0];
	HR(App->GetDevice()->CreateBuffer(&instbd, nullptr, &buffer));

}

void PlayScene::UpdateInstancedBuffer_TaikoModeNote(ID3D11Buffer* instBuffer, const MilliDouble refTime, Lane& lane)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	App->GetDeviceContext()->Map(instBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	D3D11_BUFFER_DESC d;
	noteInstancedBuffer->GetDesc(&d);
	SpriteInstanceData* dataView = reinterpret_cast<SpriteInstanceData*>(mappedData.pData);

	//Update ========================================================
	UpdateInstancedBuffer_TaikoModeNote_Internal(refTime, lane, dataView);

	App->GetDeviceContext()->Unmap(instBuffer, 0);

}

constexpr double distanceQuarterRhythm = CircleDiameter * 0.8;
constexpr pair<double, double> laneDrawArea = { -256.0, (double)LaneMaxLength };

void PlayScene::UpdateInstancedBuffer_MeasureLine(ID3D11Buffer* instBuffer, const vector<Measure>& measureList, const MilliDouble refTime, Lane& lane)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	App->GetDeviceContext()->Map(instBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	D3D11_BUFFER_DESC d;
	noteInstancedBuffer->GetDesc(&d);
	SpriteInstanceData* dataView = reinterpret_cast<SpriteInstanceData*>(mappedData.pData);

	//Update ========================================================
	UpdateInstancedBuffer_MeasureLine_Internal(refTime, measureList, lane, dataView);

	App->GetDeviceContext()->Unmap(instBuffer, 0);
}

void PlayScene::UpdateInstancedBuffer_MeasureLine_Internal(const MilliDouble refTime, const vector<Measure>&measureList, Lane& lane, SpriteInstanceData*& dataView)
{
	using namespace chrono;

	measureLineInstanceCount = 0;

	const vector<NoteDesc>& noteDescList = lane.NoteListConst();

	const vector<NoteDesc>::const_iterator& frontNote
		= lower_bound(noteDescList.begin(), noteDescList.end(),
			microseconds((long long)refTime.count()),
			NoteDesc::CompareLowerBound<microseconds, std::less<>>
		);

	if (frontNote == noteDescList.cend()) return; //end of note, no need to draw measure line

	const long long int& frontMeasureLineIdx = (long long int)frontNote->NoteRef()->mp.measureIdx;

	const NoteDesc& lastNote = noteDescList.back();
	long long int currentMeasureLineIdx = (long long int)lastNote.NoteRef()->mp.measureIdx;

	const double msQuarterInv = musicScore->baseBpm / (double)quarterBeatOfBpm60.count();

	const auto& CopyDataIntoInstancedData = [&]()
		{
			//visible check
			bool isVisible = true;
			if ((int)measureList.size() <= currentMeasureLineIdx) isVisible = measureList.back().visible;
			else isVisible = measureList.at(currentMeasureLineIdx).visible;

			if (!isVisible) return;

			const MusicalPosition& measureMusicalPos{ (size_t)currentMeasureLineIdx, 0 };
			const microseconds& lineTiming = musicScore->GetNoteTimingPoint(measureMusicalPos);
			const double noteRelativeTiming = (double)(duration_cast<milliseconds>(lineTiming).count()) - refTime.count();
			const double notePos = testLane.GetJudgePosition() + distanceQuarterRhythm * noteRelativeTiming * msQuarterInv;

			//in case the note is too far
			if (notePos >= laneDrawArea.second || notePos < 0) return;

			//set the data from note draw desc
			World3D tempWorld3d;
			tempWorld3d.SetParentWorld(&lane.laneSprite.GetWorld3d());
			tempWorld3d.SetObjectScale((FLOAT)lane.laneSprite.GetWorld3dConst().GetObjectScale().x);
			tempWorld3d.SetLocalPosition({ 0, (float)notePos, 0 });

			SpriteInstanceData tempInst{};
			tempInst.Diffuse = MyColor4::White;
			tempInst.uvworld = tempWorld3d.GetUvWorld();
			tempInst.world = tempWorld3d.GetTotalDrawWorld();
			tempInst.TextureID = 0;
			dataView[measureLineInstanceCount] = tempInst;
			
			++measureLineInstanceCount;
		};

	while (frontMeasureLineIdx <= currentMeasureLineIdx)
	{
		CopyDataIntoInstancedData();
		--currentMeasureLineIdx;
	}
}

void PlayScene::UpdateInstancedBuffer_TaikoModeNote_Internal(const MilliDouble refTime, Lane& lane, SpriteInstanceData*& dataView)
{
	using namespace chrono;

	noteInstanceCount = 0;

	const vector<NoteDesc>& noteDescList = lane.NoteListConst();

	const MicroDouble& earlyBadTiming
		= duration_cast<MicroDouble>(accRange.GetEarlyJudgeTiming(refTime, AccuracyRange::RangeName::Bad));

	const vector<NoteDesc>::const_reverse_iterator rEndIter
		= upper_bound
		(
			noteDescList.rbegin(),
			noteDescList.rend(),
			microseconds((long long)earlyBadTiming.count()),
			NoteDesc::CompareUpperBound<microseconds, greater<>>
		);

	vector<NoteDesc>::const_reverse_iterator rcIter = noteDescList.rbegin();

	const double msQuarterInv = musicScore->baseBpm / (double)quarterBeatOfBpm60.count();

	const auto& CopyNoteDrawDescIntoInstancedData = [&]() 
		{
			if (rcIter->IsPassedConst() == true) return;

			const microseconds& noteTiming = (*rcIter).Timing();
			const double noteRelativeTiming = (double)(duration_cast<milliseconds>(noteTiming).count()) - refTime.count();
			const double notePos = testLane.GetJudgePosition() + distanceQuarterRhythm * noteRelativeTiming * msQuarterInv;

			//in case the note is too far
			if (notePos >= laneDrawArea.second) return;

			//set the data from note draw desc
			const NoteDrawDesc& tempNoteDrawDesc = lane.GetNoteDrawDesc(rcIter->NoteRef()->noteType);
			World3D tempWorld3d;
			tempWorld3d.SetParentWorld(&lane.laneSprite.GetWorld3d());
			tempWorld3d.SetObjectScale((FLOAT)tempNoteDrawDesc.diameter);
			tempWorld3d.SetLocalPosition({ 0, (float)notePos, 0 });

			SpriteInstanceData tempInst{};
			tempInst.Diffuse = tempNoteDrawDesc.color;
			tempInst.uvworld = tempWorld3d.GetUvWorld();
			tempInst.world = tempWorld3d.GetTotalDrawWorld();
			tempInst.TextureID = tempNoteDrawDesc.textureID;
			dataView[(int)NoteTextureInstanceID::MAX * noteInstanceCount + (int)NoteTextureInstanceID::Note] = tempInst;

			tempInst.Diffuse = MyColor4::White;
			tempInst.TextureID = tempNoteDrawDesc.textureOverlayID;
			dataView[(int)NoteTextureInstanceID::MAX * noteInstanceCount + (int)NoteTextureInstanceID::NoteOverlay] = tempInst;

			++noteInstanceCount;
		};

	while (rcIter != rEndIter)
	{
		CopyNoteDrawDescIntoInstancedData();
		++rcIter;
	}
}
