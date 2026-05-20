#include "UserInfo.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include <iostream>

using namespace std;

// INFO_X = 91, INFO_Y = 24  (định nghĩa trong GameStatus.h)
// Panel nằm bên phải bàn cờ, bên dưới sprite nhân vật 2

void DrawPlayerInfo() {
    // Tọa độ bắt đầu ngay dưới bàn cờ 2 dòng
    int startY = TOP + BOARD_SIZE * 2;
    int startX = LEFT;

    SetColor(0, 15);

    // Viền trên
    GotoXY(startX, startY);

    // Khung tên người chơi (Cột 1)
    GotoXY(startX, startY + 1);
    SetColor(12, 15); cout << " [X] " << _PLAYER1_NAME;
    SetColor(0, 15);  cout << "  vs  ";
    SetColor(10, 15); cout << "[O] " << _PLAYER2_NAME;

    // Viền dưới
    GotoXY(startX, startY + 2);
    SetColor(0, 15);

    // Hướng dẫn phím siêu rút gọn
    GotoXY(startX, startY + 3);
    SetColor(8, 15); // Chữ màu xám đậm (hoặc dùng 0, 15 nếu không hỗ trợ)
    cout << " Phim: WASD(Di chuyen) | Enter(Danh) | Z/Y(Undo/Redo) | P(Pause) | M(Menu)";
    SetColor(0, 15);
}

void UpdateTurnInfo() {
    int startY = TOP + BOARD_SIZE * 2 + 3; // Nằm trên dòng Y + 1 của khung
    int turnX = LEFT + 35; // Cột thứ 2

    lock_guard<mutex> lock(consoleMutex);
    GotoXY(turnX, startY);
    SetColor(0, 15);
    cout << " |  LUOT DI: ";

    if (_TURN == true) {
        SetColor(12, 15); cout << "X   "; // Cắt bỏ tên để đỡ dài
    }
    else {
        SetColor(10, 15); cout << "O   ";
    }
    SetColor(0, 15);
    GotoXY(_X, _Y); // Trả con trỏ về bàn cờ
}