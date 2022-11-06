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
        /**
         * @brief Construct a new Serializer object
         *
         * @param portname The COM port to open
         * @param baudrate The buad rate
         */
        explicit Serializer(const std::string& portname, unsigned baudrate);
        Serializer(const Serializer&) = delete;

        /**
         * @brief Destroy the Serializer object and close serial port
         *
         */
        ~Serializer();

        /**
         * @brief Read a set number of bytes, or timeout. Throws bdd::Timeout
         *
         * @param out The output buffer
         * @param bytes The number of bytes to read, must be <= len(out)
         * @param timeoutMs The timeout in millisec
         */
        void readBytes(uint8_t* out, unsigned bytes, unsigned timeoutMs);

        /**
         * @brief Read set number of bytes, BLOCKING
         *
         * @param out The output buffer
         * @param bytes The number of bytes to read, must be <= len(out)
         */
        void readBytes(uint8_t* out, unsigned bytes);

        /**
         * @brief Convert a string to bytes then write to port
         *
         * @param msg The input message
         */
        void write(const std::string& msg);

        /**
         * @brief Write out binary data to port
         *
         * @param data The data
         * @param len The number of bytes
         */
        void write(const uint8_t* data, int len);

        /**
         * @brief Write out a sinlge byte of data
         *
         * @param data The input
         */
        void write(const uint8_t data);

        /**
         * @brief Attempts to flush the input buffer. Might block indefinitely if input device continues to
         * send more data
         */
        void flush();

        /**
         * @brief Check if the serial port has errored
         *
         * @return true if error has occured, do not try to use the port
         * @return false if no error has occurred
         */
        bool errored();

        /**
         * @brief Gets a list of the available serial devices and stores their names into the given vector
         *
         * @param out The output vector, will be cleared before insertion
         */
        static void checkPorts(std::vector<std::pair<std::string, std::string>>& out);
    };

    /**
     * @brief Base Serializer error
     *
     */
    class SerializerError : public std::exception
    {
    };

    /**
     * @brief Thrown when a reading error occurs, serial port should not be used again
     *
     */
    class ReadError : public SerializerError
    {
    };

    /**
     * @brief Throw when a timeout occurs, no assumptions can be made on the number of bytes read
     *
     */
    class Timeout : public SerializerError
    {
    };

} //namespace bdd
