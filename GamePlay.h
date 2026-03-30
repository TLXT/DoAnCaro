#pragma once
#include <iostream>

void StartGame();
int CheckBoard(int pX, int pY);
void MoveRight();
void MoveLeft();
void MoveDown();
void MoveUp();
void DrawCell(int x, int y, int bg_color);
void ProcessMove(int _COMMAND,bool validEnter, bool& isPlaying);
void PlayRandomMove();// them vao random