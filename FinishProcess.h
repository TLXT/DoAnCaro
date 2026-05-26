#pragma once
#include <iostream>
#include <conio.h>
#include <string>

void GarbageCollect();
void ExitGame();
int ProcessFinish(int pWhoWin);
int AskContinue();
int TestBoard();
int GetLastFinishResult();
void DrawFinishCelebrationScreen();
bool DrawFinishQuestion(const std::string& prompt);
