#include <readManager.h>

#include <iostream>

namespace bdd {
    ReadManager::ReadManager(Serializer* serial)
        : serial(serial)
        , state()
    {
        XUSB_REPORT_INIT(&state);
        std::cout << "ReadManager: Starting\n";
        // Ensure it is stopped
        serial->write(STOP_RUN);
        serial->write(START_RUN);
    }

    ReadManager::~ReadManager()
    {
        std::cout << "ReadManager: Stopping\n";
        if(!serial->errored())
        {
            serial->write(STOP_RUN);
        }
    }

    XUSB_REPORT ReadManager::updateState()
    {
        serial->readBytes(buff, sizeof(Ard_XInput));

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