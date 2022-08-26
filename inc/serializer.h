#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace serializer
{
	class Serializer
	{
	private:
		HANDLE handle;
		COMSTAT status;
		DWORD errors;
	public:
		explicit Serializer(const std::string& port, DWORD baudRate);
		Serializer(const Serializer&) = delete;
		~Serializer();

		void readBytes(unsigned long timeoutMicroS, uint8_t* out, DWORD bytes);
		void write(const std::string& msg);

		static void checkPorts(std::vector<std::wstring>& out);
	};
}
