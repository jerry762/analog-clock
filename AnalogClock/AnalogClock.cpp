// AnalogClock.cpp : 定義應用程式的進入點。
//

#include "framework.h"
#include "AnalogClock.h"

#define MAX_LOADSTRING 100
#define PI acos(-1.0)

// 全域變數:
HINSTANCE hInst;                                // 目前執行個體
WCHAR szTitle[MAX_LOADSTRING];                  // 標題列文字
WCHAR szWindowClass[MAX_LOADSTRING];            // 主視窗類別名稱

// 這個程式碼模組所包含之函式的向前宣告:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void PrepareDC(HWND hWnd, HDC hdc);
void DrawAxes(HDC hdc);
void DrawClock(HDC hdc);
void SetTransform(HDC hdc, int deg, POINT pos);

void DrawClockHand(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置程式碼。

	// 將全域字串初始化
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_ANALOGCLOCK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 執行應用程式初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ANALOGCLOCK));

	MSG msg;

	// 主訊息迴圈:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


//
//  函式: MyRegisterClass()
//
//  用途: 註冊視窗類別。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANALOGCLOCK));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ANALOGCLOCK);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函式: InitInstance(HINSTANCE, int)
//
//   用途: 儲存執行個體控制代碼並且建立主視窗
//
//   註解:
//
//        在這個函式中，我們將執行個體控制代碼儲存在全域變數中，
//        並建立及顯示主程式視窗。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 將執行個體控制代碼儲存在全域變數中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函式: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  用途: 處理主視窗的訊息。
//
//  WM_COMMAND  - 處理應用程式功能表
//  WM_PAINT    - 繪製主視窗
//  WM_DESTROY  - 張貼結束訊息然後傳回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int nTimer = 1;

	switch (message)
	{
	case WM_CREATE:
	{
		SetTimer(hWnd, nTimer, 100, NULL);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 剖析功能表選取項目:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此新增任何使用 hdc 的繪圖程式碼...

		PrepareDC(hWnd, hdc);

		//DrawAxes(hdc);

		DrawClock(hdc);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_TIMER:
	{
		DrawClockHand(hWnd);
	}
	break;
	case WM_DESTROY:
		KillTimer(hWnd, nTimer);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// [關於] 方塊的訊息處理常式。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void PrepareDC(HWND hWnd, HDC hdc)
{
	SetGraphicsMode(hdc, GM_ADVANCED);
	SetMapMode(hdc, MM_ISOTROPIC);

	RECT rectClient;
	GetClientRect(hWnd, &rectClient);

	SetWindowExtEx(hdc, 1100, 1100, NULL);
	SetViewportExtEx(hdc, rectClient.right, -rectClient.bottom, NULL);
	SetViewportOrgEx(hdc, rectClient.right / 2, rectClient.bottom / 2, NULL);
}

void DrawAxes(HDC hdc)
{
	HGDIOBJ hpen = GetStockObject(BLACK_PEN);
	HGDIOBJ hBrush = GetStockObject(BLACK_BRUSH);
	HGDIOBJ hOldPen = SelectObject(hdc, hpen);
	HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

	MoveToEx(hdc, -500, 0, NULL);
	LineTo(hdc, 500, 0);
	MoveToEx(hdc, 0, -500, NULL);
	LineTo(hdc, 0, 500);

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);

	DeleteObject(hpen);
	DeleteObject(hBrush);
}

void DrawClock(HDC hdc)
{
	HGDIOBJ hOldPen = SelectObject(hdc, CreatePen(PS_SOLID, 20, RGB(15, 10, 130)));
	HGDIOBJ hOldBrush = SelectObject(hdc, CreateSolidBrush(RGB(110, 221, 255)));

	Ellipse(hdc, -500, -500, 500, 500);

	for (size_t i = 1; i <= 60; i++)
	{
		if (i % 5 == 1)
		{
			DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 10, RGB(0, 0, 0))));
			MoveToEx(hdc, 0, 490, NULL);
			LineTo(hdc, 0, 460);
			//ExtTextOutA(hdc, 0, 440, ETO_CLIPPED, NULL, "7", 1, NULL);
		}
		else
		{
			DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(0, 0, 0))));
			MoveToEx(hdc, 0, 490, NULL);
			LineTo(hdc, 0, 460);
		}
		SetTransform(hdc, -6 * i, { 0, 0 });
	}

	static SYSTEMTIME time;
	GetLocalTime(&time);

	const float fHourAngle = -1 * ((360.0f / 12) * (time.wHour) + (time.wMinute * 0.5f));
	const float fMinuteAngle = -1 * (360.0f / 60) * (time.wMinute);
	const float fSecondAngle = -1 * ((360.0f / 60) * (time.wSecond) + (360.0f / 60000) * (time.wMilliseconds));

	// handle short hand
	SetTransform(hdc, fHourAngle, { 0, 0 });

	POINT ShortHandvertices[] = { {0, 350}, {-30, 310}, {-10, 310}, {-10, 0}, {10,0}, {10, 310},{30, 310} };
	DeleteObject(SelectObject(hdc, CreateSolidBrush(RGB(0, 0, 0))));

	Polygon(hdc, ShortHandvertices, sizeof(ShortHandvertices) / sizeof(ShortHandvertices[0]));

	// handle long hand
	SetTransform(hdc, fMinuteAngle, { 0, 0 });

	POINT LongHandvertices[] = { {0, 450}, {-30, 410}, {-10, 410}, {-10, 0}, {10,0}, {10, 410},{30, 410} };
	DeleteObject(SelectObject(hdc, CreateSolidBrush(RGB(0, 0, 0))));

	Polygon(hdc, LongHandvertices, sizeof(LongHandvertices) / sizeof(LongHandvertices[0]));

	// handle second hand
	SetTransform(hdc, fSecondAngle, { 0, 0 });

	DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 5, RGB(255, 23, 23))));
	MoveToEx(hdc, 0, 0, NULL);
	LineTo(hdc, 0, 450);

	DeleteObject(SelectObject(hdc, hOldPen));
	DeleteObject(SelectObject(hdc, hOldBrush));
}

void SetTransform(HDC hdc, int deg, POINT pos)
{
	XFORM xform;

	ZeroMemory(&xform, sizeof(XFORM));
	double theta = deg / 180.0 * PI;

	xform.eM11 = (float)cos(theta);
	xform.eM12 = (float)sin(theta);
	xform.eM21 = (float)-sin(theta);
	xform.eM22 = (float)cos(theta);

	xform.eDx = (float)pos.x;
	xform.eDy = (float)pos.y;

	SetWorldTransform(hdc, &xform);
}

void DrawClockHand(HWND hWnd)
{
	InvalidateRect(hWnd, NULL, TRUE);

	//OutputDebugString(L"hello\n");
}
