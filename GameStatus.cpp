#include "GameStatus.h"

_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN;
int _COMMAND;
int _X, _Y;

bool _BOT_MODE = false;
int _BOT_DIFFICULTY = 2;

vector<MoveNode> moveHistory;
int currentStep = 0;

void ResetData() {
    //xóa lịch sử khi bắt đầu ván mới
    moveHistory.clear();
    currentStep = 0;

    for (int i = 0; i < BOARD_SIZE; i++) {

        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j].x = 4 * j + LEFT + 2;
            _A[i][j].y = 2 * i + TOP + 1;
            _A[i][j].c = 0;
        }
        _TURN = true; _COMMAND = -1;
        _X = _A[0][0].x; _Y = _A[0][0].y;
    }
}