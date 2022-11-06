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
    private:
        uint16_t lxMin, lxRange;
        uint16_t lyMin, lyRange;
        int16_t lxDead, lyDead;

        uint16_t rxMin, rxRange;
        uint16_t ryMin, ryRange;
        int16_t rxDead, ryDead;

    public:
        explicit Calibration();
        Calibration(Calibration&) = delete;

        int16_t inline clampLX(const uint16_t in);
        int16_t inline clampLY(const uint16_t in);

        int16_t inline clampRX(const uint16_t in);
        int16_t inline clampRY(const uint16_t in);

        XUSB_REPORT clampState(const Ard_XInput& ard_state);

        void setLX(uint16_t min, uint16_t range);
        void setLXDead(uint16_t dead);
        void setLY(uint16_t min, uint16_t range);
        void setLYDead(uint16_t dead);
        void setRX(uint16_t min, uint16_t range);
        void setRXDead(uint16_t dead);
        void setRY(uint16_t min, uint16_t range);
        void setRYDead(uint16_t dead);

        // // //
        uint16_t getLXMin();
        uint16_t getLXRange();
        uint16_t getLXDead();
        uint16_t getLYMin();
        uint16_t getLYRange();
        uint16_t getLYDead();
        uint16_t getRXMin();
        uint16_t getRXRange();
        uint16_t getRXDead();
        uint16_t getRYMin();
        uint16_t getRYRange();
        uint16_t getRYDead();

        void writeCalibFile();

        bool needsCalibrate;
    };

    class CalibrationError : public std::exception
    {
    };
} //namespace bdd
