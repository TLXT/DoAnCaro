#pragma once
#include <string>

using namespace std;

void DrawPlayerInfo();
void UpdateTurnInfo();
void DrawStatusInfo(bool paused);
void DrawActionBar(int selectedAction = -1);
int GetActionCount();
