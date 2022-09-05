#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <arduino_xinput.h>
#include <calibration.h>
#include <controller.h>
#include <errorUtils.h>
#include <guiControl.h>
#include <serializer.h>

#include <iomanip>
#include <iostream>
#include <sstream>

#include <ViGEm/Common.h>

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
    std::string comPort = gui.getComPort();
    //std::string comPort = "COM4";

    if(comPort.empty())
    {
        std::cout << "No COM Port Selected, exitting\n";
        return 0;
    }

    std::cout << "Initilizing Serial Interface\n";
    bdd::Serializer serial;
    try
    {
        serial.openPort(comPort, CBR_9600);
    }
    catch(bdd::SerializerError err)
    {
        std::cout << "Cannot open Serial interface\n";
        return 1;
    }

    // Init Controller
    std::cout << "Initializing ViGEm controller\n";
    bdd::Controller controller;

    // Load calibration
    std::cout << "Initializing Calibration System\n";
    bdd::Calibration calibration;

    std::cout << "Initializing Controller State\n";
    XUSB_REPORT state;
    XUSB_REPORT_INIT(&state);

    gui.setInfoPtr(&state);

    Ard_XInput ard_state;
    uint8_t* buff = (uint8_t*)&ard_state;

    std::cout << "Initializing Complete\n";

    // TODO exitting
    // TODO remapping
    bool run = true;
    while(run)
    {
        try
        {
            serial.readBytes(10000, buff, sizeof(Ard_XInput));
        }
        catch(bdd::SerializerError err)
        {
            break;
        }

        state.wButtons = ard_state.buttons;
        state.bLeftTrigger = ard_state.lTrigger;
        state.bRightTrigger = ard_state.rTrigger;

        state.sThumbLX = ard_state.lStickX;
        /*
        state.sThumbLX = calibration.clampLX(ard_state.lStickX);
        state.sThumbLY = calibration.clampLY(ard_state.lStickY);
        state.sThumbRX = calibration.clampRX(ard_state.rStickX);
        state.sThumbRY = calibration.clampRY(ard_state.rStickY);
        */

        controller.update(state);

        if(!gui.poll(bdd::GUIMode::Info))
        {
            break;
        }
        //Sleep(10);
    }

    std::cout << "Exitting\n";

    return 0;
}