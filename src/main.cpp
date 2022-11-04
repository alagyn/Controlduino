#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <calibration.h>
#include <controller.h>
#include <errorUtils.h>
#include <guiControl.h>
#include <serializer.h>

#include <iomanip>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
    std::cout << "Starting GUI\n";
    // TODO select com-device
    bdd::ControlduinoGUI gui;

    // TODO remove
    /*
    XUSB_REPORT xxx;
    XUSB_REPORT_INIT(&xxx);

    gui.setInfoPtr(&xxx);
    gui.loop(bdd::GUIMode::Info);
    */

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
    bdd::Serializer serial;
    try
    {
        serial.openPort(comPort, CBR_9600);
    }
    catch(const bdd::SerializerError& err)
    {
        std::cout << "Cannot open Serial interface\n";
        return 1;
    }

    try
    {
        serial.write('A');
        uint8_t data;
        serial.readBytes(10000, &data, 1);
        std::cout << data << "\n";
    }
    catch(bdd::Timeout err)
    {
        std::cout << "Timeout\n";
    }

    return 1;

    // Init Read Manager
    bdd::ReadManager readMan(&serial);

    // Init Controller
    std::cout << "Initializing ViGEm controller\n";
    bdd::Controller controller;

    // Load calibration
    std::cout << "Initializing Calibration System\n";
    bdd::Calibration calibration;

    gui.setCalib(&calibration);

    std::cout << "Initializing Controller State\n";

    std::cout << "Initializing Complete\n";

    if(calibration.needsCalibrate)
    {
        try
        {
            gui.runCalibration(&readMan);
        }
        catch(bdd::GUIExit err)
        {
            std::cout << "GUI closed, exitting\n";
            return 0;
        }
    }

    // TODO remapping?
    bool run = true;
    while(run)
    {
        XUSB_REPORT state = readMan.updateState();
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

    std::cout << "Exitting\n";

    return 0;
}