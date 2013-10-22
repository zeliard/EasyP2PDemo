// P2PDemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "P2PDemo.h"
#include "Player.h"
#include "PacketType.h"
#include "Scene.h"
#include "NetHelper.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib,"ws2_32.lib")


HANDLE g_hTimer = NULL;
BOOL InitializeTimer();



class MainWindow : public BaseWindow<MainWindow>
{
	Scene   m_scene;

public:

	PCWSTR  ClassName() const { return L"P2P Demo"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

const WCHAR WINDOW_NAME[] = L"P2P Test";


INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, INT nCmdShow)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
		return 0;

	char* cmdLine = GetCommandLineA() ;
	char seps[]   = " ,\t\n" ;
	char* serverIpAddr = NULL ;
	bool serverMode = true ;
	strtok_s(cmdLine, seps, &serverIpAddr) ;

	if ( strlen(serverIpAddr) > 0 )
	{
		/// 대상 서버 주소가 있으면 클라 모드
		serverMode = false ;
	}

	GNetHelper = new NetHelper(serverMode, serverIpAddr) ;
	
	if ( !GNetHelper->Initialize() )
	{
		MessageBox(NULL, L"NetHelper::Initialize()", L"ERROR", MB_OK) ;
		return -1 ;
	}


	if (!InitializeTimer())
	{
		CoUninitialize();
		return 0;
	}

	MainWindow win;

	if (!win.Create(WINDOW_NAME, WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	// Run the message loop.

	MSG msg = { };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		// Wait until the timer expires or any message is posted.
		if (MsgWaitForMultipleObjects(1, &g_hTimer, FALSE, INFINITE, QS_ALLINPUT) 
			== WAIT_OBJECT_0)
		{
			InvalidateRect(win.Window(), NULL, FALSE);
		}
	}


	CloseHandle(g_hTimer);
	CoUninitialize();


	delete GNetHelper ;

	return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = m_hwnd;
	static bool handshaked = false ;

	switch (uMsg)
	{
	case WM_CREATE:
		if (FAILED(m_scene.Initialize()))
		{
			return -1;
		}
		return 0;

	case WM_DESTROY:
		m_scene.CleanUp();
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		
		if ( wParam == VK_RETURN && !handshaked )
		{
			handshaked = true ;
			if ( !GNetHelper->DoHandShake() )
			{
				ExitProcess(1) ;
			}
		}

		return 0 ;

	case WM_PAINT:
	case WM_DISPLAYCHANGE:
		{
			PAINTSTRUCT ps;
			BeginPaint(m_hwnd, &ps);
			m_scene.Render(m_hwnd);
			EndPaint(m_hwnd, &ps);
		}
		return 0;

	case WM_SIZE:
		{
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);
			m_scene.Resize(x,y);
			InvalidateRect(m_hwnd, NULL, FALSE);
		}
		return 0;

	case WM_ERASEBKGND:
		return 1;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}


BOOL InitializeTimer()
{
	g_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (g_hTimer == NULL)
	{
		return FALSE;
	}

	LARGE_INTEGER li = {0};

	if (!SetWaitableTimer(g_hTimer, &li, (1000/30), NULL, NULL,FALSE))
	{
		CloseHandle(g_hTimer);
		g_hTimer = NULL;
		return FALSE;
	}

	return TRUE;
}

