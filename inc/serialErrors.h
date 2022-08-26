#pragma once
#include <exception>

namespace bdd {
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
