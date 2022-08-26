#pragma once
#include <exception>
#include <string>

#include <GLFW/glfw3.h>

namespace bdd {

    class ControlduinoGUI
    {
    public:
        ControlduinoGUI();
        ~ControlduinoGUI();

        std::string getComPort();
        // TODO remap gui
    private:
        enum class Mode
        {
            None,
            Port
        };

        Mode mode;
        GLFWwindow* window;
        std::string port;

        bool fresh;

        void loop();
        bool poll();

        bool drawComSelect();
    };

    class GUIError : public std::exception
    {
    };
} //namespace bdd