#pragma once
#include <iostream>
#include <vector>
using namespace std;


#define BOARD_SIZE 12
#define CONSOLE_COLS 120
#define CONSOLE_LINES 40
#define BOARD_BG_COLOR 7
#define BOARD_CURSOR_COLOR 11


#define LEFT  ((CONSOLE_COLS - (BOARD_SIZE * 4 + 1)) / 2)
#define TOP    0


#define SPRITE_DRAW_W   22
#define SPRITE_DRAW_H   16
#define LEFT_SPRITE_X   6
#define RIGHT_SPRITE_X  (CONSOLE_COLS - SPRITE_DRAW_W - 6)
#define SPRITE_START_Y  6


#define INFO_X  0
#define INFO_Y  (TOP + BOARD_SIZE * 2 + 2)


#define HUD_PLAYER_Y   INFO_Y
#define TIMER_X        ((CONSOLE_COLS - 44) / 2)
#define TIMER_Y        (INFO_Y + 1)
#define TURN_INFO_X    (TIMER_X + 22)
#define TURN_INFO_Y    TIMER_Y
#define STATUS_X       ((CONSOLE_COLS - 30) / 2)
#define STATUS_Y       (INFO_Y + 2)
#define HUD_HELP_Y     (INFO_Y + 3)


#define BOT_MSG_X  ((CONSOLE_COLS - 34) / 2)
#define BOT_MSG_Y  (INFO_Y + 4)
#define RESULT_X   ((CONSOLE_COLS - 80) / 2)
#define RESULT_Y   (INFO_Y + 5)


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
