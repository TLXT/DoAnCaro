#include "Menu.h"
#include "ControlConsole.h"
#include "GameStatus.h"

#include "btn_normal.h"
#include "btn_hover.h"

using namespace std;


int MainMenu() {
    string options[4] = { "Play Game", "Load Game", "Settings", "Exit" };
    return GraphicalMenu(options, 4, "GAME CARO", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int PlayGameMenu() {
    string options[3] = { "Player vs Player", "Player vs Bot", "Quay lai" };
    return GraphicalMenu(options, 3, "CHON CHE DO", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int DifficultyMenu() {
    string options[4] = { "De (Easy)", "Trung binh", "Kho (Hard)", "Quay lai" };
    return GraphicalMenu(options, 4, "CHON DO KHO", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int SettingsMenu() {
    string options[2] = { "Clear Data", "Quay lai" };
    return GraphicalMenu(options, 2, "CAI DAT", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int GameMenu() {
    string options[4] = { "Thoat game", "Luu game", "Tai game", "Thoat menu" };
    return GraphicalMenu(options, 4, "MENU TAM DUNG", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

string TypeName() {
    string res = "";
    while (true) {
        char c = _getch();
        if (c == 13 && res.length() > 0) { // Bấm Enter
            break;
        }
        else if (c == 8) { // Bấm Backspace (Xóa)
            if (res.length() > 0) {
                res.pop_back();
                cout << "\b \b";
            }
        }
        // Cho phép nhập chữ, số (tối đa 15 ký tự để không vỡ UI)
        else if ((isalnum(c)) && res.length() < 15) {
            res += c;
            cout << c;
        }
    }
    return res;
}

void InputPlayerNames(bool isBotMode) {
    system("cls");
    system("color F0");
    SetColor(12, 15);

    // Nhập tên Người chơi 1
    GotoXY(35, 10); cout << "Nhap ten Nguoi choi 1 (X): ";
    UnhideCursor();
    _PLAYER1_NAME = TypeName();

    if (isBotMode) {
        // Tự động đặt tên Bot theo độ khó
        if (_BOT_DIFFICULTY == 1) _PLAYER2_NAME = "Bot (De)";
        else if (_BOT_DIFFICULTY == 2) _PLAYER2_NAME = "Bot (Trung Binh)";
        else _PLAYER2_NAME = "Bot (Kho)";
    }
    else {
        // Nhập tên Người chơi 2
        GotoXY(35, 12); cout << "Nhap ten Nguoi choi 2 (O): ";
        _PLAYER2_NAME = TypeName();

        // Vòng lặp kiểm tra trùng tên
        while (_PLAYER1_NAME == _PLAYER2_NAME) {
            // Xóa dòng thông báo cũ và tên đã nhập sai
            GotoXY(35, 13); cout << "Ten trung voi Player 1! Vui long nhap lai...";
            _getch();

            // Xóa các dòng text cũ trên console để nhập lại cho sạch
            GotoXY(35, 13); cout << "                                                  ";
            GotoXY(61, 12); cout << "                                                  ";

            GotoXY(35, 12); cout << "Nhap lai ten Nguoi choi 2 (O): ";
            _PLAYER2_NAME = TypeName();
        }
    } // Kết thúc block else

    HideCursor();
}