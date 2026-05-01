#include "FinishProcess.h"
#include "GameStatus.h"
#include "ControlConsole.h"
#include "UserInfo.h"

using namespace std;

void GarbageCollect() {
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            _A[i][j].c = 0;
}

void ExitGame() {
    system("cls");
    GarbageCollect();
}

int ProcessFinish(int pWhoWin) {
    // Hiển thị kết quả ngay dưới bàn cờ, căn với cột LEFT của bàn cờ
    // _A[11][11].y = 2*11 + TOP + 1 = 22 + 3 + 1 = 26  =>  +2 = dòng 28
    GotoXY(LEFT, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 2);

    SetColor(1, 15);  // Chữ xanh dương, nền trắng

    switch (pWhoWin) {
    case -1:
        cout << _PLAYER1_NAME << " (X) da THANG va " << _PLAYER2_NAME << " (O) da THUA!          ";
        break;
    case  1:
        cout << _PLAYER2_NAME << " (O) da THANG va " << _PLAYER1_NAME << " (X) da THUA!          ";
        break;
    case  0:
        cout << "Tran dau HOA! Ban co da het o trong.                          ";
        break;
    case  2:
        _TURN = !_TURN;
        UpdateTurnInfo();
        break;
    }

    SetColor(0, 15);
    GotoXY(_X, _Y);
    return pWhoWin;
}

int AskContinue() {
    // Hiển thị câu hỏi ngay dưới thông báo kết quả (dòng 30)
    GotoXY(LEFT, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 4);
    SetColor(0, 15);
    cout << "Nhan 'y' de choi lai, 'n' de thoat: ";
    return toupper(_getch());
}

int TestBoard() {
    // 1. Kiểm tra người thắng
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c != 0) {
                int c = _A[i][j].c;
                if (j <= BOARD_SIZE - 5
                    && _A[i][j + 1].c == c && _A[i][j + 2].c == c
                    && _A[i][j + 3].c == c && _A[i][j + 4].c == c) return c;
                if (i <= BOARD_SIZE - 5
                    && _A[i + 1][j].c == c && _A[i + 2][j].c == c
                    && _A[i + 3][j].c == c && _A[i + 4][j].c == c) return c;
                if (i <= BOARD_SIZE - 5 && j <= BOARD_SIZE - 5
                    && _A[i + 1][j + 1].c == c && _A[i + 2][j + 2].c == c
                    && _A[i + 3][j + 3].c == c && _A[i + 4][j + 4].c == c) return c;
                if (i >= 4 && j <= BOARD_SIZE - 5
                    && _A[i - 1][j + 1].c == c && _A[i - 2][j + 2].c == c
                    && _A[i - 3][j + 3].c == c && _A[i - 4][j + 4].c == c) return c;
            }
        }
    }

    // 2. Kiểm tra hòa (đầy bàn)
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (_A[i][j].c == 0) return 2;  // Vẫn còn ô trống → chưa kết thúc

    return 0;  // Hòa
}
