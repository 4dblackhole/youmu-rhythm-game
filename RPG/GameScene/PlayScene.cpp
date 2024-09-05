#include "framework.h"
#include "PlayScene.h"

constexpr float LayoutSize = 4.0f;
constexpr int LayoutBaseX = -60;
constexpr int LayoutBaseY = 200;
constexpr int LayoutDistanceY = 100;

constexpr int TriangleOffsetX = -8;
constexpr int TriangleOffsetY = 11;

constexpr float LaneWidth = 180.0f;

PlayScene::PlayScene(Music* m, Pattern* p) :
	music(m), pattern(p),
	transparentBlackBG(0, 0, (float)StandardWidth, (float)StandardHeight, { 0,0,0,0.5f }, true),
	laneSprite(LaneWidth, 10000), circleSprite(0, 0)
{
	timer.Reset();
	InitPauseOptionLayoutList();
	ChangeStatusLoad();

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

void PlayScene::InitTextures()
{
	ReleaseTextures();
	const wstring skinDir = SkinDir + L"test Skin/";
	Texture* tempTexture = nullptr;

	tempTexture = new Texture;
	tempTexture->CreateTexture(App->GetDevice(), &D2D, skinDir + L"LaneBackground.png");
	textureList.emplace(make_pair(TextureName::LaneBackground, tempTexture));

	tempTexture = new Texture;
	tempTexture->CreateTexture(App->GetDevice(), &D2D, skinDir + L"hitcircle.png");
	textureList.emplace(make_pair(TextureName::hitCircle, tempTexture));
	tempTexture = new Texture;
	tempTexture->CreateTexture(App->GetDevice(), &D2D, skinDir + L"hitcircleoverlay.png");
	textureList.emplace(make_pair(TextureName::hitcircleoverlay, tempTexture));
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
	laneSprite.GetWorld3d().SetCenterPosition({ 0, 0.5f, 0 });
	laneSprite.GetWorld3d().SetLocalRotation({ 0, 0, -Math::PI * 0.5f });
	laneSprite.GetWorld3d().SetLocalPosition({ LaneWidth,100,0 });
	laneSprite.SetTexture(laneBGTexture->textureSRV.Get());

	laneSprite.RepeatTextureInExtraArea(laneBGTexture->width, laneBGTexture->height);
	laneSprite.GetWorld3d().SetAlignX(AlignModeX::Left);
	laneSprite.GetWorld3d().SetParentDrawWorld();

	Texture* const& hitCircleTexture = textureList.find(TextureName::hitCircle)->second;
	circleSprite.GetWorld3d().SetParentWorld(&laneSprite.GetWorld3d());
	circleSprite.GetWorld3d().SetObjectScale({ (float)hitCircleTexture->width, (float)hitCircleTexture->height });
	circleSprite.SetTexture(hitCircleTexture->textureSRV.Get());
	circleSprite.Diffuse = XMFLOAT4(0, 1, 0.5f, 1);

	Texture* const& hitCircleOverlayTexture = textureList.find(TextureName::hitcircleoverlay)->second;
	circleOverlaySprite.GetWorld3d().SetParentWorld(&circleSprite.GetWorld3d());
	circleOverlaySprite.GetWorld3d().SetObjectScale({ (float)hitCircleOverlayTexture->width, (float)hitCircleOverlayTexture->height });
	circleOverlaySprite.SetTexture(hitCircleOverlayTexture->textureSRV.Get());
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
	swprintf_s(tempStr, L"Time: %.3fs", totalMusicTime * 0.001f);
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
		tempNote.mp.position = ShortCut::StrToRationalNumber<64>(noteElements[(int)Note::DataOrder::Beat]);
		size_t referencingMeasureIdx = min(measureIdx, musicScore->measures.size() - 1);
		if (tempNote.mp.position >= musicScore->measures[referencingMeasureIdx].length) //out of measure
		{
			if (ReadNextLine()) break;
			else continue;
		}


		//DataOrder::Key
		wss << noteElements[(int)Note::DataOrder::Key];
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
	InitMeasurePrefixSum();
	InitBpmTimingPrefixSum();
}

void PlayScene::InitMeasurePrefixSum()
{
	if (musicScore->measures.empty()) return;

	ReleaseMeasurePrefixSum();
	measurePrefixSum.reserve(musicScore->measures.size());
	
	vector<Measure>::const_iterator it = musicScore->measures.begin();
	measurePrefixSum.emplace_back((it++)->length);

	for (size_t prevIdx = 0; it!= musicScore->measures.cend(); ++prevIdx, ++it)
	{
		const auto& val = measurePrefixSum[prevIdx] + it->length;
		measurePrefixSum.emplace_back(val);
	}
}

void PlayScene::ReleaseMeasurePrefixSum()
{
	measurePrefixSum.clear();
}

//[start, end]
RationalNumber<64> PlayScene::GetMeasurePrefixSum(int start, int end)
{
	if (end < start) return 0;
	if (start == end)
	{
		const int& idx = std::clamp(start, 0, (int)musicScore->measures.size() - 1);
		return musicScore->measures[idx].length;
	}

	const RationalNumber<64>& endSum = GetMeasurePrefixSum(end);
	if (start < 1) return endSum;
	const RationalNumber<64>& prevSum = GetMeasurePrefixSum(start - 1);
	return endSum - prevSum;
}

//[start, end]
RationalNumber<64> PlayScene::GetMeasurePrefixSum(int idx)
{
	if (idx < 0) return 0;
	const size_t& measureListSize = measurePrefixSum.size();
	if (idx < (int)measureListSize) return measurePrefixSum[idx];
	else
	{
		return measurePrefixSum[idx] + musicScore->measures.back().length * (idx - (int)measureListSize + 1);
	}

}

void PlayScene::InitBpmTimingPrefixSum()
{
	ReleaseBpmTimingPrefixSum();
	bpmTimingPrefixSum.emplace(make_pair(MusicalPosition({ 0, RationalNumber<64>(0) }), 0));
	if (musicScore->bpms.empty()) return;

	//initiation variables for loop
	MusicBPM currentBpmSignature({}, musicScore->baseBpm); // 0-pos, basebpm
	set<MusicBPM>::const_iterator nextBpmSignature = musicScore->bpms.begin();
	
	while (nextBpmSignature != musicScore->bpms.cend())
	{
		const decltype(bpmTimingPrefixSum)::const_iterator& lastSumIter = std::prev(bpmTimingPrefixSum.end());
		const MusicalPosition& currentPos = currentBpmSignature.mp;
		const double& currentBPM = currentBpmSignature.BPM();

		const RationalNumber<64>& currentMeasureArea = GetMeasurePrefixSum((int)currentPos.measureIdx, (int)nextBpmSignature->mp.measureIdx - 1);
		const RationalNumber<64>& currentBpmArea = currentMeasureArea - currentBpmSignature.mp.position + nextBpmSignature->mp.position;

		constexpr size_t msPerCommonTimeOfBPM60 = 240000;
		const double& msOfCurrentArea = (double)currentBpmArea * ((double)msPerCommonTimeOfBPM60 / currentBPM);
		const double& resultTiming = lastSumIter->second.count() + msOfCurrentArea;
		bpmTimingPrefixSum[nextBpmSignature->mp] = decltype(bpmTimingPrefixSum)::mapped_type(resultTiming);

		currentBpmSignature = *nextBpmSignature;
		++nextBpmSignature;
	}

}

void PlayScene::ReleaseBpmTimingPrefixSum()
{
	bpmTimingPrefixSum.clear();
}

const decltype(PlayScene::bpmTimingPrefixSum)::iterator PlayScene::GetBpmTimingPoint(const decltype(PlayScene::bpmTimingPrefixSum)::key_type& val)
{
	return bpmTimingPrefixSum.lower_bound(val);
}

chrono::microseconds PlayScene::GetNoteTimingPoint(const Note& note)
{
	using namespace chrono;
	const decltype(PlayScene::bpmTimingPrefixSum)::iterator& bpmIter = GetBpmTimingPoint(note.mp);
	const MusicalPosition& bpmPos = bpmIter->first;
	const MilliDouble& bpmTiming = bpmIter->second;

	const RationalNumber<64>& relativePos = GetMeasurePrefixSum((int)bpmPos.measureIdx, (int)note.mp.measureIdx - 1) + note.mp.position - bpmPos.position;
	const MilliDouble& resultTiming = bpmTiming + MilliDouble((double)relativePos);
	return microseconds((microseconds::rep)duration_cast<duration<double, std::micro>>(resultTiming).count());

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
		}

		//TODO: run Load-functions as thread

		endIdx = uniFile.find(EndlineIdc, timeSignatureIdx);
		const wstring_view timeSignatureView(uniFile.c_str() + endIdx + EndlineIdcLength, patternIdx - (endIdx + EndlineIdcLength));
		LoadTimeSignature(timeSignatureView);

		endIdx = uniFile.find(EndlineIdc, patternIdx);
		const wstring_view patternView(uniFile.c_str() + endIdx + EndlineIdcLength);
		LoadPattern(patternView);


		testLane.LoadNotes(musicScore);

		const Note* firstNote = musicScore->GetFirstNote();
		constexpr chrono::milliseconds waitTime(1000);
		if (firstNote != nullptr)
		{
			firstNoteTiming = GetNoteTimingPoint(*firstNote);
			if (firstNoteTiming < waitTime) UpdateMusicTimeOffset(firstNoteTiming - waitTime);
		}
		//this_thread::sleep_for(chrono::milliseconds(3000));
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
}

