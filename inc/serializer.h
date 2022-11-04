#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace bdd {
    class Serializer
    {
    private:
        HANDLE handle;
        COMSTAT status;
        DWORD errors;

    public:
        explicit Serializer();
        Serializer(const Serializer&) = delete;
        ~Serializer();

        void openPort(const std::string& port, DWORD baudRate);

        void readBytes(unsigned long timeoutMicroS, uint8_t* out, DWORD bytes);
        void write(const std::string& msg);
        void write(const uint8_t* data, int len);
        void write(const uint8_t data);

        static void checkPorts(std::vector<std::pair<std::string, std::string>>& out);
    };

    class SerializerError : public std::exception
    {
    };

    class InitError : public SerializerError
    {
    };

    class ReadError : public SerializerError
    {
    };

    class ReadEOF : public SerializerError
    {
    };

    class Timeout : public SerializerError
    {
    };

    class WriteError : public SerializerError
    {
    };
} //namespace bdd
