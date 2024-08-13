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
	InitPauseOptionLayoutList();
	ChangeStatus(Status::Load);

	InitLoadingText();
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

void PlayScene::LoadMusicScore()
{
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

		timeSignatureIdx += 1;
		endIdx = uniFile.find(EndlineIdc, timeSignatureIdx);


		/*
		//pseudo loading
		this_thread::sleep_for(chrono::milliseconds(1000));
		*/

		musicScoreLoadFlag = true;
		TRACE(_T("Loading Complete\n"));
	}
	catch (void* p)
	{
		UNREFERENCED_PARAMETER(p);
		delete musicScore;
		musicScore = nullptr;
	}
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
	bool patternLoadFlag = false;
	SafeDelete(musicScore);
	timer.Reset();
	thread loadMusicScoreThread(thread(&PlayScene::LoadMusicScore, this));
	loadMusicScoreThread.detach();
	TRACE(_T("Loading...\n"));
}

void PlayScene::ChangeStatusReady()
{
	sceneStatus = Status::Ready;
}

void PlayScene::ChangeStatusStart()
{
	sceneStatus = Status::Start;
}

void PlayScene::ChangeStatusResume()
{
	sceneStatus = Status::Resume;
	timer.Reset();
}

void PlayScene::ChangeStatusPause()
{
	sceneStatus = Status::Pause;
}

void PlayScene::ChangeStatusEnd()
{
	sceneStatus = Status::End;
}

void PlayScene::UpdateOnLoad(float dt)
{
	timer.Tick();
	if (musicScoreLoadFlag)
	{
		constexpr float waitTime = 3.0f;
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
}

void PlayScene::UpdateOnStart(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) ChangeStatus(Status::Pause);
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
			ChangeStatus(Status::Resume);
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

void PlayScene::RenderOnPause(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	transparentBlackBG.Render(deviceContext, cam);
	for (auto& it : PauseOptLayoutList) it.Draw();
	pauseOptSelectTriangle.Draw();

	DEBUG_BREAKPOINT;
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
		break;
	case PlayScene::Status::Start:
		break;
	case PlayScene::Status::Resume:
		break;
	case PlayScene::Status::Pause:
		RenderOnPause(deviceContext, cam);
		break;
	case PlayScene::Status::End:
		break;
	default:
		break;
	}
}

void PlayScene::EndScene()
{
	SCENEMANAGER.RemoveScene(SceneManager::Name::PlayScene);
}
