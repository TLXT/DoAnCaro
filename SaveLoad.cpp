#include "SaveLoad.h"
#include "GameStatus.h"
#include "ControlConsole.h"
#include "DrawBoard.h"
#include "GamePlay.h"
#include "UserInfo.h"
#include "Character.h"


#include "DrawBackground.hpp"
#include "Menu.h"
#include "btn_normal.h"
#include "btn_hover.h"
#include "Language.h"
#include <ctime>

using namespace std;

struct SaveSlotPreview {
    int slotNumber = 0;
    bool occupied = false;
    string fileName;
    string player1;
    string player2;
    string saveTitle;
    string savedAt;
    int moves = 0;
};

static string ChooseSaveSlotMenu(bool forSaving);

static void FillNoticeRect(int x, int y, int w, int h, int r, int g, int b) {
    printf("\x1b[48;2;%d;%d;%dm", r, g, b);
    string row(w, ' ');
    for (int i = 0; i < h; i++) {
        GotoXY(x, y + i);
        cout << row;
    }
    printf("\x1b[0m");
}

static void DrawNoticeFrame(int x, int y, int w, int h, int fr, int fg, int fb, int br, int bg, int bb) {
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", fr, fg, fb, br, bg, bb);
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
    printf("\x1b[0m");
}

static void PrintNoticeText(int x, int y, const string& text, int fr, int fg, int fb, int br, int bg, int bb) {
    GotoXY(x, y);
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", fr, fg, fb, br, bg, bb);
    cout << text;
    printf("\x1b[0m");
}

static void PrintNoticeCentered(int x, int y, int w, const string& text, int fr, int fg, int fb, int br, int bg, int bb) {
    int textX = x + (w - TextDisplayWidth(text)) / 2;
    if (textX < x) textX = x;
    PrintNoticeText(textX, y, text, fr, fg, fb, br, bg, bb);
}

static void DrawNoticeIcon(int centerX, int y, char type, int r, int g, int b, int br, int bg, int bb) {
    static const char* CHECK[5] = {
        "00001",
        "00010",
        "10100",
        "01000",
        "00000"
    };
    static const char* WARN[5] = {
        "00100",
        "01110",
        "01110",
        "00000",
        "00100"
    };
    static const char* CROSS[5] = {
        "10001",
        "01010",
        "00100",
        "01010",
        "10001"
    };

    const char** glyph = (type == 'x') ? CROSS : ((type == '!') ? WARN : CHECK);
    int startX = centerX - 5;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            if (glyph[row][col] != '1') continue;
            GotoXY(startX + col * 2, y + row);
            printf("\x1b[48;2;%d;%d;%dm  \x1b[0m", r, g, b);
        }
    }
}

static void ShowSaveNotice(const string& title, const string& message, char iconType, int accentR, int accentG, int accentB) {
    ClearScreenFast();
    DrawLoadgameBackground();

    const int panelR = 12;
    const int panelG = 16;
    const int panelB = 24;
    int panelW = max(64, min(CONSOLE_COLS - 16, max(TextDisplayWidth(title), TextDisplayWidth(message)) + 22));
    int panelH = 15;
    int panelX = CenterConsoleX(panelW, CONSOLE_COLS);
    int panelY = 10;

    FillNoticeRect(panelX, panelY, panelW, panelH, panelR, panelG, panelB);
    DrawNoticeFrame(panelX, panelY, panelW, panelH, accentR, accentG, accentB, panelR, panelG, panelB);
    DrawNoticeIcon(panelX + panelW / 2, panelY + 2, iconType, accentR, accentG, accentB, panelR, panelG, panelB);
    PrintNoticeCentered(panelX, panelY + 8, panelW, title, 255, 245, 80, panelR, panelG, panelB);
    PrintNoticeCentered(panelX, panelY + 10, panelW, message, 225, 245, 255, panelR, panelG, panelB);
    PrintNoticeCentered(panelX, panelY + 12, panelW, L(NoticeContinue), 0, 255, 255, panelR, panelG, panelB);
    _getch();
    SetColor(15, 0);
}

