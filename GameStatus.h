#pragma once
#include <iostream>
#include <vector>
using namespace std;

// ============================================================
//  Cấu hình bàn cờ
// ============================================================
#define BOARD_SIZE 12
#define CONSOLE_COLS 120
#define CONSOLE_LINES 40
#define BOARD_BG_COLOR 7
#define BOARD_CURSOR_COLOR 11

// Bàn cờ căn giữa màn hình (~125 cols / 2 - 49/2 = 38)
#define LEFT  ((CONSOLE_COLS - (BOARD_SIZE * 4 + 1)) / 2)
#define TOP    0

// ============================================================
//  Vị trí Sprite nhân vật (2 bên bàn cờ)
//  Mỗi pixel = 2 chars → sprite 16px rộng chiếm 32 cols
//  Trái : cols  2 – 33  (gap 4 đến board)
//  Phải : cols 91 – 122 (gap 4 sau board kết thúc col 86)
// ============================================================
#define SPRITE_DRAW_W   28
#define SPRITE_DRAW_H   22
#define LEFT_SPRITE_X   3
#define RIGHT_SPRITE_X  (CONSOLE_COLS - SPRITE_DRAW_W - 3)
#define SPRITE_START_Y  1

// ============================================================
//  Vị trí panel thông tin (bên phải, bên dưới sprite phải)
//  Sprite phải kết thúc ở dòng 3 + 20 - 1 = 22
//  Panel bắt đầu từ dòng 24
// ============================================================
#define INFO_X  0
#define INFO_Y  (TOP + BOARD_SIZE * 2 + 1)

// ============================================================
//  Vị trí đồng hồ đếm ngược (bên dưới panel thông tin)
//  Panel chiếm ~18 dòng → timer ở dòng 24 + 18 = 42
// ============================================================
// Tính nhẩm: col3 là LEFT + 70. Giả sử LEFT = 10 thì TIMER_X = 80. startY ở trên tầm 30.
#define HUD_PLAYER_Y   INFO_Y
#define TIMER_X        ((CONSOLE_COLS - 44) / 2)
#define TIMER_Y        (INFO_Y + 1)
#define TURN_INFO_X    (TIMER_X + 22)
#define TURN_INFO_Y    TIMER_Y
#define STATUS_X       ((CONSOLE_COLS - 30) / 2)
#define STATUS_Y       (INFO_Y + 2)
#define HUD_HELP_Y     (INFO_Y + 3)

// ============================================================
//  Vị trí thông báo Bot đang suy nghĩ
// ============================================================
#define BOT_MSG_X  ((CONSOLE_COLS - 34) / 2)
#define BOT_MSG_Y  (INFO_Y + 4)
#define RESULT_X   ((CONSOLE_COLS - 80) / 2)
#define RESULT_Y   (INFO_Y + 5)

// ============================================================
//  Struct & biến toàn cục
// ============================================================
struct _POINT { int x, y, c; };
extern _POINT _A[BOARD_SIZE][BOARD_SIZE];
extern bool   _TURN;
extern int    _COMMAND;
extern int    _X, _Y;

extern bool _BOT_MODE;
extern int  _BOT_DIFFICULTY;

struct MoveNode {
    int row, col, c;
};

extern vector<MoveNode> moveHistory;
extern int currentStep;

#include <string>
extern std::string _PLAYER1_NAME;
extern std::string _PLAYER2_NAME;

void ResetData();
