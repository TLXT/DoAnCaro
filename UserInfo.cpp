#include "UserInfo.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "Menu.h"
#include "Language.h"
#include "Character.h"
#include "GameTimer.h"
#include <iostream>
#include <algorithm>
#include <cstdio>

using namespace std;

static const int HUD_GROUND_R = 92;
static const int HUD_GROUND_G = 58;
static const int HUD_GROUND_B = 32;
static const int HUD_PANEL_Y = TOP + BOARD_SIZE * 2 + 1;
static const int HUD_TIMER_Y = HUD_PANEL_Y;
static const int HUD_ACTION_Y = HUD_PANEL_Y;
static const int SIDE_TIMER_Y = SPRITE_START_Y + SPRITE_DRAW_H + 2;
static int gLastTimerSeconds = -1;
static bool gLastTimerTurn = true;
static bool gForceTimerDraw = true;
static bool gForceActionDraw = true;

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
    case 2: return "RESTART";
    case 3: return L(HudPause);
    default: return "";
    }
}

static void ClearHudLine(int y) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { 0, static_cast<SHORT>(y) };
    DWORD written = 0;
    FillConsoleOutputCharacterA(hOut, ' ', CONSOLE_COLS, coord, &written);
    FillConsoleOutputAttribute(hOut, 6 * 16, CONSOLE_COLS, coord, &written);

    GotoXY(0, y);
    printf("\x1b[48;2;%d;%d;%dm", HUD_GROUND_R, HUD_GROUND_G, HUD_GROUND_B);
    cout << string(CONSOLE_COLS - 1, ' ');
    printf("\x1b[0m");
}

void DrawStatusInfo(bool paused) {
    (void)paused;
}

static string ClipText(const string& text, int maxLen) {
    if ((int)text.length() <= maxLen) return text;
    if (maxLen <= 3) return text.substr(0, max(0, maxLen));
    return text.substr(0, maxLen - 3) + "...";
}

