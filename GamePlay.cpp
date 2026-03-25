#include "GamePlay.h"
#include "GameStatus.h"
#include "DrawBoard.h"
#include "ControlConsole.h"
#include "UserInfo.h"

using namespace std;

void DrawCell(int x, int y, int bg_color) {
    lock_guard<std::mutex> lock(consoleMutex); //Cập nhật dòng khóa đảm bảo tránh xong đột khi dùng hàm GotoXY
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
    //lock_guard<std::mutex> lock(consoleMutex); //Cập nhật dòng khóa đảm bảo tránh xong đột khi dùng hàm GotoXY
    if (_X < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].x) {
        DrawCell(_X, _Y, 15);
        _X += 4;
        DrawCell(_X, _Y, 11);
    }
}
void MoveLeft() {
    //lock_guard<std::mutex> lock(consoleMutex); //Cập nhật dòng khóa đảm bảo tránh xong đột khi dùng hàm GotoXY
    if (_X > _A[0][0].x) {
        DrawCell(_X, _Y, 15);
        _X -= 4;
        DrawCell(_X, _Y, 11);
    }
}
void MoveDown() {
    //lock_guard<std::mutex> lock(consoleMutex); //Cập nhật dòng khóa đảm bảo tránh xong đột khi dùng hàm GotoXY
    if (_Y < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y) {
        DrawCell(_X, _Y, 15);
        _Y += 2;
        DrawCell(_X, _Y, 11);
    }
}
void MoveUp() {
    //lock_guard<std::mutex> lock(consoleMutex); //Cập nhật dòng khóa đảm bảo tránh xong đột khi dùng hàm GotoXY
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
        srand(time(NULL));
        int index = rand() % emptyCells.size();
        int r = emptyCells[index].first;
        int c = emptyCells[index].second;

        // Cập nhật tọa độ toàn cục để đồng bộ với hàm CheckBoard
        _X = _A[r][c].x;
        _Y = _A[r][c].y;

        CheckBoard(_X, _Y);
        DrawCell(_X, _Y, 11);
    }
}