static string FormatVietnamTime(time_t utcSeconds) {
    time_t vietnamSeconds = utcSeconds + 7 * 60 * 60;
    tm vietnamTime{};
    gmtime_s(&vietnamTime, &vietnamSeconds);

    char buffer[32];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", &vietnamTime);
    return string(buffer) + " VN";
}

static string CurrentVietnamTimeString() {
    return FormatVietnamTime(time(nullptr));
}

static string FileVietnamModifiedTime(const string& filePath) {
    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExA(filePath.c_str(), GetFileExInfoStandard, &data)) {
        return "";
    }

    ULARGE_INTEGER value{};
    value.LowPart = data.ftLastWriteTime.dwLowDateTime;
    value.HighPart = data.ftLastWriteTime.dwHighDateTime;

    const unsigned long long windowsToUnix = 116444736000000000ULL;
    if (value.QuadPart < windowsToUnix) return "";

    time_t utcSeconds = static_cast<time_t>((value.QuadPart - windowsToUnix) / 10000000ULL);
    return FormatVietnamTime(utcSeconds);
}

static void DrawSaveNamePrompt(const string& slotName, const string& saveTitle, bool showError, bool fullRedraw) {
    const int panelW = 74;
    const int panelH = 11;
    const int panelX = CenterConsoleX(panelW, CONSOLE_COLS);
    const int panelY = 11;
    const int inputW = 32;
    const int inputX = panelX + 29;
    const int inputY = panelY + 4;

    if (fullRedraw) {
        ClearScreenFast();
        DrawLoadgameBackground();

        string title = (GetLanguage() == Vietnamese) ? u8"\u0110\u1EB6T T\u00CAN B\u1EA2N L\u01AFU" : "SAVE NAME";
        DrawMenuTitle(title, 2, CONSOLE_COLS);
        DrawFrame(panelX, panelY, panelW, panelH);

        string slotLabel = (GetLanguage() == Vietnamese) ? u8"\u00D4 L\u01AFU: " : "SLOT: ";
        PrintTextWithBg(panelX + 5, panelY + 2, slotLabel + slotName, 14);
        PrintTextWithBg(panelX + 5, inputY, (GetLanguage() == Vietnamese) ? u8"T\u00CAN B\u1EA2N L\u01AFU:" : "SAVE TITLE:", 11);

        string help = (GetLanguage() == Vietnamese)
            ? u8"ENTER: L\u01AFU | ESC: H\u1EE6Y"
            : "ENTER: SAVE | ESC: CANCEL";
        PrintTextWithBg(CenterConsoleX(TextDisplayWidth(help), CONSOLE_COLS), panelY + 7, help, 11);
    }

    GotoXY(inputX, inputY);
    SetColor(15, 1);
    string shown = saveTitle;
    if (TextDisplayWidth(shown) > inputW - 2) shown = shown.substr(0, inputW - 5) + "...";
    cout << " " << shown;
    int remain = inputW - 1 - TextDisplayWidth(shown);
    if (remain > 0) cout << string(remain, ' ');
    SetColor(15, 0);

    FillNoticeRect(panelX + 2, panelY + 9, panelW - 4, 1, 15, 15, 20);

    if (showError) {
        string error = (GetLanguage() == Vietnamese)
            ? u8"T\u00CAN B\u1EA2N L\u01AFU KH\u00D4NG \u0110\u01AF\u1EE2C TR\u1ED0NG"
            : "SAVE TITLE CANNOT BE EMPTY";
        PrintTextWithBg(CenterConsoleX(TextDisplayWidth(error), CONSOLE_COLS), panelY + 9, error, 12);
    }

    UnhideCursor();
    GotoXY(inputX + 1 + min(TextDisplayWidth(saveTitle), inputW - 2), inputY);
}

