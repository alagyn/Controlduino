#pragma once
#include <exception>
#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <ViGEm/Common.h>
#include <Xinput.h>
#include <arduino_xinput.h>

namespace bdd {

    /**
     * @brief Handles analog axi calibration and arduino -> XInput conversion
     *
     */
    class Calibration
    {
    public:
        explicit Calibration();
        Calibration(Calibration&) = delete;

        int16_t clampLX(const uint16_t in);
        int16_t clampLY(const uint16_t in);

        int16_t clampRX(const uint16_t in);
        int16_t clampRY(const uint16_t in);

        XUSB_REPORT clampState(const Ard_XInput& ard_state);

        void writeCalibFile();

        bool needsCalibrate;

        uint16_t lxMin, lxWidth;
        uint16_t lyMin, lyWidth;
        int16_t lxDead, lyDead;

        uint16_t rxMin, rxWidth;
        uint16_t ryMin, ryWidth;
        int16_t rxDead, ryDead;
    };

    class CalibrationError : public std::exception
    {
    };
} //namespace bdd
