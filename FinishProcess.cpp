#include "FinishProcess.h"
#include "GameStatus.h"
#include "ControlConsole.h"
#include "UserInfo.h"

using namespace std;

void GarbageCollect() {
    // Giải phóng bộ nhớ cho ma trận bàn cờ
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j].c = 0;
        }
    }
}

void ExitGame() {
    system("cls");
    GarbageCollect();
}

int ProcessFinish(int pWhoWin) {
    GotoXY(0, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 2);

    // chữ xanh dương (1) nền trắng (15)
    SetColor(1, 15);

    switch (pWhoWin) {
    case -1: cout << "Nguoi choi X da thang va nguoi choi O da thua!\n"; break;
    case 1:  cout << "Nguoi choi O da thang va nguoi choi X da thua!\n"; break;
    case 0:  cout << "Hai ben hoa nhau! Ban co da full o.          \n"; break;
    case 2:
        _TURN = !_TURN;
        UpdateTurnInfo();
        break;
    }

    SetColor(0, 15);
    GotoXY(_X, _Y);
    return pWhoWin;
}

int AskContinue() {
    GotoXY(0, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 4);
    SetColor(0, 15);
    cout << "Nhan 'y/n' de tiep tuc/dung: ";
    return toupper(_getch());
}

int TestBoard() {
    // 1. Kiểm tra người thắng trước
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c != 0) {
                int c = _A[i][j].c;
                if (j <= BOARD_SIZE - 5 && _A[i][j + 1].c == c && _A[i][j + 2].c == c && _A[i][j + 3].c == c && _A[i][j + 4].c == c) return c;
                if (i <= BOARD_SIZE - 5 && _A[i + 1][j].c == c && _A[i + 2][j].c == c && _A[i + 3][j].c == c && _A[i + 4][j].c == c) return c;
                if (i <= BOARD_SIZE - 5 && j <= BOARD_SIZE - 5 && _A[i + 1][j + 1].c == c && _A[i + 2][j + 2].c == c && _A[i + 3][j + 3].c == c && _A[i + 4][j + 4].c == c) return c;
                if (i >= 4 && j <= BOARD_SIZE - 5 && _A[i - 1][j + 1].c == c && _A[i - 2][j + 2].c == c && _A[i - 3][j + 3].c == c && _A[i - 4][j + 4].c == c) return c;
            }
        }
    }

    // 2. Kiểm tra full ô
    bool isFull = true;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c == 0) {
                isFull = false;
                break;
            }
        }
    }

    if (isFull) return 0;

    return 2;
}