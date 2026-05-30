#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <algorithm>

using namespace std;


struct SaveFileInfo {
    string name;
    ULARGE_INTEGER time;
};

string SaveGame();
vector<string> GetSaveFiles();
string ChooseFileMenu();
bool LoadGame();
void ClearAllData();
bool loadPresent();