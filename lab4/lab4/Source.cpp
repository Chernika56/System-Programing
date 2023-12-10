#include <Windows.h>
#include <iostream>
#include "Key.h"

int main()
{
	Key key(HKEY_CURRENT_USER, L"lab4\\Key");
	const BYTE data[]{ 101, 82, 16, 5, 19, 55, 15, 101 };
	int dataSize = 8;

	key.AddValue(L"123", REG_SZ, data, dataSize);
	key.Save();
	key.ReadFlags(L"123");

	RegCloseKey(key.GetKey());
	key.Find(HKEY_CURRENT_USER, L"lab4\\Key");

	return 0;
}