void PlayScene::StopThread()
{
	STopPlayMusicThread();
	StopLoadMusicScoreThread();
}

PlayScene::~PlayScene()
{
	StopThread();
	SafeDelete(musicScore);
	ReleaseTextures();
}

void PlayScene::BeginScene()
{
	InitTextures();
	InitLanes();
	InitSprites();
}

void PlayScene::OnResize(float newW, float newH)
{
	transparentBlackBG.ChangeWidthToCurrentWidth(newW, newH);
	loadingText.GetWorld2d().OnParentWorldUpdate();
	currentTimeText.GetWorld2d().OnParentWorldUpdate();

	laneSprite.OnResize();
	circleSprite.OnResize();
	circleOverlaySprite.OnResize();
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
	totalMusicTime = rhythmTimer.TotalTime() * 1000.0f + (double)(duration_cast<milliseconds>(musicTimeOffset).count());
}

void PlayScene::STopPlayMusicThread()
{
	playMusicThreadRunFlag = false;
	if (playMusicThread.joinable())playMusicThread.join();
}

void PlayScene::PlayMusic()
{
	while (totalMusicTime < 0 && playMusicThreadRunFlag); //busy wait

	music->PlayMusic();
	UpdateTotalMusicTime();
	music->channel->setPosition((unsigned int)totalMusicTime, FMOD_TIMEUNIT_MS);
}

