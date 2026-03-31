#pragma once
#include <iostream>
#include <vector>
using namespace std;

bool AskForReplay();
void ChooseReplaySpeed(float& speed, int& delay);
void PlayReplay(int delay);
void RedrawBoard(int step);
void HandleReplayOption();