#include <ErrorUtils.h>
#include <strsafe.h>
#include <Windows.h>

#include <sstream>
#include <iostream>

void displayError(const wchar_t* from, const wchar_t* msgBuff)
{
	size_t size = lstrlen(msgBuff) + lstrlen(from) + 40;

	wchar_t* displayBuff = new wchar_t[size];

	StringCchPrintfW(displayBuff,
		size,
		L"Error: %s: %s",
		from, msgBuff);

	std::wcout << L"Error: " << from << L", " << msgBuff << std::endl;
	MessageBox(NULL, (LPCTSTR)displayBuff, TEXT("Error"), MB_OK);
}

void displayLastError(const wchar_t* from)
{
	// Display the error message and exit the process
	wchar_t* msgBuff;
	DWORD dw = GetLastError();

	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(wchar_t*)&msgBuff,
		0, NULL);

	displayError(from, msgBuff);
	LocalFree(msgBuff);
}
