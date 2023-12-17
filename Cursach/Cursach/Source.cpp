#include <windows.h>
#include <tchar.h>
#include <thread>
#include <vector>
#include <ctime>
#include <atomic>
#include <sstream>
#include <list>
#include <chrono>
#include <random>

/// <summary>
/// Initializing global variables.
/// </summary>
HINSTANCE hInst;
HWND hwndMain, hwndInput1, hwndInput2, hwndSpeed_Edit, hwndNum_Edit, hwndCircle1, hwndCircle2,
hwndTime_Edit, hwndReaction;
int speed = 0, circleNumMax = 0, circleNum = 0, circleNumDel = 0, clickNum = 0, gameTime = 0;
std::wstring massageText;
bool isReady = false, isGreen = false;
HBRUSH hbrush;

/// <summary>
/// Generating a random number seed.
/// </summary>
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, 1000000);

/// <summary>
/// Prototypes of message processing functions.
/// </summary>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Input1WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Input2WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Circle1WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Circle2WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ReactionWndProc(HWND, UINT, WPARAM, LPARAM);

/// <summary>
/// Represents an event related to a circle, including its position, visibility, and timestamps.
/// </summary>
class CircleEvent {
public:
	/// <summary>
	/// Gets or sets the X-coordinate of the circle event.
	/// </summary>
	int x;

	/// <summary>
	/// Gets or sets the Y-coordinate of the circle event.
	/// </summary>
	int y;

	/// <summary>
	/// Gets or sets a value indicating whether the circle is visible.
	/// </summary>
	bool visible;

	/// <summary>
	/// Gets or sets the timestamp when the circle appeared.
	/// </summary>
	std::chrono::steady_clock::time_point appearanceTimestamp;

	/// <summary>
	/// Gets or sets the timestamp when the circle is scheduled for removal.
	/// </summary>
	std::chrono::steady_clock::time_point removalTimestamp;

	/// <summary>
	/// Initializes a new instance of the CircleEvent class.
	/// </summary>
	/// <param name="_x">The X-coordinate of the circle event.</param>
	/// <param name="_y">The Y-coordinate of the circle event.</param>
	/// <param name="_visible">A value indicating whether the circle is visible.</param>
	/// <param name="_appearanceTimestamp">The timestamp when the circle appeared.</param>
	/// <param name="_removalTimestamp">The timestamp when the circle is scheduled for removal.</param>
	CircleEvent(int _x, int _y, bool _visible,
		std::chrono::steady_clock::time_point _appearanceTimestamp,
		std::chrono::steady_clock::time_point _removalTimestamp)
		: x(_x), y(_y), visible(_visible),
		appearanceTimestamp(_appearanceTimestamp),
		removalTimestamp(_removalTimestamp) {}

	/// <summary>
	/// Sets the removal timestamp of the circle.
	/// </summary>
	/// <param name="removalTime">The timestamp when the circle is scheduled for removal.</param>
	void setRemovalTimestamp(std::chrono::steady_clock::time_point removalTime) {
		removalTimestamp = removalTime;
	}
};

/// <summary>
/// Queue for storing circle events.
/// </summary>
std::list<CircleEvent> circleEventsList;

/// <summary>
/// Structure for storing information about clicks.
/// </summary>
struct ClickEvent {
	std::chrono::system_clock::time_point start;  // Time when the click started
	std::chrono::system_clock::time_point end;    // Time when the click ended
};

/// <summary>
/// Queue for storing information about clicks.
/// </summary>
std::list<ClickEvent> clickEventsList;

/// <summary>
/// Current click event information.
/// </summary>
ClickEvent clickEvent;

/// <summary>
/// Draws circles on the specified window.
/// </summary>
/// <param name="hwnd">Handle to the window where circles will be drawn.</param>
void DrawCircles(HWND hwnd) {
	// Generate random coordinates for the circle
	int x = dis(gen) % 800 + 50;
	int y = dis(gen) % 600 + 50;

	// Get device context and draw the circle
	HDC hdc = GetDC(hwnd);
	SelectObject(hdc, hbrush);
	Ellipse(hdc, x - 25, y - 25, x + 25, y + 25);
	ReleaseDC(hwnd, hdc);

	// Record the appearance time of the circle
	std::chrono::steady_clock::time_point appearanceTime = std::chrono::steady_clock::now();
	circleEventsList.push_back(CircleEvent(x, y, true, appearanceTime, appearanceTime));
}

