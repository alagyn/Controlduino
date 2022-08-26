#pragma once
#include <exception>
#define WIN32_LEAN_AND_MEAN
#include <ViGEm/Client.h>
#include <Windows.h>
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