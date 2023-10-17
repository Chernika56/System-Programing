#pragma once

namespace ProjConst {
	// NAMES
	const wchar_t* PROJ_NAME = L"Tracker";
	const wchar_t* WND_CAPTION = L"Lab1";
	const wchar_t* PIC_NAMES[] = { L"shrek1.png", L"donkey.png", L"shrek2.png", L"kat.png"};
	
	// DRAWING
	const int SPEED = 5;

	// SIZES
	const int WND_DEF_HEIGHT = 750;
	const int WND_DEF_WIDTH = 750;
	
	const int TOTAL_PICTURES = 4;
	const int PIC_WIDTH = 100;
	const int PIC_HEIGHT = 200;
	const int PIC_INITIAL_X = 100;
	const int PIC_INITIAL_Y = 100;

	const int BORDER_BOTTOM = WND_DEF_HEIGHT - PIC_HEIGHT - 40;
	const int BORDER_TOP = 0;
	const int BORDER_RIGHT = WND_DEF_WIDTH - PIC_WIDTH - 20;
	const int BORDER_LEFT = 0;
	
	// CODES
	const int RUS_LAYOUT = 68748313;
	const int ENG_LAYOUT = 67699721;
	
	const int VK_CAP_RUS_W = 1062;
	const int VK_RUS_W = 1094;

	const int VK_CAP_RUS_S = 1067;
	const int VK_RUS_S = 1099;

	const int VK_CAP_RUS_A = 1060;
	const int VK_RUS_A = 1092;

	const int VK_CAP_RUS_D = 1042;
	const int VK_RUS_D = 1074;
}
