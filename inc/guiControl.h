#pragma once
#include <exception>
#include <string>

typedef void (*DrawFunc)(void);

void startGui(DrawFunc drawFunc);

std::wstring selectComPort();

class GUIError : public std::exception
{

};
