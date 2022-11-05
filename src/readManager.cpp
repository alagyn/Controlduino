#include <readManager.h>

#include <iostream>

namespace bdd {
    ReadManager::ReadManager()
        : serial(nullptr)
    {
    }

    ReadManager::~ReadManager()
    {
        std::cout << "ReadManager: Stopping\n";
        if(serial && !serial->errored())
        {
            serial->write(STOP_RUN);
            serial->flush();
        }
    }

    constexpr unsigned TIMEOUT = 200;
    constexpr unsigned ATTEMPTS = 20;

    void ReadManager::init(Serializer* serial)
    {
        std::cout << "ReadManager: Starting\n";
        this->serial = serial;

        // Ensure it is stopped
        serial->write(STOP_RUN);

        // Clear flush the buffer
        serial->flush();

        // Send shutdown signal until it is echoed or too many attempts
        uint8_t out = 0;
        for(int i = 0; i < ATTEMPTS && out != STOP_RUN; ++i)
        {
            serial->write(STOP_RUN);
            try
            {
                serial->readBytes(&out, 1, TIMEOUT);
            }
            catch(const Timeout& err)
            {
                // Pass
            }
        }

        if(out != STOP_RUN)
        {
            std::cout << "ReadManager() Controller not responding to initial shutdown\n";
            throw ReadError();
        }

        std::cout << "ReadManager() STOP_RUN sent\n";

        // Send start signal until is is echoed or too many attempts
        for(int i = 0; i < ATTEMPTS && out != START_RUN; ++i)
        {
            serial->write(START_RUN);
            try
            {
                serial->readBytes(&out, 1, TIMEOUT);
            }
            catch(const Timeout& err)
            {
                // Pass
            }
        }

        if(out != START_RUN)
        {
            // Send final stop just in case
            serial->write(STOP_RUN);
            std::cout << "ReadManager() Controller not responding to START_RUN\n";
            throw ReadError();
        }

        std::cout << "ReadManager() START_RUN sent\n";
    }

    Ard_XInput ReadManager::updateState()
    {
        serial->readBytes(buff, sizeof(Ard_XInput));

        return ard_state;
    }
} //namespace bdd