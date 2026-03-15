#pragma once
#include <windows.h>
#include <mutex>
#include <string>

using namespace std;

extern std::mutex consoleMutex;

void GotoXY(int x, int y);
void hideCursor();
void unhideCursor();
void setColor(int color, int bgColor);
void setBgColor(int color, int bgColor);
void FixConsoleWindow();
void setConsoleWindow(int width, int height);