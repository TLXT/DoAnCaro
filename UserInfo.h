#pragma once
#include <string>

using namespace std;

void DrawPlayerInfo();
void UpdateTurnInfo();
void DrawStatusInfo(bool paused);
void DrawActionBar(int selectedAction = -1);
void DrawTimerBox(const string& text, int color);
void DrawTurnTimers(int secondsLeft);
void PrintHudTextWithBg(int x, int y, const string& text, int color);
int GetActionCount();
