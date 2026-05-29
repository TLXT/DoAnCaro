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
#include "Language.h"

using namespace std;

struct SaveSlotPreview {
    int slotNumber = 0;
    bool occupied = false;
    string fileName;
    string player1;
    string player2;
    int moves = 0;
};

static string ChooseSaveSlotMenu(bool forSaving);

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
    {
        string selectedFile = ChooseSaveSlotMenu(true);
        if (selectedFile.empty()) {
            SetColor(15, 0);
            GotoXY(_X, _Y);
            return "";
        }

        ofstream outFile(selectedFile + ".caro", ios::trunc);
        if (outFile.is_open()) {
            outFile << _TURN << " " << _X << " " << _Y << endl;
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    outFile << _A[i][j].c << " ";
                }
                outFile << endl;
            }
            outFile << currentStep << endl;
            for (int i = 0; i < currentStep; i++) {
                outFile << moveHistory[i].row << " " << moveHistory[i].col << " " << moveHistory[i].c << endl;
            }
            outFile << CharacterASelect << " " << CharacterBSelect << endl;
            outFile << (_BOT_MODE ? 1 : 0) << " " << _BOT_DIFFICULTY << endl;
            outFile << _PLAYER1_NAME << endl;
            outFile << _PLAYER2_NAME << endl;
            outFile.close();

            system("cls");
            DrawLoadgameBackground();
            GotoXY(CenterConsoleX(TextDisplayWidth(L(TextId::SaveSuccess)), CONSOLE_COLS), 18);
            SetColor(10, 15);
            cout << L(TextId::SaveSuccess);
            _getch();
            SetColor(15, 0);
            return selectedFile;
        }

        system("cls");
        DrawLoadgameBackground();
        GotoXY(CenterConsoleX(TextDisplayWidth(L(TextId::SaveCreateError)), CONSOLE_COLS), 18);
        SetColor(12, 15);
        cout << L(TextId::SaveCreateError);
        _getch();
        SetColor(15, 0);
        return "";
    }

#if 0
    string filename;

    system("cls");
    DrawLoadgameBackground();   // [Merged] Vẽ nền xịn từ file gốc

    while (true) {
        GotoXY(30, 27);
        cout << "                                                                    ";

        GotoXY(30, 27);
        // [Merged] Dùng ANSI code để màu chữ đỏ sắc nét đè trên nền game
        printf("\x1b[38;2;255;50;50m\x1b[48;2;20;20;20m");
        cout << L(TextId::SavePrompt);

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
            cout << L(TextId::SaveDuplicate);
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
        cout << L(TextId::SaveSuccess);
        _getch();
        return filename;
    }
    else {
        GotoXY(30, 28);
        SetColor(12, 15);
        cout << L(TextId::SaveCreateError);
    }

    _getch();

    GotoXY(30, 27); cout << "                                                                    ";
    GotoXY(30, 28); cout << "                                                                    ";
    SetColor(0, 15);
    GotoXY(_X, _Y);
    return "";
#endif
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

static string SaveSlotBaseName(int index) {
    return "slot" + to_string(index + 1);
}

