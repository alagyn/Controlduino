#pragma once

#include <memory>
#include <string>
#include <vector>

#include <SDKDDKVer.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>

namespace bdd {

    class Serializer
    {
    private:
        boost::asio::io_context ctx;
        boost::asio::serial_port port;

        bool error;
        bool timedOut;
        boost::system::error_code temp_ec;

        void timeout();
        void readHandler(const boost::system::error_code& ec);

    public:
        explicit Serializer(const std::string& portname, unsigned baudrate);
        Serializer(const Serializer&) = delete;
        ~Serializer();

        void openPort(const std::string& portName, unsigned baudrate);

        // Blocking with timeout
        void readBytes(uint8_t* out, unsigned bytes, unsigned timeoutMs);

        // Blocking
        void readBytes(uint8_t* out, unsigned bytes);
        void write(const std::string& msg);
        void write(const uint8_t* data, int len);
        void write(const uint8_t data);

        void flush();

        bool errored();

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
