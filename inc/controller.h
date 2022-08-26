#pragma once
#include <exception>
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
// Force these separate since Windows.h MUST come first
#include <ViGEm/Client.h>

#pragma comment(lib, "setupapi.lib")

#include "calibration.h"

namespace bdd {

    class Controller
    {
    private:
        PVIGEM_CLIENT client;
        PVIGEM_TARGET pad;

    public:
        explicit Controller();
        ~Controller();

        void update(const XUSB_REPORT& state);
    };

    class ControllerException : public std::exception
    {
    };

} //namespace bdd