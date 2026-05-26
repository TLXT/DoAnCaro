#include "SaveLoad.h"
#include "GameStatus.h"
#include "ControlConsole.h"
#include "DrawBoard.h"
#include "GamePlay.h"
#include "UserInfo.h"
#include "Character.h"

// Tích hợp cả thư viện vẽ background (từ bản 1) và nút bấm (từ bản 2)
#include "DrawBackground.hpp"
#include "Menu.h"
#include "btn_normal.h"
#include "btn_hover.h"

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
        else if (isalnum((unsigned char)c) || c == '-' || c == '_') {
            res += c;
            cout << c;
        }
    }
    return res;
}

string SaveGame() {
    string filename;

    system("cls");
    DrawLoadgameBackground();   // [Merged] Vẽ nền xịn từ file gốc

    while (true) {
        GotoXY(30, 27);
        cout << "                                                                    ";

        GotoXY(30, 27);
        // [Merged] Dùng ANSI code để màu chữ đỏ sắc nét đè trên nền game
        printf("\x1b[38;2;255;50;50m\x1b[48;2;20;20;20m");
        cout << "Nhap ten file de luu: ";

        UnhideCursor();
        filename = TypeFileName();
        HideCursor();

        if (filename == "") {
            GotoXY(30, 27); cout << "                                                                    ";
            GotoXY(30, 28); cout << "                                                                    ";
            SetColor(0, 15);
            GotoXY(_X, _Y);
            return"";
        }

        ifstream checkFile(filename + ".caro");
        if (checkFile.is_open()) {
            checkFile.close();

            GotoXY(30, 28);
            SetColor(12, 15); // Màu đỏ
            cout << "Ten file da ton tai! Vui long nhap ten khac... (Nhan phim bat ky)";
            _getch();

            GotoXY(30, 28);
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
            outFile << moveHistory[i].row << " " << moveHistory[i].col << " " << moveHistory[i].c << endl;
        }
        // Save character selection and game mode
        outFile << CharacterASelect << " " << CharacterBSelect << endl;
        outFile << (_BOT_MODE ? 1 : 0) << " " << _BOT_DIFFICULTY << endl;
        outFile << _PLAYER1_NAME << endl;
        outFile << _PLAYER2_NAME << endl;
        outFile.close();

        GotoXY(30, 28);
        SetColor(10, 15);
        cout << "Luu thanh cong! Nhan phim bat ky de tiep tuc...";
        _getch();
        return filename;
    }
    else {
        GotoXY(30, 28);
        SetColor(12, 15);
        cout << "Loi tao file! Nhan phim bat ky...";
    }

    _getch();

    GotoXY(30, 27); cout << "                                                                    ";
    GotoXY(30, 28); cout << "                                                                    ";
    SetColor(0, 15);
    GotoXY(_X, _Y);
    return "";
}

// [Merged] Logic lấy file mới nhất bằng Struct từ bản thứ 2
vector<string> GetSaveFiles() {
    vector<SaveFileInfo> files;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA("*.caro", &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            string fileName = findFileData.cFileName;
            fileName = fileName.substr(0, fileName.find_last_of("."));

            // Lấy thời gian file được lưu cuối cùng
            ULARGE_INTEGER fileTime;
            fileTime.LowPart = findFileData.ftLastWriteTime.dwLowDateTime;
            fileTime.HighPart = findFileData.ftLastWriteTime.dwHighDateTime;

            files.push_back({ fileName, fileTime });
        } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }

    // Sắp xếp file theo thời gian lưu (Giảm dần: Mới nhất lên đầu)
    sort(files.begin(), files.end(), [](const SaveFileInfo& a, const SaveFileInfo& b) {
        return a.time.QuadPart > b.time.QuadPart;
        });

    vector<string> fileList;
    for (const auto& item : files) {
        fileList.push_back(item.name);
    }

    return fileList;
}

