#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h> 
#include <vector>

using namespace std;

string TypeFileName();
string SaveGame();
vector<string> GetSaveFiles();
string ChooseFileMenu();
bool LoadGame();//tao lai ban cu
void ClearAllData();
bool LoadGame(string filename);
void loadPresent();