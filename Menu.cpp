#include "Menu.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "character.h"
using namespace std;

int GenericMenu(string options[], int size, string title) {
    int currentSelect = 0;

    while (true) {
        system("cls");
        system("color F0");

        SetColor(12, 15);
        GotoXY(40, 5); cout << "==============================";
        GotoXY(40, 6);
        int padding = (26 - title.length()) / 2;
        cout << "||" << string(padding, ' ') << title << string(26 - title.length() - padding, ' ') << "||";
        GotoXY(40, 7); cout << "==============================";

        for (int i = 0; i < size; i++) {
            if (i == currentSelect) {
                SetColor(0, 11); // Nền Cyan
                GotoXY(45, 11 + i * 2);
                cout << ">> " << options[i] << " <<";
            }
            else {
                SetColor(0, 15); // Nền trắng
                GotoXY(45, 11 + i * 2);
                cout << "   " << options[i] << "   ";
            }
        }
        SetColor(0, 15);

        int key = toupper(_getch());
        if (key == 'W' || key == 72) {
            currentSelect--;
            if (currentSelect < 0) currentSelect = size - 1;
        }
        else if (key == 'S' || key == 80) {
            currentSelect++;
            if (currentSelect >= size) currentSelect = 0;
        }
        else if (key == 13) { // Phím Enter
            return currentSelect;
        }
    }
}

int MainMenu() {
    string options[4] = { "1. Play Game", "2. Load Game", "3. Settings", "4. Exit" };
    return GenericMenu(options, 4, "GAME CARO");
}

int PlayGameMenu() {
    string options[3] = { "1. Player vs Player", "2. Player vs Bot", "3. Quay lai" };
    return GenericMenu(options, 3, "CHON CHE DO");
}

int DifficultyMenu() {
    string options[4] = { "1. De (Easy)", "2. Trung binh (Medium)", "3. Kho (Hard)", "4. Quay lai" };
    return GenericMenu(options, 4, "CHON DO KHO");
}

int SettingsMenu() {
    string options[2] = { "1. Clear Data (Xoa toan bo Save)", "2. Quay lai" };
    return GenericMenu(options, 2, "CAI DAT");
}
int GameMenu() {
    string options[4] = { "1. Thoat game","2. Luu game","3. Tai game","4. Thoat menu"};
    return GenericMenu(options, 4, "MENU");
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

    // --- NHẬP CHO NGƯỜI CHƠI 1 ---
    GotoXY(35, 10); cout << "Nhap ten Nguoi choi 1 (X): ";
    UnhideCursor();
    _PLAYER1_NAME = TypeName();
    HideCursor();
    
    // Gọi menu chọn nhân vật cho Player 1
    // Hàm này sẽ trả về chỉ số 0-4 tương ứng với Knight, Assassin, v.v.
    CharacterASelect = CharacterSelectionMenu(); 

    if (isBotMode) {
        // --- CHẾ ĐỘ VỚI BOT ---
        if (_BOT_DIFFICULTY == 1) _PLAYER2_NAME = "Bot (De)";
        else if (_BOT_DIFFICULTY == 2) _PLAYER2_NAME = "Bot (Trung Binh)";
        else _PLAYER2_NAME = "Bot (Kho)";

        // Bot có thể mặc định là nhân vật cuối cùng hoặc ngẫu nhiên
        CharacterBSelect = 4; // Ví dụ mặc định Bot là Officer
    }
    else {
        // --- CHẾ ĐỘ PVP ---
        system("cls"); // Xóa màn hình để nhập người chơi 2
        SetColor(12, 15);
        GotoXY(35, 12); cout << "Nhap ten Nguoi choi 2 (O): ";
        UnhideCursor();
        _PLAYER2_NAME = TypeName();

        // Kiểm tra trùng tên
        while (_PLAYER1_NAME == _PLAYER2_NAME) {
            GotoXY(35, 13); cout << "Ten trung voi Player 1! Vui long nhap lai...";
            _getch();
            GotoXY(35, 13); cout << "                                                 ";
            GotoXY(61, 12); cout << "                                                 ";
            GotoXY(35, 12); cout << "Nhap lai ten Nguoi choi 2 (O): ";
            _PLAYER2_NAME = TypeName();
        }
        HideCursor();

        // Gọi menu chọn nhân vật cho Player 2
        CharacterBSelect = CharacterSelectionMenu();
    }
}

int GenericCharacterMenu(string options[], int size, string title) {
    int currentSelect = 0;
    int marginX = 80;

    while (true) {
        system("cls");
        system("color F0");

        // Vẽ nhân vật trước khi đợi nhập phím
        outsidedisplay(currentSelect);

        // Vẽ Menu ở phía bên phải
        SetColor(12, 15);
        GotoXY(marginX, 5); cout << "==============================";
        GotoXY(marginX, 6);
        int padding = (26 - title.length()) / 2;
        cout << "||" << string(padding, ' ') << title << string(26 - title.length() - padding, ' ') << "||";
        GotoXY(marginX, 7); cout << "==============================";

        for (int i = 0; i < size; i++) {
            if (i == currentSelect) {
                SetColor(0, 11); // Nền Cyan cho mục đang chọn
                GotoXY(marginX + 5, 11 + i * 2);
                cout << ">> " << options[i] << " <<";
            }
            else {
                SetColor(0, 15); // Nền trắng cho các mục khác
                GotoXY(marginX + 5, 11 + i * 2);
                cout << "   " << options[i] << "   ";
            }
        }
        SetColor(0, 15);

        // Đợi người dùng nhấn phím
        int key = toupper(_getch());

        // Xử lý logic phím bấm
        if (key == 'W' || key == 72) {
            currentSelect--;
            if (currentSelect < 0) currentSelect = size - 1;
        }
        else if (key == 'S' || key == 80) {
            currentSelect++;
            if (currentSelect >= size) currentSelect = 0;
        }
        else if (key == 13) { // Phím Enter
            return currentSelect;
        }
    }
}

int CharacterSelectionMenu() {
    string options[5] = { "1. Knight","2. Assassin","3. Vampire ","4. Paladin" ,"5. Officer"};
    return GenericCharacterMenu(options, 5, "CHARACTER MENU");
}