bool LoadGame() {
    string filename = ChooseFileMenu();

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
        moveHistory.clear();
        if (inFile >> currentStep) {
            for (int i = 0; i < currentStep; i++) {
                MoveNode node;
                inFile >> node.row >> node.col >> node.c;
                moveHistory.push_back(node);
            }
        }
        else {
            currentStep = 0;
        }
        // Load character selection and game mode (backward compatible)
        int charA = 0, charB = 4;
        if (inFile >> charA >> charB) {
            CharacterASelect = (charA >= 0 && charA <= 4) ? charA : 0;
            CharacterBSelect = (charB >= 0 && charB <= 4) ? charB : 4;
            int botMode = 0, botDiff = 2;
            if (inFile >> botMode >> botDiff) {
                _BOT_MODE = (botMode != 0);
                _BOT_DIFFICULTY = botDiff;
            }
            inFile.ignore(1000, '\n'); // skip rest of line
            string p1, p2;
            if (getline(inFile, p1) && !p1.empty()) _PLAYER1_NAME = p1;
            if (getline(inFile, p2) && !p2.empty()) _PLAYER2_NAME = p2;
        } else {
            CharacterASelect = 0;
            CharacterBSelect = 4;
        }
        inFile.close();

        system("cls");
        DrawIngameBackground(); // [Merged] Vẽ background ingame từ bản gốc
        DrawBoard(BOARD_SIZE);
        DrawPlayerInfo();
        UpdateTurnInfo();

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                // [Merged] Fix lỗi hiển thị: Chỉ vẽ các ô đã có quân
                if (_A[i][j].c != 0) {
                    DrawCell(_A[i][j].x, _A[i][j].y, BOARD_BG_COLOR);
                }
            }
        }
        DrawCell(_X, _Y, BOARD_CURSOR_COLOR);
        ingamedisplay(CharacterASelect, true);
        ingamedisplay(CharacterBSelect, false);

        return true;
    }
    else {
        system("cls");
        DrawLoadgameBackground();
        GotoXY(40, 15);
        SetColor(12, 15);
        cout << "Loi doc file! Nhan phim bat ky de thoat...";
        _getch();
        return false;
    }
}