static string PromptSaveDisplayName(const string& slotName) {
    string saveTitle;
    bool showError = false;

    DrawSaveNamePrompt(slotName, saveTitle, showError, true);

    while (true) {
        int key = _getch();

        if (key == 27) {
            HideCursor();
            return "";
        }
        if (key == 13) {
            if (!saveTitle.empty()) {
                HideCursor();
                return saveTitle;
            }
            showError = true;
            DrawSaveNamePrompt(slotName, saveTitle, showError, false);
            continue;
        }
        if (key == 8) {
            if (!saveTitle.empty()) saveTitle.pop_back();
            showError = false;
            DrawSaveNamePrompt(slotName, saveTitle, showError, false);
            continue;
        }
        if (key >= 32 && key <= 126 && TextDisplayWidth(saveTitle) < 24) {
            saveTitle.push_back(static_cast<char>(key));
            showError = false;
            DrawSaveNamePrompt(slotName, saveTitle, showError, false);
        }
    }
}

string SaveGame() {
    {
        string selectedFile = ChooseSaveSlotMenu(true);
        if (selectedFile.empty()) {
            SetColor(15, 0);
            GotoXY(_X, _Y);
            return "";
        }

        string saveTitle = PromptSaveDisplayName(selectedFile);
        if (saveTitle.empty()) {
            SetColor(15, 0);
            GotoXY(_X, _Y);
            return "";
        }
        string savedAt = CurrentVietnamTimeString();

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
            outFile << "#META" << endl;
            outFile << saveTitle << endl;
            outFile << savedAt << endl;
            outFile.close();

            ShowSaveNotice(L(SaveSuccessTitle), L(SaveSuccess), 'v', 60, 230, 120);
            SetColor(15, 0);
            return selectedFile;
        }

        ShowSaveNotice(L(SaveCreateErrorTitle), L(SaveCreateError), '!', 255, 80, 80);
        SetColor(15, 0);
        return "";
    }
}


vector<string> GetSaveFiles() {
    vector<SaveFileInfo> files;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA("*.caro", &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            string fileName = findFileData.cFileName;
            fileName = fileName.substr(0, fileName.find_last_of("."));


            ULARGE_INTEGER fileTime;
            fileTime.LowPart = findFileData.ftLastWriteTime.dwLowDateTime;
            fileTime.HighPart = findFileData.ftLastWriteTime.dwHighDateTime;

            files.push_back({ fileName, fileTime });
        } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }


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
    preview.saveTitle = baseName;
    preview.savedAt = preview.occupied ? FileVietnamModifiedTime(baseName + ".caro") : "";

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

            string marker;
            if (getline(inFile, marker) && marker == "#META") {
                string title, savedAt;
                if (getline(inFile, title) && !title.empty()) preview.saveTitle = title;
                if (getline(inFile, savedAt) && !savedAt.empty()) preview.savedAt = savedAt;
            }
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

    string slotWord = (GetLanguage() == Vietnamese) ? u8"\u00D4 L\u01AFU " : "SLOT ";
    string title = slotWord + to_string(slot.slotNumber);
    if (selected) title = "> " + title + " <";

    PrintSlotText(x + 2, y + 1, title, 22, selected ? 12 : 1, bg);
    if (slot.occupied) {
        PrintSlotText(x + 2, y + 2, FitText(slot.saveTitle, 24), 24, 9, bg);
        string versus = (GetLanguage() == Vietnamese) ? u8" \u0110\u1EA4U " : " VS ";
        string moveLabel = (GetLanguage() == Vietnamese) ? u8"N\u01AF\u1EDAC: " : "MOVES: ";
        PrintSlotText(x + 2, y + 3, FitText(slot.player1, 8) + versus + FitText(slot.player2, 8), 24, 9, bg);
        PrintSlotText(x + 2, y + 4, slot.savedAt.empty() ? (moveLabel + to_string(slot.moves)) : slot.savedAt, 24, 8, bg);
        DrawFlowerIcon(x + w - 15, y + 1, bg);
    }
    else {
        string emptyText = (GetLanguage() == Vietnamese) ? u8"<TR\u1ED0NG>" : "<EMPTY>";
        string actionText = forSaving
            ? ((GetLanguage() == Vietnamese) ? u8"ENTER \u0110\u1EC2 L\u01AFU" : "ENTER TO SAVE")
            : ((GetLanguage() == Vietnamese) ? u8"CH\u01AFA C\u00D3 D\u1EEE LI\u1EC6U" : "NO DATA");
        PrintSlotText(x + 2, y + 2, emptyText, 24, 8, bg);
        PrintSlotText(x + 2, y + 3, actionText, 24, 8, bg);
        string fileLabel = (GetLanguage() == Vietnamese) ? u8"T\u1EC6P: " : "FILE: ";
        PrintSlotText(x + 2, y + 4, fileLabel + SaveSlotBaseName(slot.slotNumber - 1), 24, 8, bg);
        DrawSaplingIcon(x + w - 15, y + 1, bg);
    }
}

