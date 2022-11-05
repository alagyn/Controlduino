#pragma once

#include <serializer.h>

#include <ViGEm/Common.h>
#include <arduino_xinput.h>

namespace bdd {
    class ReadManager
    {
    public:
        explicit ReadManager(Serializer* serial);
        ReadManager(const ReadManager&) = delete;

        ~ReadManager();

        XUSB_REPORT updateState();

    private:
        Serializer* serial;
        XUSB_REPORT state;

        Ard_XInput ard_state;
        uint8_t* buff = (uint8_t*)&ard_state;
    };
} //namespace bdd
