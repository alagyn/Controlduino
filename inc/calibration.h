#pragma once
#include <stdint.h>
#include <exception>

class Calibration
{
public:
	explicit Calibration();
	Calibration(Calibration&) = delete;

	int16_t clampLX(const uint16_t in);
	int16_t clampLY(const uint16_t in);
	
	int16_t clampRX(const uint16_t in);
	int16_t clampRY(const uint16_t in);
private:

	uint16_t lxMin, lxWidth;
	uint16_t lyMin, lyWidth;
	int16_t lxDead, lyDead;

	uint16_t rxMin, rxWidth;
	uint16_t ryMin, ryWidth;
	int16_t rxDead, ryDead;
};

class CalibrationError : public std::exception
{

};