#pragma once
#include <iostream>
#include <conio.h>
#include <string>
#include "character.h"

using namespace std;

int GenericMenu(string options[], int size, string title);
int GenericCharacterMenu(string options[], int size, string title);
int MainMenu();
int PlayGameMenu();
int DifficultyMenu();
int SettingsMenu();
int GameMenu();
int CharacterSelectionMenu();
void InputPlayerNames(bool isBotMode);