// [Merged] UI Menu Nút Cuộn Kết Hợp Background
string ChooseFileMenu() {
    while (true) {
        vector<string> files = GetSaveFiles();
        if (files.empty()) {
            system("cls");
            DrawLoadgameBackground(); // [Merged] Phủ nền khi báo lỗi trống file

            // Dùng ANSI in đè nền xám viền đỏ để báo lỗi đẹp hơn
            GotoXY(40, 15);
            printf("\x1b[38;2;255;50;50m\x1b[48;2;20;20;20m");
            cout << "Chua co du lieu Save! Nhan phim bat ky de quay lai...";
            _getch();
            return "";
        }

        int currentSelect = 0;
        int maxDisplay = 5; // Chỉ hiển thị tối đa 5 file cùng lúc để không vỡ khung
        int startIndex = 0; // Vị trí file bắt đầu hiển thị (dùng để cuộn)

        int lastSelect = -1;
        int lastStartIndex = -1;
        bool isLooping = true;

        int consoleW = CONSOLE_COLS;
        string title = "DANH SACH CAC VAN DA LUU";
        int frameW = static_cast<int>(title.length()) + 8;
        int btnCols = BTN_NORMAL_W * 2;
        int startX = CenterConsoleX(btnCols, consoleW);
        int startY_Base = 10;

        int bgNorm = BTN_NORMAL[BTN_NORMAL_H / 2][BTN_NORMAL_W / 2];
        int bgHov = BTN_HOVER[BTN_HOVER_H / 2][BTN_HOVER_W / 2];

        while (isLooping) {
            // NẾU BỊ CUỘN TRANG (thay đổi startIndex): Xóa và vẽ lại toàn bộ khung và nền
            if (startIndex != lastStartIndex) {
                system("cls");
                DrawLoadgameBackground(); // Vẽ lại nền sau khi clean

                // Vẽ Title bằng text ANSI để tiệp với không gian nền
                GotoXY(CenterConsoleX(35, consoleW), 5);
                printf("\x1b[38;2;255;50;50m\x1b[48;2;30;30;30m");
                cout << "=== DANH SACH CAC VAN DA LUU ===";

                GotoXY(CenterConsoleX(58, consoleW), startY_Base + maxDisplay * (BTN_NORMAL_H + 1) + 2);
                printf("\x1b[38;2;150;150;150m\x1b[48;2;20;20;20m");
                cout << "(W/S: Chon | Enter: Tai game | X: Xoa file | ESC: Huy)";

                lastStartIndex = startIndex;
                lastSelect = -1; // Ép vẽ lại toàn bộ nút
            }

            // NẾU CÓ SỰ DỊCH CHUYỂN NÚT BẤM (Cập nhật hiển thị Hover)
            if (currentSelect != lastSelect) {
                int endIdx = min(startIndex + maxDisplay, (int)files.size());

                for (int i = startIndex; i < endIdx; i++) {
                    int rowOffset = i - startIndex; // Tính toán xem nút sẽ nằm ở dòng thứ mấy trên màn hình
                    int startY = startY_Base + rowOffset * (BTN_NORMAL_H + 1);

                    if (i == currentSelect) {
                        DrawSolidImage(BTN_HOVER, BTN_HOVER_W, BTN_HOVER_H, startX, startY);
                        GotoXY(startX + (btnCols - static_cast<int>(files[i].length())) / 2, startY + BTN_HOVER_H / 2);
                        SetColor(0, bgHov); cout << files[i];
                    }
                    else {
                        DrawSolidImage(BTN_NORMAL, BTN_NORMAL_W, BTN_NORMAL_H, startX, startY);
                        GotoXY(startX + (btnCols - static_cast<int>(files[i].length())) / 2, startY + BTN_NORMAL_H / 2);
                        SetColor(0, bgNorm); cout << files[i];
                    }
                }
                lastSelect = currentSelect;
            }

            // Xử lý phím
            SetColor(0, 15);
            int key = ReadMenuKey();

            if (key == 27) return ""; // ESC
            else if (key == 'W' || key == 72) {
                currentSelect--;
                if (currentSelect < 0) currentSelect = static_cast<int>(files.size()) - 1;
                PlayMenuSound();
            }
            else if (key == 'S' || key == 80) {
                currentSelect++;
                if (currentSelect >= static_cast<int>(files.size())) currentSelect = 0;
                PlayMenuSound();
            }
            else if (key == 13) {
                PlayMenuSound();
                return files[currentSelect];
            }
            else if (key == 'X') {
                string fileToDelete = files[currentSelect] + ".caro";
                DeleteFileA(fileToDelete.c_str());
                PlayMenuSound();
                isLooping = false; // Bấm xóa sẽ kích hoạt reset lại file
            }

            // LOGIC CUỘN TRANG
            // Nếu con trỏ chuột nhảy lên trên khỏi giới hạn hiển thị
            if (currentSelect < startIndex) {
                startIndex = currentSelect;
            }
            // Nếu con trỏ chuột nhảy xuống dưới khỏi giới hạn hiển thị
            else if (currentSelect >= startIndex + maxDisplay) {
                startIndex = currentSelect - maxDisplay + 1;
            }
        }
    }
}

void ClearAllData() {
    vector<string> files = GetSaveFiles();
    if (files.empty()) {
        system("cls");
        DrawLoadgameBackground(); // [Merged] Draw bg 
        GotoXY(40, 15);
        printf("\x1b[38;2;255;50;50m\x1b[48;2;20;20;20m");
        cout << "Khong co du lieu luu nao de xoa!";
        _getch();
        return;
    }

    // [Merged] Gọi Graphic Yes No từ bản 2
    string prompt = "Ban co chac muon xoa TOAN BO " + to_string(files.size()) + " file luu?";
    bool confirm = GraphicalYesNo(prompt, 10, true, BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);

    system("cls");
    DrawLoadgameBackground();

    if (confirm) {
        for (string file : files) {
            string fullPath = file + ".caro";
            DeleteFileA(fullPath.c_str());
        }
        GotoXY(35, 17);
        SetColor(10, 15); cout << "Da xoa thanh cong toan bo du lieu! Nhan phim bat ky...";
    }
    else {
        GotoXY(35, 17);
        SetColor(8, 15); cout << "Da huy thao tac xoa. Nhan phim bat ky...";
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
            if (_A[i][j].c != 0) { // [Merged] Chỉ vẽ lên ô có quân
                DrawCell(_A[i][j].x, _A[i][j].y, BOARD_BG_COLOR);
            }
    DrawCell(_X, _Y, BOARD_CURSOR_COLOR);
    ingamedisplay(CharacterASelect, true);
    ingamedisplay(CharacterBSelect, false);
    return true;
}
