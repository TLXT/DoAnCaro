#include "SaveLoad.h"
#include "GameStatus.h"
#include "ControlConsole.h"
#include "DrawBoard.h"
#include "GamePlay.h"
#include "UserInfo.h"
#include "DrawBackground.hpp"
using namespace std;

string TypeFileName() {
    string res = "";
    while (true) {
        char c = _getch();

        if (c == 27) { // BẤM ESC
            return "";
        }
        else if (c == 13 && res.length() > 0) {
            break;
        }
        else if (c == 8) { // Bấm Backspace
            if (res.length() > 0) {
                res.pop_back();
                cout << "\b \b";
            }
        }
        else if (isalnum(c) || c == '-' || c == '_') {
            res += c;
            cout << c;
        }
    }
    return res;
}
string SaveGame() {
    string filename;

    system("cls");              // Xóa bàn cờ đi
    DrawLoadgameBackground();   // Vẽ nền Save/Load lên

    while (true) {
        GotoXY(5, 27);
        cout << "                                                                    ";

        GotoXY(5, 27);
        //SetColor(12, 15);
        printf("\x1b[38;2;255;50;50m\x1b[48;2;20;20;20m");
        cout << "Nhap ten file de luu: ";

        UnhideCursor();
        filename = TypeFileName();
        HideCursor();

        if (filename == "") {
            GotoXY(5, 27); cout << "                                                                    ";
            GotoXY(5, 28); cout << "                                                                    ";
            SetColor(0, 15);
            GotoXY(_X, _Y);
            return"";
        }

        ifstream checkFile(filename + ".caro");
        if (checkFile.is_open()) {
            checkFile.close();

            GotoXY(5, 28);
            SetColor(12, 15); // Màu đỏ
            cout << "Ten file da ton tai! Vui long nhap ten khac... (Nhan phim bat ky)";
            _getch();

            GotoXY(5, 28);
            cout << "                                                                    ";
        }
        else {
            break;
        }
    }

    ofstream outFile(filename + ".caro");
    if (outFile.is_open()) {
        outFile << _TURN << " " << _X << " " << _Y << endl;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                outFile << _A[i][j].c << " ";
            }
            outFile << endl;
        }
        outFile << currentStep << endl; //lưu số bước hiện tại
        for (int i = 0; i < currentStep; i++) {
            //lưu tọa độ dòng, cột và loại quân cờ (-1 hoặc 1) của từng bước đi
            outFile << moveHistory[i].row << " " << moveHistory[i].col << " " << moveHistory[i].c << endl;
        }
        outFile.close();

        GotoXY(5, 28);
        SetColor(10, 15);
        cout << "Luu thanh cong! Nhan phim bat ky de tiep tuc...";
        return filename;
    }
    else {
        GotoXY(5, 28);
        SetColor(12, 15);
        cout << "Loi tao file! Nhan phim bat ky...";
    }

    _getch();

    GotoXY(5, 27); cout << "                                                                    ";
    GotoXY(5, 28); cout << "                                                                    ";
    SetColor(0, 15);
    GotoXY(_X, _Y);
}

vector<string> GetSaveFiles() {
    vector<string> files;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA("*.caro", &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            string fileName = findFileData.cFileName;
            fileName = fileName.substr(0, fileName.find_last_of("."));
            files.push_back(fileName);
        } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    return files;
}


bool LoadGame() {
    string filename = ChooseFileMenu();

    system("cls"); 
    DrawLoadgameBackground();

    if (filename == "") {
        return false;
    }

    ifstream inFile(filename + ".caro");
    if (inFile.is_open()) {
        inFile >> _TURN >> _X >> _Y;

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                _A[i][j].x = 4 * j + LEFT + 2;
                _A[i][j].y = 2 * i + TOP + 1;
                inFile >> _A[i][j].c;
            }
        }
        moveHistory.clear(); //xóa lịch sử cũ trước khi load
        if (inFile >> currentStep) { //kiểm tra xem file save có dữ liệu lịch sử không
            for (int i = 0; i < currentStep; i++) {
                MoveNode node;
                inFile >> node.row >> node.col >> node.c;
                moveHistory.push_back(node);
            }
        }
        else {
            currentStep = 0; //tránh lỗi crash nếu đọc nhầm file save cũ (file được tạo trước khi sửa code)
        }
        inFile.close();

        system("cls");
        DrawIngameBackground();
        DrawBoard(BOARD_SIZE);
        DrawPlayerInfo();
        UpdateTurnInfo();

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (_A[i][j].c != 0) {
                    DrawCell(_A[i][j].x, _A[i][j].y, 15);
                } 
            }
        }
        DrawCell(_X, _Y, 11);

        return true;
    }
    else {
        system("cls");
        DrawLoadgameBackground();
        GotoXY(40, 15);
        //SetColor(12, 15);
        printf("\x1b[38;2;255;50;50m\x1b[48;2;20;20;20m");
        cout << "Loi doc file! Nhan phim bat ky de thoat...";
        _getch();
        return false;
    }
}