static bool SaveFileExists(const string& baseName) {
    string filePath = baseName + ".caro";
    DWORD attr = GetFileAttributesA(filePath.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

static int FixedSlotIndex(const string& name) {
    if (name.length() <= 4) return 0;
    if (name[0] != 's' || name[1] != 'l' || name[2] != 'o' || name[3] != 't') return 0;

    int value = 0;
    for (int i = 4; i < static_cast<int>(name.length()); i++) {
        if (name[i] < '0' || name[i] > '9') return 0;
        value = value * 10 + (name[i] - '0');
    }
    return value > 0 ? value : 0;
}

static bool IsFixedSlotName(const string& name) {
    return FixedSlotIndex(name) > 0;
}

static int RoundSlotCapacity(int count) {
    if (count < 6) count = 6;
    return ((count + 5) / 6) * 6;
}

static string FitText(const string& text, int maxWidth) {
    if (TextDisplayWidth(text) <= maxWidth) return text;

    string result;
    int width = 0;
    for (unsigned char ch : text) {
        if ((ch & 0xC0) != 0x80) {
            if (width >= maxWidth - 3) break;
            width++;
        }
        result.push_back(static_cast<char>(ch));
    }
    return result + "...";
}

static void PrintSlotText(int x, int y, const string& text, int maxWidth, int color, int bg) {
    string cropped = FitText(text, maxWidth);
    GotoXY(x, y);
    SetColor(color, bg);
    cout << cropped;
    int remain = maxWidth - TextDisplayWidth(cropped);
    if (remain > 0) cout << string(remain, ' ');
    SetColor(15, 0);
}

static void FillSlotArea(int x, int y, int w, int h, int bg) {
    SetColor(0, bg);
    string row(w, ' ');
    for (int i = 0; i < h; i++) {
        GotoXY(x, y + i);
        cout << row;
    }
    SetColor(15, 0);
}

static void DrawSlotBorder(int x, int y, int w, int h, bool selected, int bg) {
    int borderColor = selected ? 11 : 8;
    SetColor(borderColor, bg);
    GotoXY(x, y);
    cout << "\xE2\x94\x8C";
    for (int i = 0; i < w - 2; i++) cout << "\xE2\x94\x80";
    cout << "\xE2\x94\x90";
    for (int row = 1; row < h - 1; row++) {
        GotoXY(x, y + row);
        cout << "\xE2\x94\x82";
        GotoXY(x + w - 1, y + row);
        cout << "\xE2\x94\x82";
    }
    GotoXY(x, y + h - 1);
    cout << "\xE2\x94\x94";
    for (int i = 0; i < w - 2; i++) cout << "\xE2\x94\x80";
    cout << "\xE2\x94\x98";
    SetColor(15, 0);
}

static void DrawIconPixel(int x, int y, int color) {
    GotoXY(x, y);
    SetColor(color, color);
    cout << "  ";
}

static void DrawSaplingIcon(int x, int y, int bg) {
    SetColor(0, bg);
    for (int row = 0; row < 5; row++) {
        GotoXY(x, y + row);
        cout << string(12, ' ');
    }

    DrawIconPixel(x + 4, y + 0, 10);
    DrawIconPixel(x + 2, y + 1, 10);
    DrawIconPixel(x + 6, y + 1, 10);
    DrawIconPixel(x + 4, y + 2, 2);
    DrawIconPixel(x + 4, y + 3, 2);
    DrawIconPixel(x + 2, y + 4, 6);
    DrawIconPixel(x + 4, y + 4, 6);
    DrawIconPixel(x + 6, y + 4, 6);
    SetColor(15, 0);
}

static void DrawFlowerIcon(int x, int y, int bg) {
    SetColor(0, bg);
    for (int row = 0; row < 5; row++) {
        GotoXY(x, y + row);
        cout << string(12, ' ');
    }

    DrawIconPixel(x + 4, y + 0, 12);
    DrawIconPixel(x + 2, y + 1, 13);
    DrawIconPixel(x + 4, y + 1, 14);
    DrawIconPixel(x + 6, y + 1, 13);
    DrawIconPixel(x + 4, y + 2, 2);
    DrawIconPixel(x + 2, y + 3, 10);
    DrawIconPixel(x + 4, y + 3, 2);
    DrawIconPixel(x + 6, y + 3, 10);
    DrawIconPixel(x + 2, y + 4, 6);
    DrawIconPixel(x + 4, y + 4, 6);
    DrawIconPixel(x + 6, y + 4, 6);
    SetColor(15, 0);
}

static SaveSlotPreview ReadSavePreview(const string& baseName, int slotNumber) {
    SaveSlotPreview preview;
    preview.slotNumber = slotNumber;
    preview.fileName = baseName;
    preview.occupied = SaveFileExists(baseName);
    preview.player1 = "X";
    preview.player2 = "O";

    if (!preview.occupied) return preview;

    ifstream inFile(baseName + ".caro");
    if (!inFile.is_open()) return preview;

    int turn = 0, x = 0, y = 0;
    inFile >> turn >> x >> y;

    int cell = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (!(inFile >> cell)) return preview;
        }
    }

    int moveCount = 0;
    if (inFile >> moveCount) {
        preview.moves = moveCount;
        for (int i = 0; i < moveCount; i++) {
            int row = 0, col = 0, mark = 0;
            if (!(inFile >> row >> col >> mark)) break;
        }
    }

    int charA = 0, charB = 0;
    if (inFile >> charA >> charB) {
        int botMode = 0, botDiff = 0;
        if (inFile >> botMode >> botDiff) {
            inFile.ignore(1000, '\n');
            string p1, p2;
            if (getline(inFile, p1) && !p1.empty()) preview.player1 = p1;
            if (getline(inFile, p2) && !p2.empty()) preview.player2 = p2;
        }
    }

    return preview;
}

