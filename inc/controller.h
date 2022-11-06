#pragma once
#include <exception>

#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <ViGEm/Client.h>

#pragma comment(lib, "setupapi.lib")

#include "calibration.h"

namespace bdd {

    class Controller
    {
    private:
        PVIGEM_CLIENT client;
        PVIGEM_TARGET pad;
        Calibration* calib;

    public:
        explicit Controller();
        ~Controller();

        /**
         * @brief Initialize the controller
         *
         * @param calib The calibration to use
         */
        void init(Calibration* calib);

        /**
         * @brief Updates the controller's state using the arduino state
         *
         * @param ard_state The input state
         */
        void update(const Ard_XInput& ard_state);
    };

    class ControllerException : public std::exception
    {
    };

} //namespace bdd