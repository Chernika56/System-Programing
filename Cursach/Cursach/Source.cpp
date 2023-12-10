#include <windows.h>
#include <tchar.h>
#include <thread>
#include <vector>
#include <ctime>
#include <atomic>
#include <sstream>

std::vector<std::thread> circleThreads;
std::vector<std::pair<int, int>> circles;
std::atomic<bool> drawCircles(true);
std::wstring cursorInfo;

HINSTANCE hInst;
HWND hwndMain, hwndInput, hwndSpeed_Edit, hwndSpeed_Button, hwndCyrcle, hwndInput_Button;
int speed;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InputWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CircleWndProc(HWND, UINT, WPARAM, LPARAM);

void DrawCircles(HWND hwnd) 
{
	while (drawCircles) 
	{
		Sleep(5000 / speed);

		int x = rand() % 1000;
		int y = rand() % 800;

		circles.push_back(std::make_pair(x, y));

		HDC hdc = GetDC(hwnd);
		HBRUSH brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
		SelectObject(hdc, brush);
		Ellipse(hdc, x - 25, y - 25, x + 25, y + 25);
		DeleteObject(brush);
		ReleaseDC(hwnd, hdc);
	}
}

void CreateMainWindow() {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("MainWindowClass");
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

    hwndMain = CreateWindow(
        _T("MainWindowClass"), _T("Главное окно"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwndMain, SW_SHOWNORMAL);
    UpdateWindow(hwndMain);
}

// Функция создания окна для ввода скорости
void CreateInputWindow() {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = InputWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("InputWindowClass");
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

    hwndInput = CreateWindow(
        _T("InputWindowClass"), _T("Окно ввода скорости"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwndInput, SW_SHOWNORMAL);
    UpdateWindow(hwndInput);
}

//Функция создания окна для рисования кругов
void CreateCircleWindow() {
    WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = CircleWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("CircleWindowClass");
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

    hwndCyrcle = CreateWindow(
        _T("CircleWindowClass"), _T("Окно с кругами"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 800,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwndCyrcle, SW_SHOWNORMAL);
    UpdateWindow(hwndCyrcle);
}

// Основная функция WinAPI
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    MSG msg;
    CreateMainWindow();

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Обработчик сообщений для главного окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        CreateWindow(
            _T("BUTTON"), _T("Открыть окно ввода скорости"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            100, 50, 200, 50,
            hWnd, (HMENU)1, hInst, NULL
        );

        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            CreateInputWindow();
        }

        break;
	case WM_DESTROY:
		PostQuitMessage(0);

		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// Обработчик сообщений для окна ввода скорости
LRESULT CALLBACK InputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        hwndSpeed_Edit = CreateWindow(
            _T("EDIT"), _T(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            10, 10, 150, 30,
            hWnd, (HMENU)3, hInst, NULL
        );

        CreateWindow(
            _T("BUTTON"), _T("Вывести сообщение"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            10, 50, 150, 30,
            hWnd, (HMENU)4, hInst, NULL
        );

        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 4) {
            TCHAR buffer[256];
            GetWindowText(hwndSpeed_Edit, buffer, sizeof(buffer) / sizeof(buffer[0]));

            speed = _tstoi(buffer);

            CreateCircleWindow();

            DestroyWindow(hWnd);
        }

        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);

        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK CircleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		circles.clear();
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

				if ((mouseX - x) * (mouseX - x) + (mouseY - y) * (mouseY - y) <= 25 * 25)
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

			if ((mouseX - x) * (mouseX - x) + (mouseY - y) * (mouseY - y) <= 25 * 25)
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
			Ellipse(hdc, x - 25, y - 25, x + 25, y + 25);
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
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}