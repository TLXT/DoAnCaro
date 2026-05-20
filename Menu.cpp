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

    int consoleW = 120;
    int frameW = 52; // Kích thước đủ chứa 27 ký tự (câu hỏi) + 15 ký tự (tên) + padding
    int frameX = (consoleW - frameW) / 2;
    int p1Y = 8;
    int p2Y = 14; // Cách Frame 1 một khoảng vừa đẹp

    // ============================================
    // Frame Nhập Tên Player 1
    // ============================================
    DrawFrame(frameX, p1Y, frameW, 3);
    GotoXY(frameX + 4, p1Y + 1);
    SetColor(12, 15); // Chữ đỏ nền trắng
    cout << "Nhap ten Nguoi choi 1 (X): ";

    UnhideCursor();
    GotoXY(frameX + 31, p1Y + 1);
    _PLAYER1_NAME = TypeName();
    HideCursor();

    // ============================================
    // Frame Nhập Tên Player 2 (Hoặc Bot)
    // ============================================
    if (isBotMode) {
        // Tự động đặt tên Bot theo độ khó
        if (_BOT_DIFFICULTY == 1) _PLAYER2_NAME = "Bot (De)";
        else if (_BOT_DIFFICULTY == 2) _PLAYER2_NAME = "Bot (Trung Binh)";
        else _PLAYER2_NAME = "Bot (Kho)";

        // Vẽ luôn Frame hiển thị tên Bot cho đồng bộ giao diện 2 khối
        DrawFrame(frameX, p2Y, frameW, 3);
        GotoXY(frameX + 4, p2Y + 1);
        SetColor(1, 15); // Chữ xanh dương nền trắng
        cout << "Ten Nguoi choi 2 (Bot): " << _PLAYER2_NAME;
        Sleep(1000); // Tạm dừng 1 giây để người chơi kịp nhìn tên Bot
    }
    else {
        DrawFrame(frameX, p2Y, frameW, 3);

        while (true) {
            GotoXY(frameX + 4, p2Y + 1);
            SetColor(1, 15);
            cout << "Nhap ten Nguoi choi 2 (O): ";

            // Xóa vùng text nhập tên bên trong Frame (quét 16 khoảng trắng)
            // Cực kỳ quan trọng để dọn dẹp phần chữ khi người chơi nhập sai và phải nhập lại
            GotoXY(frameX + 31, p2Y + 1);
            cout << string(16, ' ');

            UnhideCursor();
            GotoXY(frameX + 31, p2Y + 1);
            _PLAYER2_NAME = TypeName();
            HideCursor();

            if (_PLAYER1_NAME == _PLAYER2_NAME) {
                // Báo lỗi căn giữa, ngay bên dưới Frame Player 2
                string errorMsg = "Ten bi trung voi Player 1! Nhan phim bat ky de nhap lai...";
                GotoXY((consoleW - errorMsg.length()) / 2, p2Y + 4);
                SetColor(12, 15);
                cout << errorMsg;

                _getch(); // Chờ người dùng xem lỗi

                // Tẩy xóa dòng báo lỗi bằng các khoảng trắng để trả lại màn hình sạch sẽ
                GotoXY((consoleW - errorMsg.length()) / 2, p2Y + 4);
                cout << string(errorMsg.length(), ' ');
            }
            else {
                break; // Tên hợp lệ thì thoát vòng lặp
            }
        }
    }
}