static vector<SaveSlotPreview> BuildSaveSlots(bool forSaving) {
    vector<string> files = GetSaveFiles();
    vector<string> legacyFiles;
    int highestFixedSlot = 0;

    for (const string& file : files) {
        int fixedIndex = FixedSlotIndex(file);
        if (fixedIndex > 0) {
            highestFixedSlot = max(highestFixedSlot, fixedIndex);
        }
        else {
            legacyFiles.push_back(file);
        }
    }

    int capacity = RoundSlotCapacity(max(highestFixedSlot, static_cast<int>(legacyFiles.size())));
    vector<SaveSlotPreview> slots(capacity);
    for (int i = 0; i < capacity; i++) {
        slots[i] = ReadSavePreview(SaveSlotBaseName(i), i + 1);
    }

    int fillIndex = 0;
    for (const string& file : legacyFiles) {
        while (fillIndex < static_cast<int>(slots.size()) && slots[fillIndex].occupied) {
            fillIndex++;
        }
        if (fillIndex >= static_cast<int>(slots.size())) {
            int oldSize = static_cast<int>(slots.size());
            slots.resize(oldSize + 6);
            for (int i = oldSize; i < static_cast<int>(slots.size()); i++) {
                slots[i] = ReadSavePreview(SaveSlotBaseName(i), i + 1);
            }
        }

        slots[fillIndex] = ReadSavePreview(file, fillIndex + 1);
        fillIndex++;
    }

    if (forSaving) {
        bool hasEmptySlot = false;
        for (const SaveSlotPreview& slot : slots) {
            if (!slot.occupied) {
                hasEmptySlot = true;
                break;
            }
        }

        if (!hasEmptySlot) {
            int oldSize = static_cast<int>(slots.size());
            slots.resize(oldSize + 6);
            for (int i = oldSize; i < static_cast<int>(slots.size()); i++) {
                slots[i] = ReadSavePreview(SaveSlotBaseName(i), i + 1);
            }
        }
    }

    return slots;
}

static void DrawSaveSlotCard(const SaveSlotPreview& slot, int x, int y, int w, int h, bool selected, bool forSaving) {
    int bg = selected ? 14 : 15;
    FillSlotArea(x, y, w, h, bg);
    DrawSlotBorder(x, y, w, h, selected, bg);

    string title = "SLOT " + to_string(slot.slotNumber);
    if (selected) title = "> " + title + " <";

    PrintSlotText(x + 2, y + 1, title, 22, selected ? 12 : 1, bg);
    if (slot.occupied) {
        PrintSlotText(x + 2, y + 2, FitText(slot.player1, 8) + " VS " + FitText(slot.player2, 8), 24, 9, bg);
        PrintSlotText(x + 2, y + 3, "MOVES: " + to_string(slot.moves), 24, 9, bg);
        PrintSlotText(x + 2, y + 4, "FILE: " + slot.fileName, 24, 8, bg);
        DrawFlowerIcon(x + w - 15, y + 1, bg);
    }
    else {
        string emptyText = (GetLanguage() == GameLanguage::Vietnamese) ? u8"<TR\u1ED0NG>" : "<EMPTY>";
        string actionText = forSaving
            ? ((GetLanguage() == GameLanguage::Vietnamese) ? u8"ENTER \u0110\u1EC2 L\u01AFU" : "ENTER TO SAVE")
            : ((GetLanguage() == GameLanguage::Vietnamese) ? u8"CH\u01AFA C\u00D3 D\u1EEE LI\u1EC6U" : "NO DATA");
        PrintSlotText(x + 2, y + 2, emptyText, 24, 8, bg);
        PrintSlotText(x + 2, y + 3, actionText, 24, 8, bg);
        PrintSlotText(x + 2, y + 4, "FILE: " + SaveSlotBaseName(slot.slotNumber - 1), 24, 8, bg);
        DrawSaplingIcon(x + w - 15, y + 1, bg);
    }
}

static void DrawPageButton(int x, int y, int w, const string& text, bool selected) {
    FillSlotArea(x, y, w, 3, selected ? 14 : 0);
    DrawSlotBorder(x, y, w, 3, selected, selected ? 14 : 0);
    PrintSlotText(x + (w - TextDisplayWidth(text)) / 2, y + 1, text, TextDisplayWidth(text), selected ? 12 : 11, selected ? 14 : 0);
}

