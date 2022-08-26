#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Serializer.h"
#include "SerialErrors.h"
#include "ErrorUtils.h"
#include <arduino_xinput.h>
#include "calibration.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include <ViGEm/Client.h>
#include "controller.h"

int main(int argc, char* argv[])
{
	// TODO select com-device
	serializer::Serializer serial("COM4", CBR_9600);

	// Init Controller
	Controller controller;

	// Load calibration
	Calibration calibration;

	XUSB_REPORT state;
	XUSB_REPORT_INIT(&state);

	Ard_XInput ard_state;
	uint8_t* buff = (uint8_t*)&ard_state;

	
	// TODO exitting
	// TODO remapping
	bool run = true;
	while (run)
	{
		serial.readBytes(10000, buff, sizeof(Ard_XInput));
		
		state.wButtons = ard_state.buttons;
		state.bLeftTrigger = ard_state.lTrigger;
		state.bRightTrigger = ard_state.rTrigger;

		state.sThumbLX = calibration.clampLX(ard_state.lStickX);
		state.sThumbLY = calibration.clampLY(ard_state.lStickY);
		state.sThumbRX = calibration.clampRX(ard_state.rStickX);
		state.sThumbRY = calibration.clampRY(ard_state.rStickY);

		controller.update(state);
	}

	return 0;
}