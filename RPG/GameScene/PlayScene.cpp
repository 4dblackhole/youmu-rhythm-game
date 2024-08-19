#include "framework.h"
#include "PlayScene.h"

constexpr float LayoutSize = 4.0f;
constexpr int LayoutBaseX = -60;
constexpr int LayoutBaseY = 200;
constexpr int LayoutDistanceY = 100;

constexpr int TriangleOffsetX = -8;
constexpr int TriangleOffsetY = 11;

PlayScene::PlayScene(const Music* m, const Pattern* p) : 
	music(m), pattern(p), 
	transparentBlackBG(0, 0, (float)StandardWidth, (float)StandardHeight, { 0,0,0,0.5f }, true)
{
	timer.Reset();
	InitPauseOptionLayoutList();
	ChangeStatus(Status::Load);

	InitLanes();

	InitLoadingText();
	InitCurrentTimeText();
}

void PlayScene::InitLanes()
{
	testLane.AddTargetKey(1);
	testLane.AddTargetKey(2);
	testLane.AddTargetKey(3);
	testLane.AddTargetKey(4);
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
	IDWriteTextFormat*& currentFont = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	WCHAR tempStr[50];
	swprintf_s(tempStr, L"Time: %.3fs", rhythmTimer.TotalTime());
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
	//double tobeCompareBpm = 120.0;
	//if (musicScore->bpms.empty()) tobeCompareBpm = musicScore->baseBpm;
	//else tobeCompareBpm = std::prev(musicScore->bpms.cend())->BPM();

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
		tempNote.mp.position = ShortCut::StrToRationalNumber<64>(noteElements[(int)Note::DataOrder::Beat]);
		size_t referencingMeasureIdx = min(measureIdx, musicScore->measures.size() - 1);
		if (tempNote.mp.position >= musicScore->measures[referencingMeasureIdx].length) //out of measure
		{
			if (ReadNextLine()) break;
			else continue;
		}


		//DataOrder::Key
		wss << noteElements[(int)Note::DataOrder::Key];
		wss >> tempNote.keyType;

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

		musicScore->notesPerKeyMap[tempNote.keyType].emplace(make_pair(tempNote.mp, tempNote));
		if (ReadNextLine()) break; //in case last line
		else continue;
	}
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

		wstringstream wss;
		wss << val << endl;
		wss >> musicScore->offset;

		//Base BPM
		size_t baseBpmIdx = uniFile.find(BaseBpmIdc, offsetIdx);
		if (baseBpmIdx != wstring::npos) 
		{
			endIdx = uniFile.find(EndlineIdc, baseBpmIdx);
			lineStr = wstring_view(uniFile.c_str() + baseBpmIdx, endIdx - baseBpmIdx);
			ShortCut::WordSeparateW(lineStr, L":", nullptr, &val);
			wss << val << endl;
			wss >> musicScore->baseBpm;
		}

		endIdx = uniFile.find(EndlineIdc, timeSignatureIdx);
		const wstring_view timeSignatureView(uniFile.c_str() + endIdx + EndlineIdcLength, patternIdx - (endIdx + EndlineIdcLength));
		LoadTimeSignature(timeSignatureView);

		endIdx = uniFile.find(EndlineIdc, patternIdx);
		const wstring_view patternView(uniFile.c_str() + endIdx + EndlineIdcLength);
		LoadPattern(patternView);

		testLane.LoadNotes(musicScore);

	}
	catch (void* p)
	{
		UNREFERENCED_PARAMETER(p);
		delete musicScore;
		musicScore = nullptr;
	}
	musicScoreLoadFlag = true;
	rhythmTimer.Reset();
}


PlayScene::~PlayScene()
{
	SafeDelete(musicScore);
}

void PlayScene::BeginScene()
{
}

void PlayScene::OnResize(float newW, float newH)
{
	transparentBlackBG.ChangeWidthToCurrentWidth(newW, newH);
	loadingText.GetWorld2d().OnParentWorldUpdate();
	currentTimeText.GetWorld2d().OnParentWorldUpdate();
}

void PlayScene::ChangeStatus(Status s)
{
	TRACE(_T("status %d\n"), (int)s);
	switch (s)
	{
	case PlayScene::Status::Load:
		ChangeStatusLoad();
		break;
	case PlayScene::Status::Ready:
		ChangeStatusReady();
		break;
	case PlayScene::Status::Start:
		ChangeStatusStart();
		break;
	case PlayScene::Status::Resume:
		ChangeStatusResume();
		break;
	case PlayScene::Status::Pause:
		ChangeStatusPause();
		break;
	case PlayScene::Status::End: 
		ChangeStatusEnd();
		break;
	}
}

