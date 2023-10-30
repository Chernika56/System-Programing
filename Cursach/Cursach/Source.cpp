#include <windows.h>
#include <thread>
#include <vector>
#include <ctime>
#include <atomic>
#include <sstream>
#include "Constant.h"

std::vector<std::thread> circleThreads;
std::vector<std::pair<int, int>> circles;
std::atomic<bool> drawCircles(true);
std::wstring cursorInfo;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void DrawCircles(HWND hwnd) 
{
	while (drawCircles) 
	{
		int x = rand() % ProjConst::WND_INI_WIDTH;
		int y = rand() % ProjConst::WND_INI_HEIGHT;

		circles.push_back(std::make_pair(x, y));

		HDC hdc = GetDC(hwnd);
		HBRUSH brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
		SelectObject(hdc, brush);
		Ellipse(hdc, x - ProjConst::CIRCEL_RADIUS, y - ProjConst::CIRCEL_RADIUS, 
			x + ProjConst::CIRCEL_RADIUS, y + ProjConst::CIRCEL_RADIUS);
		DeleteObject(brush);
		ReleaseDC(hwnd, hdc);

		Sleep(1000);
	}
}

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
		100, 0, ProjConst::WND_INI_WIDTH, ProjConst::WND_INI_HEIGHT,
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
	switch (message)
	{
	case WM_CREATE:
		srand(time(nullptr));
		std::thread(DrawCircles, hWnd).detach();
		break;
	case WM_SIZE:
		InvalidateRect(hWnd, NULL, true);
		break;
	case WM_CHAR: 
	{
		if (wParam == 'W' || wParam == 'w')
		{
			std::wstringstream ss;
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			ScreenToClient(hWnd, &cursorPos);

			int mouseX = cursorPos.x;
			int mouseY = cursorPos.y;

			for (size_t i = 0; i < circles.size(); i++)
			{
				int x = circles[i].first;
				int y = circles[i].second;

				if ((mouseX - x) * (mouseX - x) + (mouseY - y) * (mouseY - y) <=
					ProjConst::CIRCEL_RADIUS * ProjConst::CIRCEL_RADIUS)
				{
					circles.erase(circles.begin() + i);

					InvalidateRect(hWnd, NULL, TRUE);
					break;
				}
			}
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);
		for (size_t i = 0; i < circles.size(); i++) 
		{
			int x = circles[i].first;
			int y = circles[i].second;

			if ((mouseX - x) * (mouseX - x) + (mouseY - y) * (mouseY - y) <= 
				ProjConst::CIRCEL_RADIUS * ProjConst::CIRCEL_RADIUS) 
			{
				circles.erase(circles.begin() + i);

				InvalidateRect(hWnd, NULL, TRUE);
				break;
			}
		}
	}
	break;
	case WM_RBUTTONDOWN:
		drawCircles = false;
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		for (const auto& circle : circles) 
		{
			int x = circle.first;
			int y = circle.second;

			HBRUSH brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			SelectObject(hdc, brush);
			Ellipse(hdc, x - ProjConst::CIRCEL_RADIUS, y - ProjConst::CIRCEL_RADIUS, 
				x + ProjConst::CIRCEL_RADIUS, y + ProjConst::CIRCEL_RADIUS);
			DeleteObject(brush);
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CLOSE:
		for (std::thread& thread : circleThreads) 
		{
			thread.join();
		}
		circles.clear();
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}