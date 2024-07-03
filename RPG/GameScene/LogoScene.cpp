#include "framework.h"

#include "LogoScene.h"

LogoScene::LogoScene() : 
	fmodLogo(0.0f, 0.0f, 728.0f, 192.0f), 
	background(0, 0, (float)StandardWidth, (float)StandardHeight, { 0,0,0,1 }, true)
{
	Init();
}

LogoScene::~LogoScene()
{
	Release();
}

//Any key input process
void LogoScene::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (state)
	{
	case LogoScene::Normal:
		if (keyState[wParam] == false)
		{
			keyState[wParam] = true;
			bgTimer.Reset();
			state = LogoScene::Fade;
		}
		break;
	case LogoScene::Fade:
		if (keyState[wParam] == false)
		{
			keyState[wParam] = true;
			state = LogoScene::End;
		}
	break;
	default:
		break;
	}
}

//Any key input process
void LogoScene::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	keyState[wParam] = false;
}

void LogoScene::Init()
{
	fmodLogo.SetTexture(GETTEXTURE(TextureManager::Name::fmodLogo));
}

void LogoScene::Release()
{
}

void LogoScene::BeginScene(float newW, float newH)
{
	//Key input state reset
	ZeroMemory(keyState, sizeof(keyState));

	//Logo state machine reset
	bgTimer.Reset();
	state = LogoScene::Normal;
	
	fmodLogo.Diffuse.w = 1.0f;

	//Logo image setting reset
	OnResize(newW, newH);
}

void LogoScene::OnResize(float newW, float newH)
{
	background.Resize(newW, newH);
}

void LogoScene::Update(float dt)
{
	switch (state)
	{
	case LogoScene::Normal:
	{
		bgTimer.Tick();
		if (bgTimer.TotalTime() > 5.0f)
		{
			bgTimer.Reset();
			state = LogoScene::Fade;
		}
	}
	break;
	case LogoScene::Fade:
	{
		bgTimer.Tick();
		constexpr float FadeSpeed = 0.7f;
		fmodLogo.Diffuse.w = Math::Clamp(1.0f - (bgTimer.TotalTime() * FadeSpeed), 0.0f, 1.0f);

		if (fmodLogo.Diffuse.w < 0.01f) state = LogoScene::End;
	}
	break;
	case LogoScene::End:
		SCENEMANAGER.ChangeScene(SceneManager::Name::Title);
		break;
	default:
		break;
	}
}

void LogoScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{

	background.Render(deviceContext, cam);
	fmodLogo.Render(deviceContext, cam);
}

void LogoScene::EndScene()
{
}
