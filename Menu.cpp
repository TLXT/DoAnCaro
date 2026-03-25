#include "Menu.h"
#include "ControlConsole.h"

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
    string options[4] = { "1. thoat","2. Luu game","3.Tai game","4.Setting"};
    return GenericMenu(options, 4, "MENU");
}