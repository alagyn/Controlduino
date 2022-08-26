#pragma once
#include <exception>
#include <string>

typedef void (*DrawFunc)(void);

void startGui(DrawFunc drawFunc);

std::string selectComPort();

class GUIError : public std::exception
{

};