static void DrawPageButton(int x, int y, int w, const string& text, bool selected) {
    FillSlotArea(x, y, w, 3, selected ? 14 : 0);
    DrawSlotBorder(x, y, w, 3, selected, selected ? 14 : 0);
    PrintSlotText(x + (w - TextDisplayWidth(text)) / 2, y + 1, text, TextDisplayWidth(text), selected ? 12 : 11, selected ? 14 : 0);
}

static void DrawSaveSlotScreen(const vector<SaveSlotPreview>& slots, int page, int currentSelect, bool forSaving) {
    ClearScreenFast();
    DrawLoadgameBackground();

    string title = forSaving
        ? ((GetLanguage() == Vietnamese) ? u8"L\u01AFU V\u00C1N" : "SAVE GAME")
        : L(LoadTitle);
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
        ? ((GetLanguage() == Vietnamese) ? u8"WASD: CH\u1ECCN | ENTER: L\u01AFU/GHI \u0110\u00C8 | X: X\u00D3A T\u1EC6P | ESC: QUAY L\u1EA0I" : "WASD: SELECT | ENTER: SAVE/OVERWRITE | X: DELETE FILE | ESC: BACK")
        : L(LoadHelp);
    int helpY = startY + 3 * (slotH + gapY);
    PrintSlotText(CenterConsoleX(TextDisplayWidth(help), CONSOLE_COLS), helpY, help, TextDisplayWidth(help), 11, 0);

    string pageText = ((GetLanguage() == Vietnamese) ? u8"TRANG " : "PAGE ")
        + to_string(page + 1) + "/" + to_string(pageCount);
    PrintSlotText(CenterConsoleX(TextDisplayWidth(pageText), CONSOLE_COLS), helpY + 1, pageText, TextDisplayWidth(pageText), 14, 0);

    if (page > 0) {
        DrawPageButton(startX, helpY + 2, 22,
            (GetLanguage() == Vietnamese) ? u8"<< TRANG TR\u01AF\u1EDAC" : "<< PREV PAGE",
            currentSelect == 6);
    }
    if (page < pageCount - 1) {
        DrawPageButton(startX + totalW - 22, helpY + 2, 22,
            (GetLanguage() == Vietnamese) ? u8"TRANG SAU >>" : "NEXT PAGE >>",
            currentSelect == 7);
    }
}

static void DrawSaveSlotSelectionItem(const vector<SaveSlotPreview>& slots, int page, int select, bool selected, bool forSaving) {
    const int cols = 2;
    const int slotW = 44;
    const int slotH = 6;
    const int gapX = 4;
    const int gapY = 1;
    const int totalW = cols * slotW + gapX;
    const int startX = CenterConsoleX(totalW, CONSOLE_COLS);
    const int startY = 7;
    const int pageCount = static_cast<int>((slots.size() + 5) / 6);
    const int helpY = startY + 3 * (slotH + gapY);

    if (select >= 0 && select < 6) {
        int slotIndex = page * 6 + select;
        if (slotIndex >= static_cast<int>(slots.size())) return;

        int col = select % cols;
        int row = select / cols;
        int x = startX + col * (slotW + gapX);
        int y = startY + row * (slotH + gapY);
        DrawSaveSlotCard(slots[slotIndex], x, y, slotW, slotH, selected, forSaving);
    }
    else if (select == 6 && page > 0) {
        DrawPageButton(startX, helpY + 2, 22,
            (GetLanguage() == Vietnamese) ? u8"<< TRANG TR\u01AF\u1EDAC" : "<< PREV PAGE",
            selected);
    }
    else if (select == 7 && page < pageCount - 1) {
        DrawPageButton(startX + totalW - 22, helpY + 2, 22,
            (GetLanguage() == Vietnamese) ? u8"TRANG SAU >>" : "NEXT PAGE >>",
            selected);
    }
}

