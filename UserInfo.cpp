#include "UserInfo.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "Menu.h"
#include <iostream>
#include <algorithm>
#include <cstdio>

using namespace std;

static const string ACTION_LABELS[] = {
    "UNDO",
    "REDO",
    u8"T\u1EA0M D\u1EEANG",
    "MENU"
};

int GetActionCount() {
    return static_cast<int>(sizeof(ACTION_LABELS) / sizeof(ACTION_LABELS[0]));
}

static void ClearHudLine(int y) {
    GotoXY(0, y);
    printf("\x1b[48;2;15;15;20m");
    cout << string(CONSOLE_COLS, ' ');
    printf("\x1b[0m");
}

void DrawStatusInfo(bool paused) {
    if (paused) {
        PrintTextWithBg(STATUS_X, STATUS_Y, "TRANG THAI: TAM DUNG       ", 14);
    }
    else {
        PrintTextWithBg(STATUS_X, STATUS_Y, "TRANG THAI: DANG CHOI      ", 10);
    }
}

static string ClipText(const string& text, int maxLen) {
    if ((int)text.length() <= maxLen) return text;
    if (maxLen <= 3) return text.substr(0, max(0, maxLen));
    return text.substr(0, maxLen - 3) + "...";
}

static void PrintHudCentered(int y, const string& text, int color) {
    ClearHudLine(y);
    int x = (CONSOLE_COLS - TextDisplayWidth(text)) / 2;
    if (x < 0) x = 0;
    PrintTextWithBg(x, y, text, color);
}

static void DrawActionBox(int x, int y, const string& text, bool selected) {
    int bg = selected ? 14 : 1;
    int fg = selected ? 0 : 11;
    SetColor(fg, bg);
    GotoXY(x, y);
    cout << " " << text << " ";
    SetColor(0, 15);
}

void DrawActionBar(int selectedAction) {
    static int lastSelected = -1;

    int totalW = 0;
    for (int i = 0; i < GetActionCount(); i++) {
        totalW += TextDisplayWidth(ACTION_LABELS[i]) + 2;
        if (i + 1 < GetActionCount()) totalW += 2;
    }

    int x = CenterConsoleX(totalW, CONSOLE_COLS);
    int positions[8] = {};
    for (int i = 0; i < GetActionCount(); i++) {
        positions[i] = x;
        x += TextDisplayWidth(ACTION_LABELS[i]) + 4;
    }

    if (selectedAction < 0 || lastSelected < 0) {
        ClearHudLine(HUD_HELP_Y);
        for (int i = 0; i < GetActionCount(); i++) {
            DrawActionBox(positions[i], HUD_HELP_Y, ACTION_LABELS[i], i == selectedAction);
        }
    }
    else if (selectedAction != lastSelected) {
        DrawActionBox(positions[lastSelected], HUD_HELP_Y, ACTION_LABELS[lastSelected], false);
        DrawActionBox(positions[selectedAction], HUD_HELP_Y, ACTION_LABELS[selectedAction], true);
    }
    lastSelected = selectedAction;
}

void DrawPlayerInfo() {
    for (int y = INFO_Y; y <= RESULT_Y; y++) {
        ClearHudLine(y);
    }

    string p1 = ClipText(_PLAYER1_NAME, 14);
    string p2 = ClipText(_PLAYER2_NAME, 14);
    string playerLine = "[X] " + p1 + "   vs   [O] " + p2;
    string helpLine = "";

    PrintHudCentered(HUD_PLAYER_Y, playerLine, 15);
    DrawActionBar(-1);
    PrintTextWithBg(TIMER_X, TIMER_Y, "THOI GIAN: 30s      ", 11);
    DrawStatusInfo(false);
}

void UpdateTurnInfo() {
    lock_guard<mutex> lock(consoleMutex);
    
    string turnText = "LUOT DI: ";
    if (_TURN == true) {
        PrintTextWithBg(TURN_INFO_X, TURN_INFO_Y, turnText + "X    ", 12);
    }
    else {
        PrintTextWithBg(TURN_INFO_X, TURN_INFO_Y, turnText + "O    ", 10);
    }
    
    GotoXY(_X, _Y); // Trả con trỏ về bàn cờ
}
