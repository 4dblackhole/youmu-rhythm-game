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

	constexpr XMFLOAT4 f(1, 1, 1, 1);
	constexpr RationalNumber cr(3, 6);
	RationalNumber df = RationalNumber{ 2,4 } + RationalNumber{ -34,47 } / RationalNumber{ 7,59 };

	MainApp theApp(hInstance);

	if (!theApp.Init()) return 0;
	return theApp.Run();
}