/// <summary>
/// Outputs statistical data related to circle events.
/// </summary>
/// <param name="hwnd">Handle to the window where the data will be displayed.</param>
void dataOutput1(HWND hwnd) {
	int interval = 0;

	// Calculate the time intervals for each circle event
	for (auto it = circleEventsList.begin(); it != circleEventsList.end(); it++) {
		auto time1 = it->appearanceTimestamp.time_since_epoch();
		int milliseconds1 = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(time1).count());

		auto time2 = it->removalTimestamp.time_since_epoch();
		int milliseconds2 = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(time2).count());

		interval += milliseconds2 - milliseconds1;
	}

	// Calculate the average reaction time
	int stat1 = interval / circleEventsList.size();

	// Calculate and display accuracy
	massageText = L"Average reaction time:\n" + std::to_wstring(stat1) + L" milliseconds\n\nAccuracy:\n" +
		std::to_wstring((float)circleEventsList.size() / (clickNum) * 100) + L"%";

	MessageBox(hwnd, massageText.c_str(), _T("Result"), MB_OK);
}

/// <summary>
/// Outputs statistical data related to click events.
/// </summary>
/// <param name="hwnd">Handle to the window where the data will be displayed.</param>
void dataOutput3(HWND hwnd) {
	int interval = 0;

	// Calculate the time intervals for each click event
	for (auto it = clickEventsList.begin(); it != clickEventsList.end(); it++) {
		auto time1 = it->start.time_since_epoch();
		int milliseconds1 = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(time1).count());

		auto time2 = it->end.time_since_epoch();
		int milliseconds2 = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(time2).count());

		interval += milliseconds2 - milliseconds1;
	}

	// Calculate the average reaction time for click events
	int stat1 = interval / 5;

	// Display the average reaction time
	massageText = L"Average reaction time:\n" + std::to_wstring(stat1) + L" milliseconds\n";

	MessageBox(hwnd, massageText.c_str(), _T("Result"), MB_OK);
}

/// <summary>
/// Creates the main window.
/// </summary>
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
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 500,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwndMain, SW_SHOWNORMAL);
    UpdateWindow(hwndMain);
}

