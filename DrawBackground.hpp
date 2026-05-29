#pragma once

#include <iostream>
#include <cstdio>
#include <string>
#include "bg_menu.h"
#include "ControlConsole.h"
#include "bg_game.h"
#include "bg_load.h"
using namespace std;

inline void DrawMenuBackground() {
    char pixel[64];

    for (int i = 0; i < BG_MENU_H / 2; i++) {
        // Nhảy đến đúng dòng console trước khi vẽ
        GotoXY(0, i);

        string row_buf = "";
        int drawW = (i == CONSOLE_LINES - 1) ? BG_MENU_W - 1 : BG_MENU_W;
        for (int j = 0; j < drawW; j++) {
            int r_top = BG_MENU[i * 2][j][0];
            int g_top = BG_MENU[i * 2][j][1];
            int b_top = BG_MENU[i * 2][j][2];

            int r_bot = BG_MENU[i * 2 + 1][j][0];
            int g_bot = BG_MENU[i * 2 + 1][j][1];
            int b_bot = BG_MENU[i * 2 + 1][j][2];

            // Sử dụng snprintf thay cho sprintf
            snprintf(pixel, sizeof(pixel), "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm\xE2\x96\x80",
                r_top, g_top, b_top, r_bot, g_bot, b_bot);
            row_buf += pixel;
        }
        cout << row_buf;
    }
    cout << "\x1b[0m";
    cout.flush();
}

inline void DrawIngameBackground() {
    char pixel[64];

    for (int i = 0; i < BG_GAME_H / 2; i++) {
        // Nhảy đến đúng dòng console trước khi vẽ
        GotoXY(0, i);

        string row_buf = "";
        int drawW = (i == CONSOLE_LINES - 1) ? BG_GAME_W - 1 : BG_GAME_W;
        for (int j = 0; j < drawW; j++) {
            int r_top = BG_GAME[i * 2][j][0];
            int g_top = BG_GAME[i * 2][j][1];
            int b_top = BG_GAME[i * 2][j][2];

            int r_bot = BG_GAME[i * 2 + 1][j][0];
            int g_bot = BG_GAME[i * 2 + 1][j][1];
            int b_bot = BG_GAME[i * 2 + 1][j][2];

            // Sử dụng snprintf thay cho sprintf
            snprintf(pixel, sizeof(pixel), "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm\xE2\x96\x80",
                r_top, g_top, b_top, r_bot, g_bot, b_bot);
            row_buf += pixel;
        }
        cout << row_buf;
    }
    cout << "\x1b[0m";
    cout.flush();
}


inline void DrawLoadgameBackground() {
    char pixel[64];

    for (int i = 0; i < BG_LOAD_H / 2; i++) {
        // Nhảy đến đúng dòng console trước khi vẽ
        GotoXY(0, i);

        string row_buf = "";
        int drawW = (i == CONSOLE_LINES - 1) ? BG_LOAD_W - 1 : BG_LOAD_W;
        for (int j = 0; j < drawW; j++) {
            int r_top = BG_LOAD[i * 2][j][0];
            int g_top = BG_LOAD[i * 2][j][1];
            int b_top = BG_LOAD[i * 2][j][2];

            int r_bot = BG_LOAD[i * 2 + 1][j][0];
            int g_bot = BG_LOAD[i * 2 + 1][j][1];
            int b_bot = BG_LOAD[i * 2 + 1][j][2];

            // Sử dụng snprintf thay cho sprintf
            snprintf(pixel, sizeof(pixel), "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm\xE2\x96\x80",
                r_top, g_top, b_top, r_bot, g_bot, b_bot);
            row_buf += pixel;
        }
        cout << row_buf;
    }
    cout << "\x1b[0m";
    cout.flush();
}
