#include <readManager.h>

#include <iostream>

namespace bdd {
    ReadManager::ReadManager(Serializer* serial)
        : serial(serial)
        , state()
    {
        XUSB_REPORT_INIT(&state);
        std::cout << "Starting\n";
        serial->write(START_RUN);
    }

    ReadManager::~ReadManager()
    {
        std::cout << "Stopping\n";
        serial->write(STOP_RUN);
    }

    XUSB_REPORT ReadManager::updateState()
    {
        serial->readBytes(10000, buff, sizeof(Ard_XInput));

        state.wButtons = ard_state.buttons;
        state.bLeftTrigger = ard_state.lTrigger;
        state.bRightTrigger = ard_state.rTrigger;

        state.sThumbLX = ard_state.lStickX;
        state.sThumbLY = ard_state.lStickY;
        state.sThumbRX = ard_state.rStickX;
        state.sThumbRY = ard_state.rStickY;
        /*
        state.sThumbLX = calibration.clampLX(ard_state.lStickX);
        state.sThumbLY = calibration.clampLY(ard_state.lStickY);
        state.sThumbRX = calibration.clampRX(ard_state.rStickX);
        state.sThumbRY = calibration.clampRY(ard_state.rStickY);
        */
        return state;
    }
} //namespace bdd