string ChooseFileMenu() {
    while (true) {
        vector<string> files = GetSaveFiles();

        if (files.empty()) {
            system("cls");
            DrawLoadgameBackground();
            GotoXY(40, 15);
            //SetColor(12, 15);
            printf("\x1b[38;2;255;50;50m\x1b[48;2;20;20;20m");
            cout << "Chua co du lieu Save! Nhan phim bat ky de quay lai...";
            _getch();
            return "";
        }

        int currentSelect = 0;
        int displayCount = files.size() < 15 ? files.size() : 15;
        bool isLooping = true;
        system("cls");
        DrawLoadgameBackground();
        
        GotoXY(40, 5);
        printf("\x1b[38;2;255;50;50m\x1b[48;2;30;30;30m"); 
        cout << "=== DANH SACH CAC VAN DA LUU ===";

        GotoXY(30, 8 + displayCount * 2 + 2);
        printf("\x1b[38;2;150;150;150m\x1b[48;2;20;20;20m"); 
        cout << "(W/S: Chon | Enter: Tai game | X: Xoa file | ESC: Huy)";

        while (isLooping) {
            // system("cls");
            // //system("color F0");
            // DrawLoadgameBackground();
            // GotoXY(40, 5);
            // //SetColor(12, 15);
            // printf("\x1b[38;2;255;50;50m\x1b[48;2;30;30;30m"); // Chữ đỏ, nền xám đen
            // cout << "=== DANH SACH CAC VAN DA LUU ===";

            for (int i = 0; i < displayCount; i++) {
                GotoXY(45, 8 + i * 2);
                if (i == currentSelect) {
                    //SetColor(0, 11);
                    printf("\x1b[38;2;0;255;255m\x1b[48;2;50;50;50m");
                    cout << ">> " << files[i] << " <<";
                }
                else {
                    //SetColor(0, 15);
                    printf("\x1b[38;2;255;255;255m\x1b[48;2;20;20;20m");
                    cout << "   " << files[i] << "   ";
                }
            }

            // GotoXY(30, 8 + displayCount * 2 + 2);
            // //SetColor(8, 15);
            // printf("\x1b[38;2;150;150;150m\x1b[48;2;20;20;20m"); // Màu xám chú thích
            // cout << "(W/S: Chon | Enter: Tai game | X: Xoa file | ESC: Huy)";

            int key = toupper(_getch());
            if (key == 27) { // Bấm ESC
                return "";
            }
            else if (key == 'W' || key == 72) {
                currentSelect--;
                if (currentSelect < 0) currentSelect = displayCount - 1;
            }
            else if (key == 'S' || key == 80) {
                currentSelect++;
                if (currentSelect >= displayCount) currentSelect = 0;
            }
            else if (key == 13) { // Phím Enter
                return files[currentSelect];
            }
            else if (key == 'X') {
                string fileToDelete = files[currentSelect] + ".caro";
                DeleteFileA(fileToDelete.c_str());
                isLooping = false;
            }
        }
    }
}

void ClearAllData() {
    vector<string> files = GetSaveFiles();

    if (files.empty()) {
        system("cls");
        DrawLoadgameBackground();
        GotoXY(40, 15);
        //SetColor(12, 15);
        printf("\x1b[38;2;255;50;50m\x1b[48;2;20;20;20m");
        cout << "Khong co du lieu luu nao de xoa!";
        _getch();
        return;
    }

    system("cls");
    //system("color F0");
    DrawLoadgameBackground();
    GotoXY(35, 15);
    //SetColor(12, 15);
    printf("\x1b[38;2;255;0;0m\x1b[48;2;30;30;30m");
    cout << "Ban co chac muon xoa TOAN BO " << files.size() << " file luu? (Y/N): ";

    char confirm = toupper(_getch());
    if (confirm == 'Y') {
        for (string file : files) {
            string fullPath = file + ".caro";
            DeleteFileA(fullPath.c_str());
        }
        GotoXY(35, 17);
        SetColor(10, 15);
        cout << "Da xoa thanh cong toan bo du lieu! Nhan phim bat ky...";
    }
    else {
        GotoXY(35, 17);
        SetColor(8, 15);
        cout << "Da huy thao tac xoa. Nhan phim bat ky...";
    }
    _getch();
}
bool loadPresent() {
    system("cls");
    DrawIngameBackground();
    DrawBoard(BOARD_SIZE);
    DrawPlayerInfo();
    UpdateTurnInfo();
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (_A[i][j].c != 0) {
                DrawCell(_A[i][j].x, _A[i][j].y, 15);
            }
    DrawCell(_X, _Y, 11);
    return true;
}