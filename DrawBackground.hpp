#pragma once

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include "bg_menu.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "bg_game.h"
#include "bg_load.h"
using namespace std;

template <int H, int W>
inline void DrawScaledRgbBackground(const int (&bg)[H][W][3]) {
    if (IsWindowsTerminal()) {
        SyncConsoleLayoutToCurrentWindow();
    }

    char pixel[64];
    int lines = max(1, CONSOLE_LINES);
    int cols = max(1, CONSOLE_COLS);
    int sourceRows = max(1, H / 2);

    for (int i = 0; i < lines; i++) {
        GotoXY(0, i);

        string rowBuf;
        int drawW = (i == lines - 1) ? cols - 1 : cols;
        int sourceRow = (lines <= 1) ? 0 : (i * (sourceRows - 1)) / (lines - 1);
        int top = min(H - 2, sourceRow * 2);

        for (int j = 0; j < drawW; j++) {
            int sourceCol = (cols <= 1) ? 0 : (j * (W - 1)) / (cols - 1);

            int rTop = bg[top][sourceCol][0];
            int gTop = bg[top][sourceCol][1];
            int bTop = bg[top][sourceCol][2];

            int rBot = bg[top + 1][sourceCol][0];
            int gBot = bg[top + 1][sourceCol][1];
            int bBot = bg[top + 1][sourceCol][2];

            snprintf(pixel, sizeof(pixel), "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm\xE2\x96\x80",
                rTop, gTop, bTop, rBot, gBot, bBot);
            rowBuf += pixel;
        }
        cout << rowBuf;
    }
    cout << "\x1b[0m";
    cout.flush();
}

inline void DrawMenuBackground() {
    DrawScaledRgbBackground(BG_MENU);
}

inline void DrawIngameBackground() {
    DrawScaledRgbBackground(BG_GAME);
}

inline void DrawLoadgameBackground() {
    DrawScaledRgbBackground(BG_LOAD);
}
