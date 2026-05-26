#include "FinishProcess.h"
#include "GameStatus.h"
#include "ControlConsole.h"
#include "UserInfo.h"
#include "Sound.h"

#include "Menu.h"
#include "btn_normal.h"
#include "btn_hover.h"
#include <utility>
#include <vector>
#include <windows.h>

using namespace std;

static bool CollectWinningCells(int winner, vector<pair<int, int>>& cells) {
    if (winner == 0 || winner == 2) return false;

    const int dirs[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {-1, 1}
    };

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (_A[r][c].c != winner) continue;

            for (int d = 0; d < 4; d++) {
                cells.clear();
                bool ok = true;

                for (int k = 0; k < 5; k++) {
                    int rr = r + dirs[d][0] * k;
                    int cc = c + dirs[d][1] * k;

                    if (rr < 0 || rr >= BOARD_SIZE || cc < 0 || cc >= BOARD_SIZE || _A[rr][cc].c != winner) {
                        ok = false;
                        break;
                    }
                    cells.push_back({ rr, cc });
                }

                if (ok) return true;
            }
        }
    }

    cells.clear();
    return false;
}

static void DrawWinningCell(int r, int c) {
    int x = _A[r][c].x;
    int y = _A[r][c].y;
    char mark = (_A[r][c].c == -1) ? 'X' : 'O';

    GotoXY(x - 1, y);
    SetColor((_A[r][c].c == -1) ? 12 : 10, 14);
    cout << " " << mark << " ";
}

static void RestoreWinningCell(int r, int c) {
    int x = _A[r][c].x;
    int y = _A[r][c].y;

    GotoXY(x - 1, y);
    SetColor(0, BOARD_BG_COLOR);
    cout << "   ";

    if (_A[r][c].c == -1) {
        GotoXY(x, y);
        SetColor(12, BOARD_BG_COLOR);
        cout << "X";
    }
    else if (_A[r][c].c == 1) {
        GotoXY(x, y);
        SetColor(10, BOARD_BG_COLOR);
        cout << "O";
    }

    SetColor(0, 15);
}

static void HighlightWinningLine(int winner) {
    vector<pair<int, int>> cells;
    if (!CollectWinningCells(winner, cells)) return;

    PlayWinSound();

    for (const auto& cell : cells) {
        DrawWinningCell(cell.first, cell.second);
    }
    GotoXY(_X, _Y);

    Sleep(5000);

    for (const auto& cell : cells) {
        RestoreWinningCell(cell.first, cell.second);
    }
    GotoXY(_X, _Y);
}

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
    if (pWhoWin == -1 || pWhoWin == 1) {
        HighlightWinningLine(pWhoWin);
    }

    GotoXY(RESULT_X, RESULT_Y);
    cout << string(80, ' ');
    GotoXY(RESULT_X, RESULT_Y);

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
    // Gọi màn hình Yes/No căn giữa, không xóa màn hình
    bool yes = GraphicalYesNo("Ban co muon choi tiep khong?", 10, false,
        BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
    return yes ? 'Y' : 'N';
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
