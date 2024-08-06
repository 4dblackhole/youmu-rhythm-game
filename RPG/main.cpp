#include "framework.h"
#include "App/MainApp.h"

const MainApp* App;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

#if defined (DEBUG) | defined (_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	FMOD::Debug_Initialize(0xffff);
#endif

	MainApp theApp(hInstance);

	//FmodSystem::GetInstance().Init(FMOD_OUTPUTTYPE_ASIO, 64);
	FMODSYSTEM.ChangeDrive(0);

	if (!theApp.Init()) return 0;
	return theApp.Run();

}