/// <summary>
/// creating a window for entering the parameters of the first mode.
/// </summary>
void CreateInput1Window() {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = Input1WndProc;
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

    hwndInput1 = CreateWindow(
        _T("InputWindowClass"), _T("Окно ввода"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 250,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwndInput1, SW_SHOWNORMAL);
    UpdateWindow(hwndInput1);
}

/// <summary>
/// creating a window for entering the parameters of the second mode.
/// </summary>
void CreateInput2Window() {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = Input2WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("Input2WindowClass");
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hwndInput2 = CreateWindow(
		_T("Input2WindowClass"), _T("Окно ввода"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 250,
		NULL, NULL, hInst, NULL
	);

	ShowWindow(hwndInput2, SW_SHOWNORMAL);
	UpdateWindow(hwndInput2);
}

/// <summary>
/// Creating the first mode rendering window.
/// </summary>
void CreateCircle1Window() {
    WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = Circle1WndProc;
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

    hwndCircle1 = CreateWindow(
        _T("CircleWindowClass"), _T("Окно с кругами"),
        WS_OVERLAPPEDWINDOW,
		0, 0, 1000, 800,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwndCircle1, SW_SHOWNORMAL);
    UpdateWindow(hwndCircle1);

	SetTimer(hwndCircle1, 1, 5000 / speed, NULL);
}

/// <summary>
/// Creating a second mode rendering window.
/// </summary>
void CreateCircle2Window() {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = Circle2WndProc;
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

	hwndCircle2 = CreateWindow(
		_T("CircleWindowClass"), _T("Окно с кругами"),
		WS_OVERLAPPEDWINDOW,
		0, 0, 1000, 800,
		NULL, NULL, hInst, NULL
	);

	ShowWindow(hwndCircle2, SW_SHOWNORMAL);
	UpdateWindow(hwndCircle2);

	SetTimer(hwndCircle2, 1, gameTime * 1000, NULL);
}

/// <summary>
/// Creating a third mode rendering window.
/// </summary>
void CreateReactionWindow() {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = ReactionWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("ReactionWindowClass");
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hwndReaction = CreateWindow(
		_T("ReactionWindowClass"), _T("Окно с кругами"),
		WS_OVERLAPPEDWINDOW,
		0, 0, 800, 600,
		NULL, NULL, hInst, NULL
	);

	ShowWindow(hwndReaction, SW_SHOWNORMAL);
	UpdateWindow(hwndReaction);
}

/// <summary>
/// The entry point for the Windows application.
/// </summary>
/// <param name="hInstance">A handle to the current instance of the application.</param>
/// <param name="hPrevInstance">This parameter is always NULL.</param>
/// <param name="lpCmdLine">The command line for the application, excluding the program name.</param>
/// <param name="nCmdShow">Controls how the window is to be shown.</param>
/// <returns>The exit value contained in the given <see cref="MSG"/> structure.</returns>
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

/// <summary>
/// Message handler for the main window.
/// </summary>
/// <param name="hwnd">The handle to the window.</param>
/// <param name="message">The message identifier.</param>
/// <param name="wParam">Additional message information.</param>
/// <param name="lParam">Additional message information.</param>
/// <returns>The result of the message processing.</returns>
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        CreateWindow(
            _T("BUTTON"), _T("Режим 1"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            100, 50, 200, 50,
            hwnd, (HMENU)1, hInst, NULL
        );

		CreateWindow(
			_T("BUTTON"), _T("Режим 2"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			100, 150, 200, 50,
			hwnd, (HMENU)2, hInst, NULL
		);

		CreateWindow(
			_T("BUTTON"), _T("Режим 3"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			100, 250, 200, 50,
			hwnd, (HMENU)3, hInst, NULL
		);

		CreateWindow(
			_T("BUTTON"), _T("Выбрать цвет"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			100, 350, 200, 50,
			hwnd, (HMENU)4, hInst, NULL
		);

        break;
    case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 1:
			CreateInput1Window();

			break;
		case 2:
			CreateInput2Window();

			break;
		case 3:
			CreateReactionWindow();

			break;
		case 4:
			CHOOSECOLOR cc;                 
			static COLORREF acrCustClr[16]; 
			static DWORD rgbCurrent;        

			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hwnd;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE) {
				hbrush = CreateSolidBrush(cc.rgbResult);
				rgbCurrent = cc.rgbResult;
			}

			break;
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

/// <summary>
/// Message handler for the input window of the first mode.
/// </summary>
/// <param name="hwnd">The handle to the window.</param>
/// <param name="message">The message identifier.</param>
/// <param name="wParam">Additional message information.</param>
/// <param name="lParam">Additional message information.</param>
/// <returns>The result of the message processing.</returns>
LRESULT CALLBACK Input1WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
            _T("BUTTON"), _T("Далее"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 150, 150, 30,
            hwnd, (HMENU)5, hInst, NULL
        );

        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 5) {
            TCHAR buffer[256];

            GetWindowText(hwndSpeed_Edit, buffer, sizeof(buffer) / sizeof(buffer[0]));
            speed = _tstoi(buffer);

			GetWindowText(hwndNum_Edit, buffer, sizeof(buffer) / sizeof(buffer[0]));
			circleNumMax = _tstoi(buffer);

            CreateCircle1Window();

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

/// <summary>
/// Message handler for the input window of the second mode.
/// </summary>
/// <param name="hwnd">The handle to the window.</param>
/// <param name="message">The message identifier.</param>
/// <param name="wParam">Additional message information.</param>
/// <param name="lParam">Additional message information.</param>
/// <returns>The result of the message processing.</returns>
LRESULT CALLBACK Input2WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		CreateWindow(
			_T("STATIC"), _T("Введите время(c)"),
			WS_CHILD | WS_VISIBLE,
			20, 10, 150, 30,
			hwnd, NULL, hInst, NULL
		);

		hwndTime_Edit = CreateWindow(
			_T("EDIT"), _T(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			20, 40, 150, 30,
			hwnd, NULL, hInst, NULL
		);

		CreateWindow(
			_T("BUTTON"), _T("Далее"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			20, 80, 150, 30,
			hwnd, (HMENU)6, hInst, NULL
		);

		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == 6) {
			TCHAR buffer[256];

			GetWindowText(hwndTime_Edit, buffer, sizeof(buffer) / sizeof(buffer[0]));
			gameTime = _tstoi(buffer);

			CreateCircle2Window();

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

/// <summary>
/// Window procedure for the first drawing mode.
/// </summary>
/// <param name="hwnd">The handle to the window.</param>
/// <param name="message">The message identifier.</param>
/// <param name="wParam">Additional message information.</param>
/// <param name="lParam">Additional message information.</param>
/// <returns>The result of the message processing.</returns>
LRESULT CALLBACK Circle1WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
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

				SelectObject(hdc, hbrush);
				Ellipse(hdc, x - 25, y - 25, x + 25, y + 25);
			}
		}

		EndPaint(hwnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		dataOutput1(hwnd);

		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

/// <summary>
/// Window procedure for the second drawing mode.
/// </summary>
/// <param name="hwnd">The handle to the window.</param>
/// <param name="message">The message identifier.</param>
/// <param name="wParam">Additional message information.</param>
/// <param name="lParam">Additional message information.</param>
/// <returns>The result of the message processing.</returns>
LRESULT CALLBACK Circle2WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		circleNumDel = 0;
		circleEventsList.clear();
		DrawCircles(hwnd);

		break;
	case WM_TIMER:
		DestroyWindow(hwnd);

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

			auto it = circleEventsList.begin();
			int x = it->x;
			int y = it->y;

			if ((mouseX - x) * (mouseX - x) + (mouseY - y) * (mouseY - y) <= 25 * 25) {
				circleEventsList.clear();
				DrawCircles(hwnd);

				circleNumDel++;

				InvalidateRect(hwnd, NULL, TRUE);
				break;
			}
			
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);

		auto it = circleEventsList.begin();
		int x = it->x;
		int y = it->y;

		if ((mouseX - x) * (mouseX - x) + (mouseY - y) * (mouseY - y) <= 25 * 25) {
			circleEventsList.clear();
			DrawCircles(hwnd);

			circleNumDel++;

			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
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

				SelectObject(hdc, hbrush);
				Ellipse(hdc, x - 25, y - 25, x + 25, y + 25);
			}
		}

		EndPaint(hwnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hwnd, 1);

		massageText = L"Счет:\n" + std::to_wstring(circleNumDel);

		MessageBox(hwnd, massageText.c_str(), _T("Результат"), MB_OK);

		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

/// <summary>
/// Window procedure for the third drawing mode.
/// </summary>
/// <param name="hwnd">The handle to the window.</param>
/// <param name="message">The message identifier.</param>
/// <param name="wParam">Additional message information.</param>
/// <param name="lParam">Additional message information.</param>
/// <returns>The result of the message processing.</returns>
LRESULT CALLBACK ReactionWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	DRAWITEMSTRUCT drawItem;

	switch (message) {
	case WM_CREATE:
		CreateWindow(
			_T("STATIC"), _T("Нажмите на экран, когда будете готовы"),
			WS_CHILD | WS_VISIBLE | SS_CENTER | SS_OWNERDRAW,
			200, 250, 400, 30,
			hwnd, (HMENU)7, hInst, NULL
		);

		clickNum = 0;

		break;
	case WM_DRAWITEM:
		drawItem = *reinterpret_cast<DRAWITEMSTRUCT*>(lParam);

		if (drawItem.CtlID == 7) {
			// Рисование текста с прозрачным фоном
			SetTextColor(drawItem.hDC, RGB(0, 0, 0));
			SetBkMode(drawItem.hDC, TRANSPARENT);
			RECT rc = drawItem.rcItem;
			DrawText(drawItem.hDC, L"Нажмите на экран, когда будете готовы", -1, &rc, DT_CENTER | DT_VCENTER);
		}

		break;
	case WM_SIZE:
		InvalidateRect(hwnd, NULL, true);

		break;
	case WM_LBUTTONDOWN: {
		if (!isReady) {
			SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(CreateSolidBrush(RGB(255, 0, 0))));
			isReady = true;
			isGreen = false;
			ShowWindow(GetDlgItem(hwnd, 7), SW_HIDE);
			RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
			SetTimer(hwnd, 1, dis(gen) % 5000 + 1000, NULL);

			break;
		}
		else if (isGreen) {
			clickEvent.end = std::chrono::system_clock::now();
			clickEventsList.push_back(clickEvent);
			SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(CreateSolidBrush(RGB(255, 255, 255))));
			isReady = false;
			isGreen = false;
			ShowWindow(GetDlgItem(hwnd, 7), SW_SHOWNORMAL);
			RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);

			clickNum++;
			if (clickNum == 5) {
				DestroyWindow(hwnd);
			}

			break;
		}
		else  {
			KillTimer(hwnd, 1);
			MessageBox(hwnd, _T("Рано"), _T(""), MB_OK);
			SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(CreateSolidBrush(RGB(255, 255, 255))));
			isReady = false;
			isGreen = false;
			ShowWindow(GetDlgItem(hwnd, 7), SW_SHOWNORMAL);
			RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);

			break;
		}
	}
	break;
	case WM_TIMER:
		SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(CreateSolidBrush(RGB(0, 255, 0))));
		isGreen = true;
		RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
		clickEvent.start = std::chrono::system_clock::now();
		KillTimer(hwnd, 1);

		break;
	case WM_DESTROY:
		dataOutput3(hwnd);

		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}