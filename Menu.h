#pragma once
#include <iostream>
#include <conio.h>
#include <string>

using namespace std;

int GenericMenu(string options[], int size, string title);
int MainMenu();
int PlayGameMenu();
int DifficultyMenu();
int SettingsMenu();
int GameMenu();
void InputPlayerNames(bool isBotMode);