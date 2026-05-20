#pragma once
#include <iostream>
#include <vector>
using namespace std;

// ============================================================
//  Cấu hình bàn cờ
// ============================================================
#define BOARD_SIZE 12

// Bàn cờ căn giữa màn hình (~125 cols / 2 - 49/2 = 38)
#define LEFT  38
#define TOP    3

// ============================================================
//  Vị trí Sprite nhân vật (2 bên bàn cờ)
//  Mỗi pixel = 2 chars → sprite 16px rộng chiếm 32 cols
//  Trái : cols  2 – 33  (gap 4 đến board)
//  Phải : cols 91 – 122 (gap 4 sau board kết thúc col 86)
// ============================================================
#define LEFT_SPRITE_X   2
#define RIGHT_SPRITE_X  91
#define SPRITE_START_Y  3

// ============================================================
//  Vị trí panel thông tin (bên phải, bên dưới sprite phải)
//  Sprite phải kết thúc ở dòng 3 + 20 - 1 = 22
//  Panel bắt đầu từ dòng 24
// ============================================================
#define INFO_X  91
#define INFO_Y  24

// ============================================================
//  Vị trí đồng hồ đếm ngược (bên dưới panel thông tin)
//  Panel chiếm ~18 dòng → timer ở dòng 24 + 18 = 42
// ============================================================
// Tính nhẩm: col3 là LEFT + 70. Giả sử LEFT = 10 thì TIMER_X = 80. startY ở trên tầm 30.
#define TIMER_X (LEFT + 55) 
#define TIMER_Y (TOP + BOARD_SIZE * 2 + 3) // Khớp với dòng StartY + 1

// ============================================================
//  Vị trí thông báo Bot đang suy nghĩ
// ============================================================
#define BOT_MSG_X  91
#define BOT_MSG_Y  33

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
