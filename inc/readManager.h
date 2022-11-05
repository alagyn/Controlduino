#pragma once

#include <serializer.h>

#include <arduino_xinput.h>

namespace bdd {

    /**
     * @brief Controlduino wrapper around the Serializer.
     *
     * Sets handles reading arduino state as well as sending start and stop signals
     * to the arduino
     */
    class ReadManager
    {
    public:
        explicit ReadManager();
        ReadManager(const ReadManager&) = delete;

        /**
         * @brief Initializes the arduino connection
         *
         * @param serial The serializer
         */
        void init(Serializer* serial);

        /**
         * @brief Destroy the Read Manager object and shut down the serial connection
         *
         */
        ~ReadManager();

        /**
         * @brief Poll and return the arduino's state
         *
         * @return Ard_XInput
         */
        Ard_XInput updateState();

    private:
        Serializer* serial;

        Ard_XInput ard_state;
        uint8_t* buff = (uint8_t*)&ard_state;
    };
} //namespace bdd
