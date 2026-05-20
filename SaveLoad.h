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
// 1. Cập nhật hàm lấy file: Đọc thuộc tính và đưa file mới nhất lên đầu
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