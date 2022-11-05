
#include <serializer.h>

#include <chrono>
#include <sstream>
#include <strsafe.h>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

using namespace std;

namespace bdd {

    Serializer::Serializer(const std::string& portname, unsigned baudrate)
        : ctx()
        , port(ctx, portname)
        , error(false)
        , timedOut(false)
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

    void Serializer::timeout()
    {
        timedOut = true;
        port.cancel();
    }

    void Serializer::readHandler(const boost::system::error_code& ec)
    {
        temp_ec = ec;
    }

    void Serializer::readBytes(uint8_t* out, unsigned bytes, unsigned timeoutMs)
    {
        timedOut = false;

        // Must be called before every ctx.run()
        ctx.reset();

        boost::asio::deadline_timer timer(ctx);
        timer.expires_from_now(boost::posix_time::millisec(timeoutMs));
        timer.async_wait(boost::bind(&Serializer::timeout, this));

        boost::asio::async_read(port,
                                boost::asio::buffer(out, bytes),
                                boost::bind(&Serializer::readHandler, this, boost::asio::placeholders::error));

        // Run all async tasks to completion
        ctx.run();

        if(timedOut)
        {
            throw Timeout();
        }

        if(temp_ec)
        {
            std::cout << "Serializer::readBytes() Handler error: " << temp_ec.message() << "\n";
            throw ReadError();
        }
    }

    void Serializer::readBytes(uint8_t* out, unsigned bytes)
    {
        try
        {
            boost::asio::read(port, boost::asio::buffer(out, bytes));
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

    void Serializer::flush()
    {
        // Keeps reading until it starts timingout
        try
        {
            uint8_t xxx;
            while(true)
            {
                readBytes(&xxx, 1, 200);
            }
        }
        catch(Timeout err)
        {
            // Pass
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