#pragma once

namespace ProjConst 
{
	const int WND_INI_WIDTH = 1000;
	const int WND_INI_HEIGHT = 800;
	const FLOAT FONT_DEFAULT_SIZE = 16;
	
	const wchar_t* WND_MAIN_CLASS_NAME = L"MainWnd";
	const wchar_t* WND_MAIN_CAPTION = L"Tables";
	const wchar_t* FONT_TYPE_FIXED = L"Fixed";
	const wchar_t* FONT_TYPE_FIXEDSYS = L"Fixedsys";
	const wchar_t* FONT_TYPE_GNU_UNIFONT = L"GNU unifont";
	const wchar_t* FONT_TYPE_PC_SCREEN_FONT = L"PC Screen Font";
	const wchar_t* FONT_TYPE_PROGGY_PROGRAMMING_FONTS = L"Proggy programming fonts";
	const wchar_t* FONT_TYPE_TERMINAL = L"TERMINAL";

	const COLORREF WND_MAIN_INI_COLOR = RGB(255, 255, 255);

	const int TEXT_FORMAT_DRAW = DT_LEFT | DT_WORDBREAK | DT_WORD_ELLIPSIS;
	const int TEXT_FORMAT_COUNT = DT_LEFT | DT_WORDBREAK | DT_CALCRECT | DT_WORD_ELLIPSIS;

	const int ROWS = 3;
	const int COLUMNS = 4;

	const LPCWSTR TEXT[]
	{
		L"Never gonna give you up",
		L"We're no strangers to love",
		L"You know the rules and so do I",
		L"A full commitment's what I'm thinking of",
		L"You wouldn't get this from any other guy",
		L"I just wanna tell you I'm feeling",
		L"Gotta make you understand",
		L"Never gonna give you up",
		L"Never gonna let you down",
		L"Never gonna run around and desert you",
		L"Never gonna make you cry",
		L"Never gonna say goodbye"
	};
}
