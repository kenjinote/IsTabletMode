#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

BOOL IsTabletMode()
{
	HKEY hKey;
	DWORD dwTabletMode = 0;
	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER,
		TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell"),
		0,0,0,KEY_READ,0,&hKey,0))
	{
		DWORD dwType = REG_DWORD;
		DWORD dwByte = sizeof(DWORD);
		RegQueryValueEx(hKey, TEXT("TabletMode"), NULL, &dwType, (BYTE *)&dwTabletMode, &dwByte);
		RegCloseKey(hKey);
	}
	return dwTabletMode != 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit;
	switch (msg)
	{
	case WM_CREATE:
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), IsTabletMode() ? TEXT("タブレットモード") : TEXT("デスクトップモード"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_READONLY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SETTINGCHANGE:
		if (!lstrcmpi((LPCTSTR)lParam, TEXT("UserInteractionMode")))
		{
			SetTimer(hWnd, 0x1234, 1000, 0); // レジストリはすぐに反映されないのでタイマーを仕掛ける
		}
		break;
	case WM_TIMER:
		KillTimer(hWnd, 0x1234);
		SetWindowText(hEdit, IsTabletMode() ? TEXT("タブレットモード") : TEXT("デスクトップモード"));
		break;
	case WM_SIZE:
		MoveWindow(hEdit, 10, 10, 256, 32, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("タブレットモードかデスクトップモードか判定する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
