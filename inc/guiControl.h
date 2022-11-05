#pragma once
#include <exception>
#include <string>

#include <readManager.h>

#include <GLFW/glfw3.h>

#include <arduino_xinput.h>
#include <calibration.h>

namespace bdd {

    enum class GUIMode
    {
        None,
        Port,
        Info,
        Calib
    };

    class ControlduinoGUI
    {
    public:
        ControlduinoGUI();
        ~ControlduinoGUI();

        void setCalib(Calibration* calib);
        void setState(const Ard_XInput& state);
        std::string getComPort();

        void runCalibration(ReadManager* rm);

        bool poll(GUIMode mode);
        void loop(GUIMode mode);

    private:
        Ard_XInput state;
        Calibration* calib;

        GLFWwindow* window;
        std::string port;

        bool fresh;

        // TODO remap gui
        bool drawComSelect();
        bool drawInfo();
        bool drawCalib();

        void drawInfo_Button(const char* title, uint16_t mask);
    };

    class GUIError : public std::exception
    {
    };

    class GUIExit : public std::exception
    {
    };
} //namespace bdd