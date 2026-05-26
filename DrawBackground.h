#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include "bg_forest.h"
#include "ControlConsole.h"

using namespace std;

// FIX: Dùng GotoXY cho từng dòng thay vì \x1b[E]
// → Không bao giờ bị sọc đen, không bao giờ bị scroll
inline void DrawBackground() {
    char pixel[64];
    
    for (int i = 0; i < BG_FOREST_H / 2; i++) {
        // Nhảy đến đúng dòng console trước khi vẽ
        GotoXY(0, i);
        
        string row_buf = "";
        for (int j = 0; j < BG_FOREST_W; j++) {
            int r_top = BG_FOREST[i * 2][j][0];
            int g_top = BG_FOREST[i * 2][j][1];
            int b_top = BG_FOREST[i * 2][j][2];

            int r_bot = BG_FOREST[i * 2 + 1][j][0];
            int g_bot = BG_FOREST[i * 2 + 1][j][1];
            int b_bot = BG_FOREST[i * 2 + 1][j][2];

            sprintf(pixel, "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm\xE2\x96\x80",
                    r_top, g_top, b_top, r_bot, g_bot, b_bot);
            row_buf += pixel;
        }
        // In cả dòng 1 lần (nhanh), KHÔNG xuống dòng bằng escape
        cout << row_buf;
    }
    cout << "\x1b[0m";
    cout.flush();
}
