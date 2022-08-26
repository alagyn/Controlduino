#include <Serializer.h>
#include <SerialErrors.h>
#include <ErrorUtils.h>

#include <strsafe.h>
#include <chrono>
#include <sstream>

using namespace std;
using namespace serializer;

Serializer::Serializer(const string& port, DWORD baudRate)
	: handle()
	, status()
	, errors()
{
	handle = CreateFileA(
		static_cast<LPCSTR>(port.c_str()),
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
	);

	if(handle == INVALID_HANDLE_VALUE)
	{
		displayLastError("CreateFile");
		throw InitError();
	}

	DCB serialParams = {0};

	if(!GetCommState(handle, &serialParams))
	{
		displayLastError("GetCommState");
		throw InitError();
	}

	serialParams.BaudRate = baudRate;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.Parity = NOPARITY;
	serialParams.fDtrControl = DTR_CONTROL_ENABLE;

	if(!SetCommState(handle, &serialParams))
	{
		displayLastError("SetCommState");
		throw InitError();
	}

	PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

Serializer::~Serializer()
{
	CloseHandle(handle);
}

void Serializer::readBytes(unsigned long timeoutMs, uint8_t* out, DWORD size)
{
	using clock = std::chrono::high_resolution_clock;

	auto start = clock::now();

	while(chrono::duration_cast<chrono::milliseconds>(clock::now() - start).count() < timeoutMs)
	{
		ClearCommError(handle, &errors, &status);

		if(status.fEof)
		{
			throw ReadEOF();
		}

		if(status.cbInQue > 0)
		{
			if(!ReadFile(handle, out, size, nullptr, nullptr))
			{
				displayLastError("Serial::readByte");
				throw ReadError();
			}

			return;
		}
	}

	throw Timeout();
}

void Serializer::write(const std::string& msg)
{
	if(!WriteFile(handle, msg.c_str(), (DWORD)msg.size(), nullptr, nullptr))
	{
		displayLastError("Serial::write()");
		throw WriteError();
	}
}

void Serializer::checkPorts(std::vector<std::wstring>& out)
{
	constexpr size_t BUF_SIZE = 1000;
	char path[BUF_SIZE];

	out.clear();

	for (size_t i = 0; i < 255; ++i)
	{
		std::stringstream ss;
		ss << "COM" << i;
		DWORD ret = QueryDosDeviceA(ss.str().c_str(), path, BUF_SIZE);
		if (ret != 0)
		{
			out.emplace_back(path);
		}
	}
}