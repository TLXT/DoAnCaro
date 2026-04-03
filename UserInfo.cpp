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


    GotoXY(LEFT, TOP + 4); cout << "Player 1 (X) : " << _PLAYER1_NAME << "               ";
    GotoXY(LEFT, TOP + 5); cout << "Player 2 (O) : " << _PLAYER2_NAME << "               ";

    GotoXY(LEFT, TOP + 9); cout << "===============================";
    GotoXY(LEFT, TOP + 10); cout << "Phim dieu khien:";
    GotoXY(LEFT, TOP + 11); cout << "W, A, S, D : Di chuyen";
    GotoXY(LEFT, TOP + 12); cout << "Enter      : Danh co";
    GotoXY(LEFT, TOP + 13); cout << "M/Esc      : Mo menu game";
    GotoXY(LEFT, TOP + 14); cout << "P          : Tam dung/Tiep tuc (Pause/Play)";
    GotoXY(LEFT, TOP + 15); cout << "Z          : Undo (Hoan tac)";
    GotoXY(LEFT, TOP + 16); cout << "Y          : Redo (Lam lai)";

}

void UpdateTurnInfo() {
    GotoXY(LEFT, TOP + 7);
    if (_TURN) {
        SetColor(12, 15);
        cout << "=> LUOT DI: " << _PLAYER1_NAME << " (X)              ";
    }
    else {
        SetColor(10, 15);
        cout << "=> LUOT DI: " << _PLAYER2_NAME << " (O)              ";
    }
    SetColor(0, 15);
    GotoXY(_X, _Y);
}