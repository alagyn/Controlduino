#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Serializer.h"
#include "SerialErrors.h"
#include "ErrorUtils.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#pragma warning( push )
#pragma warning( disable : 6001)
//#pragma warning( disable : )
#include <ViGEm/Client.h>
#pragma warning ( pop )

#pragma comment(lib, "setupapi.lib")

int main(int argc, char* argv[])
{
	const auto client = vigem_alloc();
	if (!client)
	{
		displayError(L"main()", L"Cannot allocate ViGEm client");
		return 1;
	}

	auto ret = vigem_connect(client);
	if (!VIGEM_SUCCESS(ret))
	{
		std::wstringstream ss;
		ss << L"Cannot connect to ViGEm client, error code: 0x" << std::hex << ret;
		displayError(L"main()", ss.str().c_str());
		return 1;
	}

	const auto pad = vigem_target_x360_alloc();
	ret = vigem_target_add(client, pad);
	if (!VIGEM_SUCCESS(ret))
	{
		std::wstringstream ss;
		ss << L"Cannot allocate controller, error code: 0x" << std::hex << ret;
		displayError(L"main()", ss.str().c_str());
		return 1;
	}

	serializer::Serializer serial("COM4", CBR_9600);
	XUSB_REPORT state;
	XUSB_REPORT_INIT(&state);

	// TODO select com-device
	bool run = true;
	while (run)
	{
		uint8_t byte;
		serial.readByte(1000000, &byte);
		if (byte == 0x12)
		{
			std::cout << "Press\n";
			state.bLeftTrigger = 255;
		}
		else if (byte == 0x21)
		{
			std::cout << "Unpress\n";
			state.bLeftTrigger = 0;
		}
		else
		{
			std::cout << "ugh" << std::endl;
		}

		vigem_target_x360_update(client, pad, state);
	}

	vigem_target_remove(client, pad);
	vigem_target_free(pad);

	return 0;
}