#include "framework.h"
#include "App/MainApp.h"

const D3DApp* App;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

#if defined (DEBUG) | defined (_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	MainApp theApp(hInstance);
	App = &theApp;

	if (!theApp.Init()) return 0;
	return theApp.Run();
}