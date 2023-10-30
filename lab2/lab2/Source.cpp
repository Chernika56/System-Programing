#include <windows.h>
#include "Constant.h"
#include "TableDrawer.h"
#include "TextFormatter.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(CreateSolidBrush(ProjConst::WND_MAIN_INI_COLOR));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = ProjConst::WND_MAIN_CLASS_NAME;
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow(ProjConst::WND_MAIN_CLASS_NAME, ProjConst::WND_MAIN_CAPTION, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, ProjConst::WND_INI_WIDTH, ProjConst::WND_INI_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT wndRect;
	SIZE letterSize;
	LOGFONT font{};
	HFONT hFont{};

	GetWindowRect(hWnd, &wndRect);
	int width = wndRect.right - wndRect.left;
	FLOAT fontSize = -((FLOAT)width / ProjConst::WND_INI_WIDTH) * ProjConst::FONT_DEFAULT_SIZE;
	TableDrawer table(ProjConst::ROWS, ProjConst::COLUMNS, width, ProjConst::TEXT);
	
	switch (message) 
	{
	case WM_SIZE:
		InvalidateRect(hWnd, NULL, true);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		hFont = CreateFont(fontSize, 0, 0, 0, 0, 0, 0, 0, 
			ANSI_CHARSET, OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, 
			L"Times New Roman");
		//L"Times New Roman" 
		//ProjConst::FONT_TYPE_TERMINAL
		table.setHDC(hdc);
		table.draw(hFont);

		//font.lfHeight = 40;
		//font.lfWidth = 20;
		//table.setHDC(hdc);
		//table.draw(font);

		//SelectObject(hdc, hFont);

		// Uncomment to leave the same font size
		//table.setHDC(hdc);
		//table.draw();

		GetTextExtentPoint(hdc, &TextFormatter::ELLIPTICAL_TEXT[0], 1, &letterSize);
		TextFormatter::drawEllipticalText(hdc, TextFormatter::ELLIPTICAL_TEXT, { TextFormatter::COORD_X,
			TextFormatter::COORD_Y }, TextFormatter::RADIUS_V, TextFormatter::RADIUS_H, letterSize.cy + 4);
		
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