static void DrawSaveSlotScreen(const vector<SaveSlotPreview>& slots, int page, int currentSelect, bool forSaving) {
    system("cls");
    DrawLoadgameBackground();

    string title = forSaving
        ? ((GetLanguage() == GameLanguage::Vietnamese) ? u8"L\u01AFU GAME" : "SAVE GAME")
        : L(TextId::LoadTitle);
    DrawMenuTitle(title, 1, CONSOLE_COLS);

    const int cols = 2;
    const int slotW = 44;
    const int slotH = 6;
    const int gapX = 4;
    const int gapY = 1;
    const int totalW = cols * slotW + gapX;
    const int startX = CenterConsoleX(totalW, CONSOLE_COLS);
    const int startY = 7;
    const int pageStart = page * 6;
    const int pageCount = static_cast<int>((slots.size() + 5) / 6);

    for (int i = 0; i < 6; i++) {
        int slotIndex = pageStart + i;
        if (slotIndex >= static_cast<int>(slots.size())) break;

        int col = i % cols;
        int row = i / cols;
        int x = startX + col * (slotW + gapX);
        int y = startY + row * (slotH + gapY);
        DrawSaveSlotCard(slots[slotIndex], x, y, slotW, slotH, i == currentSelect, forSaving);
    }

    string help = forSaving
        ? ((GetLanguage() == GameLanguage::Vietnamese) ? u8"WASD: CH\u1ECCN | ENTER: L\u01AFU/GHI \u0110\u00C8 | X: X\u00D3A | ESC: QUAY L\u1EA0I" : "WASD: SELECT | ENTER: SAVE/OVERWRITE | X: DELETE | ESC: BACK")
        : L(TextId::LoadHelp);
    int helpY = startY + 3 * (slotH + gapY);
    PrintSlotText(CenterConsoleX(TextDisplayWidth(help), CONSOLE_COLS), helpY, help, TextDisplayWidth(help), 11, 0);

    string pageText = ((GetLanguage() == GameLanguage::Vietnamese) ? u8"TRANG " : "PAGE ")
        + to_string(page + 1) + "/" + to_string(pageCount);
    PrintSlotText(CenterConsoleX(TextDisplayWidth(pageText), CONSOLE_COLS), helpY + 1, pageText, TextDisplayWidth(pageText), 14, 0);

    if (page > 0) {
        DrawPageButton(startX, helpY + 2, 22,
            (GetLanguage() == GameLanguage::Vietnamese) ? u8"<< TRANG TR\u01AF\u1EDAC" : "<< PREV PAGE",
            currentSelect == 6);
    }
    if (page < pageCount - 1) {
        DrawPageButton(startX + totalW - 22, helpY + 2, 22,
            (GetLanguage() == GameLanguage::Vietnamese) ? u8"TRANG SAU >>" : "NEXT PAGE >>",
            currentSelect == 7);
    }
}

