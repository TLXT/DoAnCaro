#pragma once
#include <windows.h>
#include <mutex>
#include <string>

using namespace std;

extern std::mutex consoleMutex;

void GotoXY(int x, int y);
void HideCursor();
void UnhideCursor();
void SetColor(int color, int bgColor);
void SetBgColor(int color, int bgColor);
void FixConsoleWindow();
void SetConsoleWindow(int width, int height);