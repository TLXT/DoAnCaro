#include "UserInfo.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "Menu.h"
#include "Language.h"
#include "Character.h"
#include <iostream>
#include <algorithm>
#include <cstdio>

using namespace std;

static const int HUD_GROUND_R = 92;
static const int HUD_GROUND_G = 58;
static const int HUD_GROUND_B = 32;

static void HudTextRgb(int color, int& r, int& g, int& b) {
    r = 235; g = 240; b = 245;
    if (color == 10) { r = 0; g = 255; b = 80; }
    else if (color == 11) { r = 0; g = 255; b = 255; }
    else if (color == 12) { r = 255; g = 70; b = 70; }
    else if (color == 14) { r = 255; g = 240; b = 60; }
}

void PrintHudTextWithBg(int x, int y, const string& text, int color) {
    int r, g, b;
    HudTextRgb(color, r, g, b);
    GotoXY(x, y);
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm",
        r, g, b, HUD_GROUND_R, HUD_GROUND_G, HUD_GROUND_B);
    cout << text;
    printf("\x1b[0m");
}

int GetActionCount() {
    return 4;
}

static string GetActionLabel(int index) {
    switch (index) {
    case 0: return "UNDO";
    case 1: return "REDO";
    case 2: return L(TextId::HudPause);
    default: return L(TextId::HudMenu);
    }
}

static void ClearHudLine(int y) {
    GotoXY(0, y);
    printf("\x1b[48;2;%d;%d;%dm", HUD_GROUND_R, HUD_GROUND_G, HUD_GROUND_B);
    cout << string(CONSOLE_COLS - 1, ' ');
    printf("\x1b[0m");
}

static void DrawHudFrame() {
    const int x = 14;
    const int y = INFO_Y - 1;
    const int w = CONSOLE_COLS - 28;
    const int h = 8;

    printf("\x1b[38;2;80;245;230m\x1b[48;2;%d;%d;%dm", HUD_GROUND_R, HUD_GROUND_G, HUD_GROUND_B);
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

void DrawStatusInfo(bool paused) {
    if (paused) {
        PrintHudTextWithBg(STATUS_X, STATUS_Y, L(TextId::HudStatusPaused) + "       ", 14);
    }
    else {
        PrintHudTextWithBg(STATUS_X, STATUS_Y, L(TextId::HudStatusPlaying) + "      ", 10);
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
    PrintHudTextWithBg(x, y, text, color);
}

static void DrawActionBox(int x, int y, const string& text, bool selected) {
    int bg = selected ? 14 : 1;
    int fg = selected ? 0 : 11;
    SetColor(fg, bg);
    GotoXY(x, y);
    cout << " " << text << " ";
    SetColor(15, 0);
}

void DrawActionBar(int selectedAction) {
    static int lastSelected = -1;

    int totalW = 0;
    for (int i = 0; i < GetActionCount(); i++) {
        totalW += TextDisplayWidth(GetActionLabel(i)) + 2;
        if (i + 1 < GetActionCount()) totalW += 2;
    }

    int x = CenterConsoleX(totalW, CONSOLE_COLS);
    int positions[8] = {};
    for (int i = 0; i < GetActionCount(); i++) {
        string label = GetActionLabel(i);
        positions[i] = x;
        x += TextDisplayWidth(label) + 4;
    }

    if (selectedAction < 0 || lastSelected < 0) {
        GotoXY(16, HUD_HELP_Y);
        printf("\x1b[48;2;%d;%d;%dm", HUD_GROUND_R, HUD_GROUND_G, HUD_GROUND_B);
        cout << string(CONSOLE_COLS - 32, ' ');
        printf("\x1b[0m");
        for (int i = 0; i < GetActionCount(); i++) {
            DrawActionBox(positions[i], HUD_HELP_Y, GetActionLabel(i), i == selectedAction);
        }
    }
    else if (selectedAction != lastSelected) {
        DrawActionBox(positions[lastSelected], HUD_HELP_Y, GetActionLabel(lastSelected), false);
        DrawActionBox(positions[selectedAction], HUD_HELP_Y, GetActionLabel(selectedAction), true);
    }
    lastSelected = selectedAction;
}

void DrawPlayerInfo() {
    for (int y = INFO_Y - 1; y <= RESULT_Y + 1; y++) {
        ClearHudLine(y);
    }
    DrawHudFrame();

    string p1 = ClipText(_PLAYER1_NAME, 14);
    string p2 = ClipText(_PLAYER2_NAME, 14);
    string playerLine = "[X] " + p1 + "   vs   [O] " + p2;
    string helpLine = "";

    PrintHudCentered(HUD_PLAYER_Y, playerLine, 15);
    DrawActionBar(-1);
    PrintHudTextWithBg(TIMER_X, TIMER_Y, L(TextId::HudTime) + "30s      ", 11);
    DrawStatusInfo(false);
}

void UpdateTurnInfo() {
    lock_guard<mutex> lock(consoleMutex);
    
    string turnText = L(TextId::HudTurn);
    if (_TURN == true) {
        PrintHudTextWithBg(TURN_INFO_X, TURN_INFO_Y, turnText + "X    ", 12);
    }
    else {
        PrintHudTextWithBg(TURN_INFO_X, TURN_INFO_Y, turnText + "O    ", 10);
    }
    DrawIngameTurnBadges();
    
    GotoXY(_X, _Y); // Trả con trỏ về bàn cờ
}
