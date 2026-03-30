#pragma once
#include <iostream>

void StartGame();
int CheckBoard(int pX, int pY);
void MoveRight();
void MoveLeft();
void MoveDown();
void MoveUp();
void UndoMove();
void RedoMove();
void DrawCell(int x, int y, int bg_color);