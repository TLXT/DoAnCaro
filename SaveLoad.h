#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h> 
#include <vector>
#include <algorithm>

using namespace std;

// ==============================================================
// Cập nhật cấu trúc lấy file: Đọc thuộc tính và đưa file mới lên đầu
// ==============================================================
struct SaveFileInfo {
    string name;
    ULARGE_INTEGER time;
};

string TypeFileName();
string SaveGame();
vector<string> GetSaveFiles();
string ChooseFileMenu();
bool LoadGame();
void ClearAllData();
bool loadPresent();