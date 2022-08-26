#include <ErrorUtils.h>
#include <strsafe.h>
#include <Windows.h>

#include <sstream>
#include <iostream>

void displayError(const char* from, const char* msgBuff)
{
	size_t size = lstrlenA(msgBuff) + lstrlenA(from) + 40;

	char* displayBuff = new char[size];

	StringCchPrintfW(displayBuff,
		size,
		L"Error: %s: %s",
		from, msgBuff);

	std::wcout << L"Error: " << from << L", " << msgBuff << std::endl;
	MessageBoxA(NULL, (LPCTSTR)displayBuff, TEXT("Error"), MB_OK);
}

void displayLastError(const char* from)
{
	// Display the error message and exit the process
	char* msgBuff;
	DWORD dw = GetLastError();

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		&msgBuff,
		0, NULL);

	displayError(from, msgBuff);
	LocalFree(msgBuff);
}
