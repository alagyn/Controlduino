#define WIN32_LEAN_AND_MEAN

#include <serializer.h>

#include <calibration.h>
#include <controller.h>
#include <errorUtils.h>
#include <guiControl.h>

#include <iomanip>
#include <iostream>
#include <sstream>

constexpr unsigned BAUD_RATE = 9600;

int main(int argc, char* argv[])
{
    std::cout << "Starting GUI\n";
    bdd::ControlduinoGUI gui;

    // Load calibration
    std::cout << "Initializing Calibration System\n";
    bdd::Calibration calibration;

    std::cout << "Selecting COM Port\n";
    std::string comPort;
    try
    {
        comPort = gui.getComPort();
    }
    catch(bdd::GUIExit err)
    {
        std::cout << "GUI closed, exitting\n";
    }

    if(comPort.empty())
    {
        std::cout << "No COM Port Selected, exitting\n";
        return 0;
    }

    std::cout << "Initializing Serial Interface with port: " << comPort << "\n";
    bdd::Serializer serial(comPort, BAUD_RATE);

    // Init Read Manager
    bdd::ReadManager readMan;
    try
    {
        readMan.init(&serial);
    }
    catch(bdd::ReadError& err)
    {
        std::cout << "Error initializer serial manager, exitting\n";
        return 1;
    }

    // Init Controller
    std::cout << "Initializing ViGEm controller\n";
    bdd::Controller controller;

    gui.setCalib(&calibration);
    controller.init(&calibration);

    std::cout << "Initializing Complete\n";

    if(calibration.needsCalibrate)
    {
        std::cout << "Running Calibration\n";
        try
        {
            gui.runCalibration(&readMan);
            std::cout << "Calibraation Complete\n";
        }
        catch(bdd::GUIExit err)
        {
            std::cout << "GUI closed, exitting\n";
            return 0;
        }
    }

    // TODO remapping?
    bool run = true;
    try
    {
        while(run)
        {
            Ard_XInput state = readMan.updateState();

            controller.update(state);

            gui.setState(state);
            try
            {
                if(!gui.poll(bdd::GUIMode::Info))
                {
                    break;
                }
            }
            catch(bdd::GUIExit err)
            {
                std::cout << "GUI closed, exitting\n";
                return 0;
            }

            //Sleep(10);
        }
    }
    catch(bdd::ReadError err)
    {
        //PASS
    }

    std::cout << "Exitting\n";

    return 0;
}