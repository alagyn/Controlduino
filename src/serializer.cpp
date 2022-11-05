
#include <serializer.h>

#include <chrono>
#include <sstream>
#include <strsafe.h>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/system_error.hpp>

#include <iostream>

using namespace std;

namespace bdd {

    Serializer::Serializer(const std::string& portname, unsigned baudrate)
        : ctx()
        , port(ctx, portname)
        , error(false)
    {
        using serial = boost::asio::serial_port_base;
        port.set_option(serial::baud_rate(baudrate));
        port.set_option(serial::parity(serial::parity::none));
        port.set_option(serial::stop_bits(serial::stop_bits::one));
        port.set_option(serial::character_size(8U));
    }

    Serializer::~Serializer()
    {
        port.close();
    }

    bool Serializer::errored()
    {
        return error;
    }

    void Serializer::readBytes(uint8_t* out, unsigned size)
    {
        try
        {
            boost::asio::read(port, boost::asio::buffer(out, size));
        }
        catch(const boost::wrapexcept<boost::system::system_error>& err)
        {
            std::cout << "Serializer::readBytes(): " << err.what();
            error = true;
            throw ReadError();
        }
    }

    void Serializer::write(const std::string& msg)
    {
        boost::asio::write(port, boost::asio::buffer(msg.c_str(), msg.length()));
    }

    void Serializer::write(const uint8_t* data, int len)
    {
        boost::asio::write(port, boost::asio::buffer(data, len));
    }

    void Serializer::write(const uint8_t data)
    {
        boost::asio::write(port, boost::asio::buffer(&data, 1));
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