#pragma once
#include <Windows.h>
#include <string>

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

		void readByte(unsigned long timeoutMicroS, uint8_t* out);
		void write(const std::string& msg);
	};
}