void PlayScene::ChangeStatusReStart()
{
	rhythmTimer.Reset();
	UpdateTotalMusicTime();
	
	playMusicThreadRunFlag = true;
	playMusicThread = thread(&PlayScene::PlayMusic, this);

	TRACE(_T("ReStart Time: %.6lf\n"), totalMusicTime);
	sceneStatus = Status::Start;
}
void PlayScene::ChangeStatusStart()
{
	rhythmTimer.Start();
	rhythmTimer.Tick();

	playMusicThreadRunFlag = true;
	playMusicThread = thread(&PlayScene::PlayMusic, this);

	TRACE(_T("Start Time: %.6lf\n"), totalMusicTime);
	sceneStatus = Status::Start;
}

void PlayScene::ExitStatusStart()
{
	STopPlayMusicThread();
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
		circleSprite.GetWorld3d().MoveLocalPosition(0, -400 * dt, 0);
		circleOverlaySprite.GetWorld3d().OnParentWorldUpdate();
	}
	
	if (KEYBOARD.Hold('X'))
	{
		circleSprite.GetWorld3d().MoveLocalPosition(0, 400 * dt, 0);
		circleOverlaySprite.GetWorld3d().OnParentWorldUpdate();
	}

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

	laneSprite.Render(deviceContext, cam);

	circleSprite.Render(deviceContext, cam);
	circleOverlaySprite.Render(deviceContext, cam);
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
	RenderStatus(prevSceneStatus, deviceContext, cam);

	transparentBlackBG.Render(deviceContext, cam);
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
