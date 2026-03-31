#include "GamePlay.h"
#include "GameStatus.h"
#include "DrawBoard.h"
#include "ControlConsole.h"
#include "UserInfo.h"
#include "FinishProcess.h"
#include"GameTimer.h"
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
        SetColor(12, bg_color); // X màu đỏ
        cout << "X";
    }
    else if (c == 1) {
        SetColor(10, bg_color); // O màu xanh lá
        cout << "O";
    }

    SetColor(0, 15); // chữ đen, nền trắng
}
void StartGame() {
    system("color F0");
    system("cls");
    SetConsoleWindow(1000, 600);
    HideCursor();
    ResetData();
    DrawBoard(BOARD_SIZE);

    DrawPlayerInfo();
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
                else _A[i][j].c = 1;
                return _A[i][j].c;
            }
        }
    }
    return 0;
}
void ProcessMove(int _COMMAND,bool validEnter,bool& isPlaying) {
    if (_COMMAND == 'A' || _COMMAND == 75) MoveLeft();
    else if (_COMMAND == 'W' || _COMMAND == 72) MoveUp();
    else if (_COMMAND == 'S' || _COMMAND == 80) MoveDown();
    else if (_COMMAND == 'D' || _COMMAND == 77) MoveRight();
    else if (_COMMAND == 13) {
        switch (CheckBoard(_X, _Y)) {
        case -1: DrawCell(_X, _Y, 11); break;
        case 1:  DrawCell(_X, _Y, 11); break;
        case 0:  validEnter = false;
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
//Dùng để thực hiện đánh ngẫu nhiên khi hết thời gian
void PlayRandomMove() {
    vector<pair<int, int>> emptyCells;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c == 0) {
                emptyCells.push_back({ i, j });
            }
        }
    }

    if (!emptyCells.empty()) {
        DrawCell(_X, _Y, 15);
        srand(time(NULL));
        int index = rand() % emptyCells.size();
        int r = emptyCells[index].first;
        int c = emptyCells[index].second;
        _X = _A[r][c].x;
        _Y = _A[r][c].y;
        int checkRes = CheckBoard(_X, _Y); // Đánh cờ và lấy giá trị (-1 hoặc 1)

        // --- QUAN TRỌNG: Lưu vào lịch sử nước đi ---
        if (currentStep < (int)moveHistory.size()) {
            moveHistory.erase(moveHistory.begin() + currentStep, moveHistory.end());
        }
        moveHistory.push_back({ r, c, checkRes });
        currentStep++;
        // -------------------------------------------

        DrawCell(_X, _Y, 11); // Vẽ quân cờ vừa đánh với highlight
        GotoXY(_X, _Y);
    }
}

void UndoMove() {
    if (currentStep <= 0) return; // Không còn nước nào để undo thì thoát luôn

    // Nếu Bot Mode: undo 2 nước (nước bot + nước người)
    int undoCount = (_BOT_MODE) ? 2 : 1;

    // Phòng trường hợp Bot Mode chỉ có 1 nước nhưng lại cố undo 2 nước
    for (int k = 0; k < undoCount; k++) {
        if (currentStep <= 0) break;

        currentStep--;
        MoveNode& move = moveHistory[currentStep];

        _A[move.row][move.col].c = 0;
        DrawCell(_A[move.row][move.col].x, _A[move.row][move.col].y, 15);

        _TURN = !_TURN;
    }

    UpdateTurnInfo();

    // Di chuyển con trỏ về ô vừa undo
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

    // Di chuyển con trỏ về ô vừa redo
    MoveNode& lastRedo = moveHistory[currentStep - 1];
    DrawCell(_X, _Y, 15);
    _X = _A[lastRedo.row][lastRedo.col].x;
    _Y = _A[lastRedo.row][lastRedo.col].y;
    DrawCell(_X, _Y, 11);
}
