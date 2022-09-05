#pragma once
#include <exception>
#include <string>

#include <Windows.h>

#include <GLFW/glfw3.h>
#include <ViGEm/Common.h>

#include <arduino_xinput.h>

namespace bdd {

    enum class GUIMode
    {
        None,
        Port,
        Info
    };

    class ControlduinoGUI
    {
    public:
        ControlduinoGUI();
        ~ControlduinoGUI();

        void setInfoPtr(XUSB_REPORT* infoPtr);
        std::string getComPort();

        bool poll(GUIMode mode);
        void loop(GUIMode mode);

    private:
        XUSB_REPORT* infoPtr;

        GLFWwindow* window;
        std::string port;

        bool fresh;

        // TODO remap gui
        bool drawComSelect();
        bool drawInfo();

        void drawInfo_Button(const char* title, uint16_t mask);
    };

    class GUIError : public std::exception
    {
    };
} //namespace bdd