static string ChooseSaveSlotMenu(bool forSaving) {
    vector<SaveSlotPreview> slots = BuildSaveSlots(forSaving);
    int page = 0;
    int currentSelect = 0;
    int lastSelect = -1;
    int lastPage = -1;

    while (true) {
        int pageCount = static_cast<int>((slots.size() + 5) / 6);
        if (page >= pageCount) page = pageCount - 1;
        if (page < 0) page = 0;
        if (currentSelect == 6 && page == 0) currentSelect = 4;
        if (currentSelect == 7 && page >= pageCount - 1) currentSelect = 5;

        if (currentSelect != lastSelect || page != lastPage) {
            DrawSaveSlotScreen(slots, page, currentSelect, forSaving);
            lastSelect = currentSelect;
            lastPage = page;
        }

        int key = ReadMenuKey();
        if (key == 27) {
            PlayMenuSound();
            return "";
        }
        else if (key == 'W' || key == 72) {
            if (currentSelect >= 6) {
                currentSelect = (currentSelect == 6) ? 4 : 5;
            }
            else {
                currentSelect = (currentSelect >= 2) ? currentSelect - 2 : currentSelect + 4;
            }
            PlayMenuSound();
        }
        else if (key == 'S' || key == 80) {
            if (currentSelect < 4) {
                currentSelect += 2;
            }
            else if (currentSelect < 6) {
                if (currentSelect % 2 == 0 && page > 0) currentSelect = 6;
                else if (currentSelect % 2 == 1 && page < pageCount - 1) currentSelect = 7;
                else if (page > 0) currentSelect = 6;
                else if (page < pageCount - 1) currentSelect = 7;
                else currentSelect -= 4;
            }
            else {
                currentSelect = (currentSelect == 6) ? 0 : 1;
            }
            PlayMenuSound();
        }
        else if (key == 'A' || key == 75 || key == 'D' || key == 77) {
            if (currentSelect < 6) {
                currentSelect = (currentSelect % 2 == 0) ? currentSelect + 1 : currentSelect - 1;
            }
            else {
                currentSelect = (currentSelect == 6) ? 7 : 6;
            }
            PlayMenuSound();
        }
        else if (key == 'X') {
            if (currentSelect < 6) {
                int slotIndex = page * 6 + currentSelect;
                if (slotIndex < static_cast<int>(slots.size()) && slots[slotIndex].occupied) {
                    DeleteFileA((slots[slotIndex].fileName + ".caro").c_str());
                    slots = BuildSaveSlots(forSaving);
                    int newPageCount = static_cast<int>((slots.size() + 5) / 6);
                    if (page >= newPageCount) page = newPageCount - 1;
                    lastPage = -1;
                    lastSelect = -1;
                }
            }
            PlayMenuSound();
        }
        else if (key == 13) {
            if (currentSelect == 6 && page > 0) {
                page--;
                currentSelect = 0;
                lastPage = -1;
                PlayMenuSound();
            }
            else if (currentSelect == 7 && page < pageCount - 1) {
                page++;
                currentSelect = 0;
                lastPage = -1;
                PlayMenuSound();
            }
            else if (currentSelect < 6) {
                int slotIndex = page * 6 + currentSelect;
                if (slotIndex < static_cast<int>(slots.size()) && (forSaving || slots[slotIndex].occupied)) {
                    PlayMenuSound();
                    return slots[slotIndex].fileName.empty()
                        ? SaveSlotBaseName(slotIndex)
                        : slots[slotIndex].fileName;
                }
                lastSelect = -1;
                PlayMenuSound();
            }
        }
    }
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
        cout << L(TextId::LoadReadError);
        _getch();
        return false;
    }
}

// [Merged] UI Menu Nút Cuộn Kết Hợp Background
string ChooseFileMenu() {
    return ChooseSaveSlotMenu(false);

    while (true) {
        vector<string> files = GetSaveFiles();
        if (files.empty()) {
            system("cls");
            DrawLoadgameBackground(); // [Merged] Phủ nền khi báo lỗi trống file

            // Dùng ANSI in đè nền xám viền đỏ để báo lỗi đẹp hơn
            GotoXY(40, 15);
            printf("\x1b[38;2;255;50;50m\x1b[48;2;20;20;20m");
            cout << L(TextId::LoadNoData);
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
        string title = L(TextId::LoadTitle);
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
                GotoXY(CenterConsoleX(TextDisplayWidth(title), consoleW), 5);
                printf("\x1b[38;2;255;50;50m\x1b[48;2;30;30;30m");
                cout << title;

                string help = L(TextId::LoadHelp);
                GotoXY(CenterConsoleX(TextDisplayWidth(help), consoleW), startY_Base + maxDisplay * (BTN_NORMAL_H + 1) + 2);
                printf("\x1b[38;2;150;150;150m\x1b[48;2;20;20;20m");
                cout << help;

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
        cout << L(TextId::ClearNoData);
        _getch();
        return;
    }

    // [Merged] Gọi Graphic Yes No từ bản 2
    string prompt = L(TextId::ClearConfirmPrefix) + to_string(files.size()) + L(TextId::ClearConfirmSuffix);
    bool confirm = GraphicalYesNo(prompt, 10, true, BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);

    system("cls");
    DrawLoadgameBackground();

    if (confirm) {
        for (string file : files) {
            string fullPath = file + ".caro";
            DeleteFileA(fullPath.c_str());
        }
        GotoXY(35, 17);
        SetColor(10, 15); cout << L(TextId::ClearDone);
    }
    else {
        GotoXY(35, 17);
        SetColor(8, 15); cout << L(TextId::ClearCanceled);
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
