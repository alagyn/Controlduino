#include "calibration.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

#include "errorUtils.h"
#include <iomanip>

constexpr char CAL_FILE[] = "calibration.txt";

namespace bdd {

    constexpr int32_t RANGE_MAX = UINT16_MAX;

    uint16_t checkRange(uint16_t min, uint16_t range)
    {
        if(min + (uint32_t)range > RANGE_MAX)
        {
            return 1023 - min;
        }

        return range;
    }

    Calibration::Calibration()
        : needsCalibrate(false)
        , lxMin(0)
        , lxRange(0)
        , lyMin(0)
        , lyRange(0)
        , lxDead(0)
        , lyDead(0)
        , rxMin(0)
        , rxRange(0)
        , ryMin(0)
        , ryRange(0)
        , rxDead(0)
        , ryDead(0)
    {
        boost::filesystem::path calFilePath(CAL_FILE);

        if(!boost::filesystem::exists(calFilePath))
        {
            needsCalibrate = true;
            lxMin = 0;
            lxRange = RANGE_MAX;
            lyMin = 0;
            lyRange = RANGE_MAX;
            rxMin = 0;
            rxRange = RANGE_MAX;
            ryMin = 0;
            ryRange = RANGE_MAX;
        }
        else
        {
            std::ifstream f(CAL_FILE);
            f >> lxMin >> lxRange >> lxDead;
            lxRange = checkRange(lxMin, lxRange);

            f >> lyMin >> lyRange >> lyDead;
            lyRange = checkRange(lyMin, lyRange);

            f >> rxMin >> rxRange >> rxDead;
            rxRange = checkRange(rxMin, rxRange);

            f >> ryMin >> ryRange >> ryDead;
            ryRange = checkRange(ryMin, ryRange);

            std::cout << "Loaded Calibration:\n";
            std::cout << "LX Min: " << std::setw(4) << lxMin << ", Range: " << std::setw(4) << lxRange
                      << ", Dead: " << std::setw(4) << lxDead << "\n";
            std::cout << "LY Min: " << std::setw(4) << lyMin << ", Range: " << std::setw(4) << lyRange
                      << ", Dead: " << std::setw(4) << lyDead << "\n";
            std::cout << "RX Min: " << std::setw(4) << rxMin << ", Range: " << std::setw(4) << rxRange
                      << ", Dead: " << std::setw(4) << rxDead << "\n";
            std::cout << "RY Min: " << std::setw(4) << ryMin << ", Range: " << std::setw(4) << ryRange
                      << ", Dead: " << std::setw(4) << ryDead << "\n";

            writeCalibFile();
        }
    }

    int16_t clamp(const uint16_t in, const uint16_t min, const uint16_t width, const int16_t deadz)
    {
        // Cast to 32 bit to not lose data
        int32_t out = in;

        // center input range around zero
        out -= min + (width / 2);

        // scale to new range
        out *= (RANGE_MAX / width);

        // Filter deadzone
        if(abs(out) < deadz)
        {
            out = 0;
        }

        // Clamp to int 16 range
        out = out > INT16_MIN ? out : INT16_MIN;
        out = out < INT16_MAX ? out : INT16_MAX;

        // Cast back to 16 bit
        return (int16_t)out;
    }

    int16_t Calibration::clampLX(const uint16_t in)
    {
        return clamp(in, lxMin, lxRange, lxDead);
    }

    int16_t Calibration::clampLY(const uint16_t in)
    {
        return clamp(in, lyMin, lyRange, lyDead);
    }

    int16_t Calibration::clampRX(const uint16_t in)
    {
        return clamp(in, rxMin, rxRange, rxDead);
    }

    int16_t Calibration::clampRY(const uint16_t in)
    {
        return clamp(in, ryMin, ryRange, ryDead);
    }

    XUSB_REPORT Calibration::clampState(const Ard_XInput& ard_state)
    {
        XUSB_REPORT out = {0};

        out.wButtons = ard_state.buttons;
        out.bLeftTrigger = ard_state.lTrigger;
        out.bRightTrigger = ard_state.rTrigger;

        out.sThumbLX = clampLX(ard_state.lStickX);
        out.sThumbLY = clampLY(ard_state.lStickY);
        out.sThumbRX = clampRX(ard_state.rStickX);
        out.sThumbRY = clampRY(ard_state.rStickY);

        return out;
    }

    void Calibration::writeCalibFile()
    {
        std::ofstream f(CAL_FILE);
        f << lxMin << " " << lxRange << " " << lxDead << "\n";
        f << lyMin << " " << lyRange << " " << lyDead << "\n";
        f << rxMin << " " << rxRange << " " << rxDead << "\n";
        f << ryMin << " " << ryRange << " " << ryDead << "\n";
    }

    void Calibration::setLX(uint16_t min, uint16_t range)
    {
        lxMin = min;
        lxRange = checkRange(min, range);
    }

    void Calibration::setLXDead(uint16_t dead)
    {
        lxDead = dead;
    }

    void Calibration::setLY(uint16_t min, uint16_t range)
    {
        lyMin = min;
        lyRange = checkRange(min, range);
    }

    void Calibration::setLYDead(uint16_t dead)
    {
        lyDead = dead;
    }

    void Calibration::setRX(uint16_t min, uint16_t range)
    {
        rxMin = min;
        rxRange = checkRange(min, range);
    }

    void Calibration::setRXDead(uint16_t dead)
    {
        rxDead = dead;
    }

    void Calibration::setRY(uint16_t min, uint16_t range)
    {
        ryMin = min;
        ryRange = checkRange(min, range);
    }

    void Calibration::setRYDead(uint16_t dead)
    {
        ryDead = dead;
    }

    uint16_t Calibration::getLXMin()
    {
        return lxMin;
    }

    uint16_t Calibration::getLXRange()
    {
        return lxRange;
    }

    uint16_t Calibration::getLXDead()
    {
        return lxDead;
    }

    uint16_t Calibration::getLYMin()
    {
        return lyMin;
    }

    uint16_t Calibration::getLYRange()
    {
        return lyRange;
    }

    uint16_t Calibration::getLYDead()
    {
        return lyDead;
    }

    uint16_t Calibration::getRXMin()
    {
        return rxMin;
    }

    uint16_t Calibration::getRXRange()
    {
        return rxRange;
    }

    uint16_t Calibration::getRXDead()
    {
        return rxDead;
    }

    uint16_t Calibration::getRYMin()
    {
        return ryMin;
    }

    uint16_t Calibration::getRYRange()
    {
        return ryRange;
    }

    uint16_t Calibration::getRYDead()
    {
        return ryDead;
    }

} //namespace bdd