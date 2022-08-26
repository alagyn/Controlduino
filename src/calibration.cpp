#include "calibration.h"

#include <Windows.h>
#include <fstream>

#include "errorUtils.h"

constexpr wchar_t CAL_FILE[] = L"calibration.txt";

Calibration::Calibration()
	: lxMin(0)
	, lxWidth(0)
	, lyMin(0)
	, lyWidth(0)
	, lxDead(0)
	, lyDead(0)
	, rxMin(0)
	, rxWidth(0)
	, ryMin(0)
	, ryWidth(0)
	, rxDead(0)
	, ryDead(0)
{
	bool makeNew = false;
	if (GetFileAttributes(CAL_FILE) == INVALID_FILE_ATTRIBUTES)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			makeNew = true;
		}
		else
		{
			displayLastError(L"Calibration()");
			throw CalibrationError();
		}
	}

	if (makeNew)
	{
		// TODO run calibration process
	}
	else
	{
		std::ifstream f(CAL_FILE);
		f >> lxMin >> lxWidth >> lxDead;
		f >> lyMin >> lyWidth >> lyDead;
		f >> rxMin >> rxWidth >> rxDead;
		f >> ryMin >> ryWidth >> ryDead;
	}
}


constexpr int64_t RANGE_MAX = UINT16_MAX;

inline int16_t clamp(uint16_t in, const uint16_t min, const uint16_t width, const int16_t deadz)
{
	int64_t out = in;

	out = (RANGE_MAX * out) - (RANGE_MAX * min);
	out /= width;

	// center on zero
	out -= RANGE_MAX / 2;

	if (abs(out) < deadz)
	{
		out = 0;
	}

	return (int16_t)out;
}

int16_t Calibration::clampLX(const uint16_t in)
{
	return clamp(in, lxMin, lxWidth, lxDead);
}

int16_t Calibration::clampLY(const uint16_t in)
{
	return clamp(in, lyMin, lyWidth, lyDead);
}

int16_t Calibration::clampRX(const uint16_t in)
{
	return clamp(in, rxMin, rxWidth, rxDead);
}

int16_t Calibration::clampRY(const uint16_t in)
{
	return clamp(in, ryMin, ryWidth, ryDead);
}