static string ChooseSaveSlotMenu(bool forSaving) {
    // Man hinh chon slot dung chung cho luu va tai van.
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

        if (page != lastPage) {
            DrawSaveSlotScreen(slots, page, currentSelect, forSaving);
            lastSelect = currentSelect;
            lastPage = page;
        }
        else if (currentSelect != lastSelect) {
            if (lastSelect >= 0) {
                DrawSaveSlotSelectionItem(slots, page, lastSelect, false, forSaving);
            }
            DrawSaveSlotSelectionItem(slots, page, currentSelect, true, forSaving);
            lastSelect = currentSelect;
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

        int charA = 0, charB = 4;
        if (inFile >> charA >> charB) {
            CharacterASelect = (charA >= 0 && charA <= 4) ? charA : 0;
            CharacterBSelect = (charB >= 0 && charB <= 4) ? charB : 4;
            int botMode = 0, botDiff = 2;
            if (inFile >> botMode >> botDiff) {
                _BOT_MODE = (botMode != 0);
                _BOT_DIFFICULTY = botDiff;
            }
            inFile.ignore(1000, '\n');
            string p1, p2;
            if (getline(inFile, p1) && !p1.empty()) _PLAYER1_NAME = p1;
            if (getline(inFile, p2) && !p2.empty()) _PLAYER2_NAME = p2;
        } else {
            CharacterASelect = 0;
            CharacterBSelect = 4;
        }
        inFile.close();

        ClearScreenFast();
        DrawIngameBackground();
        DrawBoard(BOARD_SIZE);
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {

                if (_A[i][j].c != 0) {
                    DrawCell(_A[i][j].x, _A[i][j].y, BOARD_BG_COLOR);
                }
            }
        }
        DrawCell(_X, _Y, BOARD_CURSOR_COLOR);
        ingamedisplay(CharacterASelect, true);
        ingamedisplay(CharacterBSelect, false);
        DrawPlayerInfo();
        UpdateTurnInfo();

        return true;
    }
    else {
        ShowSaveNotice(L(LoadReadErrorTitle), L(LoadReadError), '!', 255, 80, 80);
        return false;
    }
}


string ChooseFileMenu() {
    return ChooseSaveSlotMenu(false);
}

void ClearAllData() {
    vector<string> files = GetSaveFiles();
    if (files.empty()) {
        ShowSaveNotice(L(ClearNoDataTitle), L(ClearNoData), '!', 255, 220, 80);
        return;
    }


    string prompt = L(ClearConfirmPrefix) + to_string(files.size()) + L(ClearConfirmSuffix);
    bool confirm = GraphicalYesNo(prompt, 10, true, BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);

    if (confirm) {
        for (string file : files) {
            string fullPath = file + ".caro";
            DeleteFileA(fullPath.c_str());
        }
        ShowSaveNotice(L(ClearDoneTitle), L(ClearDone), 'v', 60, 230, 120);
    }
    else {
        ShowSaveNotice(L(ClearCanceledTitle), L(ClearCanceled), 'x', 180, 190, 210);
    }
}

bool loadPresent() {
    ClearScreenFast();
    DrawIngameBackground();
    DrawBoard(BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (_A[i][j].c != 0) {
                DrawCell(_A[i][j].x, _A[i][j].y, BOARD_BG_COLOR);
            }
    DrawCell(_X, _Y, BOARD_CURSOR_COLOR);
    ingamedisplay(CharacterASelect, true);
    ingamedisplay(CharacterBSelect, false);
    DrawPlayerInfo();
    UpdateTurnInfo();
    return true;
}