static void DrawBoxBorder(int x, int y, int w, int h, bool selected) {
    int r = selected ? 255 : 80;
    int g = selected ? 240 : 245;
    int b = selected ? 80 : 230;
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", r, g, b, HUD_GROUND_R, HUD_GROUND_G, HUD_GROUND_B);
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

static void FillHudRgbRect(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;

    int startX = max(0, x);
    int startY = max(0, y);
    int endX = min(CONSOLE_COLS - 1, x + w);
    int endY = min(CONSOLE_LINES, y + h);
    if (startX >= endX || startY >= endY) return;

    string line(endX - startX, ' ');
    printf("\x1b[48;2;%d;%d;%dm", HUD_GROUND_R, HUD_GROUND_G, HUD_GROUND_B);
    for (int row = startY; row < endY; row++) {
        GotoXY(startX, row);
        cout << line;
    }
    printf("\x1b[0m");
}

void DrawTimerBox(const string& text, int color) {
    const int w = 32;
    const int h = 3;
    const int x = CenterConsoleX(w, CONSOLE_COLS);
    const int y = HUD_TIMER_Y;

    DrawBoxBorder(x, y, w, h, false);

    GotoXY(x + 1, y + 1);
    printf("\x1b[48;2;%d;%d;%dm", HUD_GROUND_R, HUD_GROUND_G, HUD_GROUND_B);
    cout << string(w - 2, ' ');
    printf("\x1b[0m");

    int textX = x + (w - TextDisplayWidth(text)) / 2;
    if (textX < x + 1) textX = x + 1;
    PrintHudTextWithBg(textX, y + 1, text, color);
}

static void DrawSideTimerBox(int x, int y, int w, bool active, bool isPlayerX, int secondsLeft) {
    int fgR = active ? 255 : 175;
    int fgG = active ? 255 : 180;
    int fgB = active ? 245 : 185;
    int bgR = active ? (isPlayerX ? 150 : 20) : 45;
    int bgG = active ? (isPlayerX ? 28 : 115) : 45;
    int bgB = active ? (isPlayerX ? 35 : 55) : 48;

    string text = to_string(secondsLeft) + "s";
    int textX = x + (w - TextDisplayWidth(text)) / 2;
    if (textX < x) textX = x;

    GotoXY(x, y);
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", fgR, fgG, fgB, bgR, bgG, bgB);
    cout << string(w, ' ');
    GotoXY(textX, y);
    cout << text;
    printf("\x1b[0m");
}

void DrawTurnTimers(int secondsLeft) {
    int safeSeconds = max(0, min(turnTimeLimit, secondsLeft));
    bool xActive = _TURN == true;
    bool oActive = _TURN == false;
    if (!gForceTimerDraw && safeSeconds == gLastTimerSeconds && _TURN == gLastTimerTurn) {
        return;
    }

    DrawSideTimerBox(LEFT_SPRITE_X, SIDE_TIMER_Y, SPRITE_DRAW_W, xActive, true, xActive ? safeSeconds : turnTimeLimit);
    DrawSideTimerBox(RIGHT_SPRITE_X, SIDE_TIMER_Y, SPRITE_DRAW_W, oActive, false, oActive ? safeSeconds : turnTimeLimit);
    gLastTimerSeconds = safeSeconds;
    gLastTimerTurn = _TURN;
    gForceTimerDraw = false;
}

static void DrawActionBox(int x, int y, int w, const string& text, bool selected, int index) {
    FillHudRgbRect(x, y, w, 3);
    DrawBoxBorder(x, y, w, 3, selected);

    int fgR = selected ? 10 : 225;
    int fgG = selected ? 10 : 245;
    int fgB = selected ? 10 : 255;
    int bgR = 20;
    int bgG = 88;
    int bgB = 150;
    if (index == 1) { bgR = 20; bgG = 130; bgB = 105; }
    else if (index == 2) { bgR = 180; bgG = 95; bgB = 30; }
    else if (index == 3) { bgR = 150; bgG = 65; bgB = 160; }
    if (selected) { bgR = 255; bgG = 240; bgB = 90; }

    GotoXY(x + 1, y + 1);
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", fgR, fgG, fgB, bgR, bgG, bgB);
    cout << string(w - 2, ' ');

    string shown = text;
    if (TextDisplayWidth(shown) > w - 2) {
        shown = ClipText(shown, w - 2);
    }

    int textX = x + (w - TextDisplayWidth(shown)) / 2;
    if (textX < x + 1) textX = x + 1;
    GotoXY(textX, y + 1);
    cout << shown;
    printf("\x1b[0m");
}

void DrawActionBar(int selectedAction) {
    lock_guard<mutex> lock(consoleMutex);
    static int lastSelected = -2;

    int actionCount = GetActionCount();
    if (selectedAction < -1) selectedAction = -1;
    if (selectedAction >= actionCount) selectedAction = actionCount - 1;
    if (!gForceActionDraw && selectedAction == lastSelected) {
        GotoXY(_X, _Y);
        return;
    }

    const int buttonW = 16;
    const int gap = 3;
    int widths[8] = {};
    for (int i = 0; i < actionCount; i++) {
        widths[i] = max(buttonW, TextDisplayWidth(GetActionLabel(i)) + 4);
    }

    int totalW = 0;
    for (int i = 0; i < actionCount; i++) {
        totalW += widths[i];
        if (i + 1 < actionCount) totalW += gap;
    }
    int x = CenterConsoleX(totalW, CONSOLE_COLS);
    int positions[8] = {};
    for (int i = 0; i < actionCount; i++) {
        positions[i] = x;
        x += widths[i] + gap;
    }

    FillHudRgbRect(0, HUD_ACTION_Y, CONSOLE_COLS - 1, 3);
    for (int i = 0; i < actionCount; i++) {
        DrawActionBox(positions[i], HUD_ACTION_Y, widths[i], GetActionLabel(i), i == selectedAction, i);
    }
    lastSelected = selectedAction;
    gForceActionDraw = false;
    GotoXY(_X, _Y);
}

void DrawPlayerInfo() {
    for (int y = HUD_PANEL_Y; y < CONSOLE_LINES; y++) {
        ClearHudLine(y);
    }

    gForceTimerDraw = true;
    gForceActionDraw = true;
    DrawTurnTimers(turnTimeLimit);
    DrawActionBar(-1);
}

void UpdateTurnInfo() {
    lock_guard<mutex> lock(consoleMutex);

    DrawIngameTurnBadges();
    DrawTurnTimers(turnTimeLimit);

    GotoXY(_X, _Y);
}
