#pragma once
#include <iostream>
#include <vector>
using namespace std;

#define BOARD_SIZE 12
#define LEFT 3
#define TOP 1

struct _POINT { int x, y, c; }; // x: tọa độ dòng, y: tọa độ cột, c: đánh dấu
extern _POINT _A[BOARD_SIZE][BOARD_SIZE];
extern bool _TURN;
extern int _COMMAND;
extern int _X, _Y;


extern bool _BOT_MODE;
extern int _BOT_DIFFICULTY;

struct MoveNode {
    int row, col, c;
};

extern vector<MoveNode> moveHistory; //lưu lịch sử nước đi
extern int currentStep; //nước đi hiện tại

void ResetData();
void UndoMove();
void RedoMove();

