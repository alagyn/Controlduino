#pragma once
#include <exception>
#include <string>

namespace bdd {

    class ControlduinoGUI
    {
    public:
        ControlduinoGUI();
        ~ControlduinoGUI();

        std::string getComPort();
        // TODO remap
    };

    class GUIError : public std::exception
    {
    };
} //namespace bdd