#include <errorUtils.h>
#include <serializer.h>

#include <chrono>
#include <sstream>
#include <strsafe.h>

using namespace std;

namespace bdd {

    Serializer::Serializer()
        : handle(nullptr)
        , status()
        , errors()
    {
    }

    void Serializer::openPort(const string& port, DWORD baudRate)
    {
        std::stringstream ss;
        ss << "\\\\.\\" << port;
        handle = CreateFileA(static_cast<LPCSTR>(ss.str().c_str()),
                             GENERIC_READ | GENERIC_WRITE,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

        if(handle == INVALID_HANDLE_VALUE)
        {
            displayLastError("CreateFile");
            throw InitError();
        }

        DCB serialParams = {0};
        serialParams.DCBlength = sizeof(DCB);

        if(!GetCommState(handle, &serialParams))
        {
            displayLastError("GetCommState");
            throw InitError();
        }

        serialParams.fBinary = true;
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
            // TODO handle busy wait?
            ClearCommError(handle, &errors, &status);

            if(status.fEof)
            {
                throw ReadEOF();
            }

            if(status.cbInQue > 0)
            {
                if(!ReadFile(handle, out, size, nullptr, nullptr))
                {
                    displayLastError("Serial::readByte()");
                    throw ReadError();
                }

                return;
            }
        }

        throw Timeout();
    }

    void Serializer::write(const std::string& msg)
    {
        ClearCommError(handle, &errors, &status);

        if(!WriteFile(handle, msg.c_str(), (DWORD)msg.size(), nullptr, nullptr))
        {
            displayLastError("Serial::write()");
            throw WriteError();
        }
    }

    void Serializer::write(const uint8_t* data, int len)
    {
        ClearCommError(handle, &errors, &status);

        DWORD written;
        if(!WriteFile(handle, data, (DWORD)len, nullptr, nullptr))
        {
            displayLastError("Serial::write()");
            throw WriteError();
        }
    }

    void Serializer::write(const uint8_t data)
    {
        ClearCommError(handle, &errors, &status);

        DWORD written;
        if(!WriteFile(handle, &data, (DWORD)1, &written, nullptr))
        {
            displayLastError("Serial::write()");
            throw WriteError();
        }

        if(written != 1)
        {
            std::stringstream ss;
            ss << "Wrong bytes written, expected 1, got " << written;
            displayError("Serial::write()", ss.str().c_str());
            throw WriteError();
        }
    }

    void Serializer::checkPorts(std::vector<std::pair<std::string, std::string>>& out)
    {
        constexpr size_t BUF_SIZE = 1000;
        char path[BUF_SIZE];

        out.clear();

        for(size_t i = 0; i < 255; ++i)
        {
            std::stringstream ss;
            ss << "COM" << i;
            DWORD ret = QueryDosDeviceA(ss.str().c_str(), path, BUF_SIZE);
            if(ret != 0)
            {
                out.emplace_back(std::make_pair(ss.str(), path));
            }
        }
    }
} //namespace bdd