void PlayScene::ChangeStatusLoad()
{
	sceneStatus = Status::Load;
	SafeDelete(musicScore);
	timer.Reset();
	thread loadMusicScoreThread(thread(&PlayScene::LoadMusicScore, this));
	loadMusicScoreThread.detach();
}

void PlayScene::ChangeStatusReady()
{
	sceneStatus = Status::Ready;
}

void PlayScene::ChangeStatusStart()
{
	rhythmTimer.Start();
	sceneStatus = Status::Start;
}

void PlayScene::ChangeStatusResume()
{
	timer.Reset();
	sceneStatus = Status::Resume;
}

void PlayScene::ChangeStatusPause()
{
	sceneStatus = Status::Pause;
}

void PlayScene::ChangeStatusEnd()
{
	sceneStatus = Status::End;
}

static constexpr float waitTime = 3.0f;
void PlayScene::UpdateOnLoad(float dt)
{
	if (musicScoreLoadFlag)
	{
		timer.Tick();
		if (musicScore == nullptr)
		{
			AlertBox(_T("Pattern Load Fail"));
			ChangeStatus(Status::End);
		}
		else if (KEYBOARD.Down(VK_RETURN) || timer.TotalTime() > waitTime) ChangeStatus(Status::Ready);
	}

	if (KEYBOARD.Down(VK_ESCAPE)) ChangeStatus(Status::End);
}

void PlayScene::UpdateOnReady(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) ChangeStatus(Status::Pause);
	ChangeStatus(Status::Start);
}

void PlayScene::UpdateOnStart(float dt)
{
	rhythmTimer.Tick();
	if (KEYBOARD.Down(VK_ESCAPE))
	{
		rhythmTimer.Stop();
		ChangeStatus(Status::Pause);
	}
	UpdateCurrentTimeText();
}

void PlayScene::UpdateOnResume(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) ChangeStatus(Status::Pause);

	timer.Tick();
	if (timer.TotalTime() >= 1.0f) ChangeStatus(Status::Ready);
}

void PlayScene::ChangePauseOptionKey(int val)
{
	FMODSYSTEM.Play(FmodSystem::Name::button01a);
	pauseOptionKey = val;
	pauseOptSelectTriangle.GetWorld2d().SetParentWorld(&PauseOptLayoutList[pauseOptionKey].GetWorld2d());
}

void PlayScene::UpdateOnPause(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE))sceneStatus = Status::Resume;

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
			ChangeStatus(Status::Resume);
			break;

		case (int)PauseOption::Restart:
			//TODO: Reload Pattern
			FMODSYSTEM.Play(FmodSystem::Name::select05);
			rhythmTimer.Reset();
			ChangeStatus(Status::Start);
			break;

		case (int)PauseOption::Quit:
			FMODSYSTEM.Play(FmodSystem::Name::select05);
			ChangeStatus(Status::End);
			break;
		}
	}
}

void PlayScene::UpdateOnEnd(float dt)
{
	SCENEMANAGER.ChangeScene(SceneManager::Name::SelectMusic);
}

void PlayScene::RenderOnLoad(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	if(!musicScoreLoadFlag)loadingText.Draw();
	if(musicScoreLoadFlag)loadingCompleteText.Draw();
}

void PlayScene::RenderOnReady(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
}

void PlayScene::RenderOnStart(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	currentTimeText.Draw();
}

void PlayScene::RenderOnResume(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
}

void PlayScene::RenderOnPause(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	transparentBlackBG.Render(deviceContext, cam);
	for (auto& it : PauseOptLayoutList) it.Draw();
	pauseOptSelectTriangle.Draw();

	DEBUG_BREAKPOINT;
}

void PlayScene::RenderOnEnd(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
}


void PlayScene::Update(float dt)
{
	switch (sceneStatus)
	{
	case Status::Load:
		UpdateOnLoad(dt);
		break;
	case Status::Ready:
		UpdateOnReady(dt);
		break;
	case Status::Start:
		UpdateOnStart(dt);
		break;
	case Status::Resume:
		UpdateOnResume(dt);
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
	switch (sceneStatus)
	{
	case PlayScene::Status::Load:
		RenderOnLoad(deviceContext, cam);
		break;
	case PlayScene::Status::Ready:
		RenderOnReady(deviceContext, cam);
		break;
	case PlayScene::Status::Start:
		RenderOnStart(deviceContext, cam);
		break;
	case PlayScene::Status::Resume:
		RenderOnResume(deviceContext, cam);
		break;
	case PlayScene::Status::Pause:
		RenderOnPause(deviceContext, cam);
		break;
	case PlayScene::Status::End:
		RenderOnEnd(deviceContext, cam);
		break;
	default:
		break;
	}
}

void PlayScene::EndScene()
{
	SCENEMANAGER.RemoveScene(SceneManager::Name::PlayScene);
}
