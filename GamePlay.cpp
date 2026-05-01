#include "GamePlay.h"
#include "GameStatus.h"
#include "DrawBoard.h"
#include "ControlConsole.h"
#include "UserInfo.h"
#include "FinishProcess.h"
#include "GameTimer.h"
#include "CharSprite.h"   // <<< THÊM MỚI: hệ thống sprite nhân vật

using namespace std;

void DrawCell(int x, int y, int bg_color) {
    lock_guard<std::mutex> lock(consoleMutex);
    int c = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].x == x && _A[i][j].y == y) {
                c = _A[i][j].c;
                break;
            }
        }
    }

    GotoXY(x, y);
    if (c == 0) {
        SetColor(0, bg_color);
        cout << " ";
    }
    else if (c == -1) {
        SetColor(12, bg_color);  // X màu đỏ
        cout << "X";
    }
    else if (c == 1) {
        SetColor(10, bg_color);  // O màu xanh lá
        cout << "O";
    }

    SetColor(0, 15);
}

void StartGame() {
    system("color F0");
    system("cls");
    SetConsoleWindow(1000, 800);   // <<< TĂNG chiều cao cửa sổ
    HideCursor();
    ResetData();

    DrawBoard(BOARD_SIZE);         // Vẽ bàn cờ (giữa màn hình, LEFT=38)
    DrawPlayerInfo();              // Vẽ panel thông tin (bên phải)
    DrawBothSprites();             // <<< THÊM MỚI: vẽ sprite 2 nhân vật
    UpdateTurnInfo();

    GotoXY(_X, _Y);
    DrawCell(_X, _Y, 11);
}

void MoveRight() {
    if (_X < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].x) {
        DrawCell(_X, _Y, 15);
        _X += 4;
        DrawCell(_X, _Y, 11);
    }
}

void MoveLeft() {
    if (_X > _A[0][0].x) {
        DrawCell(_X, _Y, 15);
        _X -= 4;
        DrawCell(_X, _Y, 11);
    }
}

void MoveDown() {
    if (_Y < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y) {
        DrawCell(_X, _Y, 15);
        _Y += 2;
        DrawCell(_X, _Y, 11);
    }
}

void MoveUp() {
    if (_Y > _A[0][0].y) {
        DrawCell(_X, _Y, 15);
        _Y -= 2;
        DrawCell(_X, _Y, 11);
    }
}

int CheckBoard(int pX, int pY) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].x == pX && _A[i][j].y == pY && _A[i][j].c == 0) {
                if (_TURN == true) _A[i][j].c = -1;
                else               _A[i][j].c = 1;
                return _A[i][j].c;
            }
        }
    }
    return 0;
}

void ProcessMove(int _COMMAND, bool validEnter, bool& isPlaying) {
    if (_COMMAND == 'A' || _COMMAND == 75) MoveLeft();
    else if (_COMMAND == 'W' || _COMMAND == 72) MoveUp();
    else if (_COMMAND == 'S' || _COMMAND == 80) MoveDown();
    else if (_COMMAND == 'D' || _COMMAND == 77) MoveRight();
    else if (_COMMAND == 13) {
        int checkRes = CheckBoard(_X, _Y);

        if (checkRes == 0) {
            validEnter = false;
        }
        else {
            DrawCell(_X, _Y, 11);

            int r = (_Y - TOP - 1) / 2;
            int c = (_X - LEFT - 2) / 4;

            if (currentStep < (int)moveHistory.size()) {
                moveHistory.erase(moveHistory.begin() + currentStep, moveHistory.end());
            }
            moveHistory.push_back({ r, c, checkRes });
            currentStep++;
        }

        if (validEnter == true) {
            timeLeft = TURN_TIME_LIMIT;
            switch (ProcessFinish(TestBoard())) {
            case -1: case 1: case 0:
                if (AskContinue() != 'Y') {
                    isPlaying = false;
                }
                else {
                    StartGame();
                    timeLeft = TURN_TIME_LIMIT;
                }
            }
        }
        validEnter = true;
    }
}

// Đánh ngẫu nhiên khi hết thời gian
void PlayRandomMove() {
    vector<pair<int, int>> emptyCells;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (_A[i][j].c == 0)
                emptyCells.push_back({ i, j });

    if (!emptyCells.empty()) {
        DrawCell(_X, _Y, 15);
        srand((unsigned int)time(NULL));
        int index = rand() % emptyCells.size();
        int r = emptyCells[index].first;
        int c = emptyCells[index].second;
        _X = _A[r][c].x;
        _Y = _A[r][c].y;
        int checkRes = CheckBoard(_X, _Y);

        if (currentStep < (int)moveHistory.size()) {
            moveHistory.erase(moveHistory.begin() + currentStep, moveHistory.end());
        }
        moveHistory.push_back({ r, c, checkRes });
        currentStep++;

        DrawCell(_X, _Y, 11);
        GotoXY(_X, _Y);
    }
}

void UndoMove() {
    if (currentStep <= 0) return;

    int undoCount = (_BOT_MODE) ? 2 : 1;

    for (int k = 0; k < undoCount; k++) {
        if (currentStep <= 0) break;
        currentStep--;
        MoveNode& move = moveHistory[currentStep];
        _A[move.row][move.col].c = 0;
        DrawCell(_A[move.row][move.col].x, _A[move.row][move.col].y, 15);
        _TURN = !_TURN;
    }

    UpdateTurnInfo();

    MoveNode& lastUndo = moveHistory[currentStep];
    DrawCell(_X, _Y, 15);
    _X = _A[lastUndo.row][lastUndo.col].x;
    _Y = _A[lastUndo.row][lastUndo.col].y;
    DrawCell(_X, _Y, 11);
}

void RedoMove() {
    if (currentStep >= (int)moveHistory.size()) return;

    int redoCount = (_BOT_MODE) ? 2 : 1;

    for (int k = 0; k < redoCount; k++) {
        if (currentStep >= (int)moveHistory.size()) break;
        MoveNode& move = moveHistory[currentStep];
        _A[move.row][move.col].c = move.c;
        DrawCell(_A[move.row][move.col].x, _A[move.row][move.col].y, 15);
        currentStep++;
        _TURN = !_TURN;
    }

    UpdateTurnInfo();

    MoveNode& lastRedo = moveHistory[currentStep - 1];
    DrawCell(_X, _Y, 15);
    _X = _A[lastRedo.row][lastRedo.col].x;
    _Y = _A[lastRedo.row][lastRedo.col].y;
    DrawCell(_X, _Y, 11);
}
