#include <windows.h>
#include <tchar.h>
#include <thread>
#include <vector>
#include <ctime>
#include <atomic>
#include <sstream>
#include <list>
#include <chrono>

HINSTANCE hInst;
HWND hwndMain, hwndInput, hwndSpeed_Edit, hwndNum_Edit,  hwndCyrcle;
int speed = 0, circleNumMax = 0, circleNum = 0, circleNumDel = 0, clickNum = 0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InputWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CircleWndProc(HWND, UINT, WPARAM, LPARAM);

// Структура для хранения информации о щелчке
class CircleEvent {
public:
	int x;
	int y;
	bool visible;
	std::chrono::steady_clock::time_point appearanceTimestamp;
	std::chrono::steady_clock::time_point removalTimestamp;

	CircleEvent(int _x, int _y, bool _visible, std::chrono::steady_clock::time_point _appearanceTimestamp, 
		std::chrono::steady_clock::time_point _removalTimestamp)
		: x(_x), y(_y), visible(_visible), appearanceTimestamp(_appearanceTimestamp), 
		removalTimestamp(_removalTimestamp) {}

	void setRemovalTimestamp(std::chrono::steady_clock::time_point removalTime) {
		removalTimestamp = removalTime;
	}
};

// Очередь для хранения событий щелчков
std::list<CircleEvent> circleEventsList;

void DrawCircles(HWND hwnd) {
	int x = rand() % 800 + 50;
	int y = rand() % 600 + 50;

	HDC hdc = GetDC(hwnd);
	HBRUSH brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
	SelectObject(hdc, brush);
	Ellipse(hdc, x - 25, y - 25, x + 25, y + 25);
	DeleteObject(brush);
	ReleaseDC(hwnd, hdc);

	std::chrono::steady_clock::time_point appearanceTime = std::chrono::steady_clock::now();
	circleEventsList.push_back(CircleEvent(x, y, true, appearanceTime, appearanceTime));
}

void dataOutput(HWND hwnd) {
	int interval = 0;

	for (auto it = circleEventsList.begin(); it != circleEventsList.end(); it++) {
		auto time1 = it->appearanceTimestamp.time_since_epoch();
		int milliseconds1 = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(time1).count());

		auto time2 = it->removalTimestamp.time_since_epoch();
		int milliseconds2 = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(time2).count());

		interval += milliseconds2 - milliseconds1;
	}

	int stat1 = interval / circleEventsList.size();

	std::wstring massageText = L"Среднее время реакции:\n" + 
		std::to_wstring(stat1) + L" миллисекунд\n\nТочность:\n" + 
		std::to_wstring((float)circleEventsList.size() / (clickNum) * 100) + L"%";

	MessageBox(hwnd, massageText.c_str(), _T("Результат"), MB_OK);
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
        _T("InputWindowClass"), _T("Окно ввода"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 250,
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
		0, 0, 1000, 800,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwndCyrcle, SW_SHOWNORMAL);
    UpdateWindow(hwndCyrcle);

	SetTimer(hwndCyrcle, 1, 5000 / speed, NULL);
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
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        CreateWindow(
            _T("BUTTON"), _T("Открыть окно ввода скорости"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            100, 50, 200, 50,
            hwnd, (HMENU)1, hInst, NULL
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
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

// Обработчик сообщений для окна ввода скорости
LRESULT CALLBACK InputWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
		CreateWindow(
			_T("STATIC"), _T("Введите скорость"),
			WS_CHILD | WS_VISIBLE,
			20, 10, 150, 30,
			hwnd, NULL, hInst, NULL
		);

        hwndSpeed_Edit = CreateWindow(
            _T("EDIT"), _T(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            20, 40, 150, 30,
            hwnd, NULL, hInst, NULL
        );

		CreateWindow(
			_T("STATIC"), _T("Введите число кругов"),
			WS_CHILD | WS_VISIBLE,
			20, 80, 150, 30,
			hwnd, NULL, hInst, NULL
		);

		hwndNum_Edit = CreateWindow(
			_T("EDIT"), _T(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			20, 110, 150, 30,
			hwnd, NULL, hInst, NULL
		);

        CreateWindow(
            _T("BUTTON"), _T("Вывести сообщение"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 150, 150, 30,
            hwnd, (HMENU)4, hInst, NULL
        );

        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 4) {
            TCHAR buffer[256];

            GetWindowText(hwndSpeed_Edit, buffer, sizeof(buffer) / sizeof(buffer[0]));
            speed = _tstoi(buffer);

			GetWindowText(hwndNum_Edit, buffer, sizeof(buffer) / sizeof(buffer[0]));
			circleNumMax = _tstoi(buffer);

            CreateCircleWindow();

            DestroyWindow(hwnd);
        }

        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);

        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK CircleWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		circleNum = 0;
		circleNumDel = 0;
		clickNum = 0;
		circleEventsList.clear();

		break;
	case WM_TIMER:
		if (circleNum < circleNumMax) {
			DrawCircles(hwnd);
			circleNum++;
		}
		else {
			if (circleNumDel == circleNumMax) {
				DestroyWindow(hwnd);
			}
		}

		break;
	case WM_SIZE:
		InvalidateRect(hwnd, NULL, true);

		break;
	case WM_CHAR:
	{
		if (wParam == 'W' || wParam == 'w') {
			std::wstringstream ss;
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			ScreenToClient(hwnd, &cursorPos);

			int mouseX = cursorPos.x;
			int mouseY = cursorPos.y;

			for (auto it = circleEventsList.begin(); it != circleEventsList.end(); it++) {
				int x = it->x;
				int y = it->y;

				if ((mouseX - x) * (mouseX - x) + (mouseY - y) * (mouseY - y) <= 25 * 25) {
					std::chrono::steady_clock::time_point removalTime = std::chrono::steady_clock::now();
					it->setRemovalTimestamp(removalTime);
					it->visible = false;

					circleNumDel++;

					InvalidateRect(hwnd, NULL, TRUE);
					break;
				}
			}

			clickNum++;
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);
		for (auto it = circleEventsList.begin(); it != circleEventsList.end(); it++) {
			int x = it->x;
			int y = it->y;

			if ((mouseX - x) * (mouseX - x) + (mouseY - y) * (mouseY - y) <= 25 * 25) {
				std::chrono::steady_clock::time_point removalTime = std::chrono::steady_clock::now();
				it->setRemovalTimestamp(removalTime);
				it->visible = false;

				circleNumDel++;

				InvalidateRect(hwnd, NULL, TRUE);
				break;
			}
		}

		clickNum++;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		for (auto it = circleEventsList.begin(); it != circleEventsList.end(); it++) {
			if (it->visible) {
				int x = it->x;
				int y = it->y;

				HBRUSH brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
				SelectObject(hdc, brush);
				Ellipse(hdc, x - 25, y - 25, x + 25, y + 25);
				DeleteObject(brush);
			}
		}

		EndPaint(hwnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hwndCyrcle, 1);
		dataOutput(hwnd);

		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}