#include "UserInfo.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "Menu.h"
#include <iostream>
#include <algorithm>
#include <cstdio>

using namespace std;

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
    int x = (CONSOLE_COLS - static_cast<int>(text.length())) / 2;
    if (x < 0) x = 0;
    PrintTextWithBg(x, y, text, color);
}

void DrawPlayerInfo() {
    for (int y = INFO_Y; y <= RESULT_Y; y++) {
        ClearHudLine(y);
    }

    string p1 = ClipText(_PLAYER1_NAME, 14);
    string p2 = ClipText(_PLAYER2_NAME, 14);
    string playerLine = "[X] " + p1 + "   vs   [O] " + p2;
    string helpLine = "[WASD] Di chuyen   [Enter] Danh   [Z/Y] Undo/Redo   [P] Pause   [M] Menu";

    PrintHudCentered(HUD_PLAYER_Y, playerLine, 15);
    PrintHudCentered(HUD_HELP_Y, helpLine, 11);
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
