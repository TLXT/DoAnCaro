#include "UserInfo.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include <iostream>

using namespace std;

#define LEFT 60
#define TOP 2

void DrawPlayerInfo() {
    SetColor(0, 15);
    GotoXY(LEFT, TOP);     cout << "===============================";
    GotoXY(LEFT, TOP + 1); cout << "|        THONG TIN GAME       |";
    GotoXY(LEFT, TOP + 2); cout << "===============================";

    GotoXY(LEFT, TOP + 4); cout << "Player 1 (X) : Nguoi choi 1";
    GotoXY(LEFT, TOP + 5); cout << "Player 2 (O) : Nguoi choi 2";

    GotoXY(LEFT, TOP + 9); cout << "===============================";
    GotoXY(LEFT, TOP + 10); cout << "Phim dieu khien:";
    GotoXY(LEFT, TOP + 11); cout << "W, A, S, D : Di chuyen";
    GotoXY(LEFT, TOP + 12); cout << "Enter      : Danh co";
    GotoXY(LEFT, TOP + 13); cout << "L          : Luu game (Save)";
    GotoXY(LEFT, TOP + 14); cout << "T          : Tai game (Load)";
    GotoXY(LEFT, TOP + 15); cout << "ESC        : Thoat";
    GotoXY(LEFT, TOP + 16); cout << "Z          : Undo (Hoan tac)";
    GotoXY(LEFT, TOP + 17); cout << "Y          : Redo (Lam lai)";
    GotoXY(LEFT, TOP + 18); cout << "ESC        : Thoat";
}

void UpdateTurnInfo() {
    GotoXY(LEFT, TOP + 7);
    if (_TURN) {
        SetColor(12, 15);
        cout << "=> LUOT DI: PLAYER 1 (X)   ";
    }
    else {
        SetColor(10, 15);
        cout << "=> LUOT DI: PLAYER 2 (O)   ";
    }
    SetColor(0, 15);
    GotoXY(_X, _Y);
}