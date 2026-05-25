#include "Menu.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "bg_menu.h"
#include "DrawBackground.hpp"
#include "btn_normal.h"
#include "btn_hover.h"
#include <windows.h>

using namespace std;

// --- CÁC HÀM WRAPPER ĐỂ TEMPLATE TRONG H CÓ THỂ GỌI ĐƯỢC ---
void DrawUIBackground() {
    DrawMenuBackground();
}

void DrawTitleArtWrapper(int startX, int startY) {
    // Gọi hàm DrawTitleArt gốc bên dưới
    void DrawTitleArt(int, int);
    DrawTitleArt(startX, startY);
}

// 1. Hàm in chữ có hộp nền tối (Dark Box) để chữ không cắn rách ảnh
void PrintTextWithBg(int startX, int startY, string text, int textColorCode) {
    int tr = 255, tg = 255, tb = 255;
    if (textColorCode == 11) { tr = 0; tg = 255; tb = 255; }
    else if (textColorCode == 14) { tr = 255; tg = 255; tb = 0; }
    else if (textColorCode == 12) { tr = 255; tg = 0; tb = 0; }

    GotoXY(startX, startY);
    // \x1b[48;2;15;15;20m là màu nền đen tuyền để lót dưới chữ
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;15;15;20m", tr, tg, tb);
    cout << text;
    printf("\x1b[0m");
}

// Gradient màu xanh băng từ trên xuống (7 màu cho 6 dòng ASCII art)
static const int TITLE_GRAD[7][3] = {
    {220, 240, 255},  // trắng băng (dòng 0)
    {140, 220, 255},  // xanh nhạt  (dòng 1)
    { 80, 200, 255},  // cyan sáng  (dòng 2)
    { 60, 180, 240},  // cyan       (dòng 3)
    { 40, 150, 220},  // xanh vừa   (dòng 4)
    { 20, 120, 200},  // xanh đậm   (dòng 5)
};

static const int SHADOW_COL[3] = { 20, 60, 120 }; // Màu outline/shadow

void DrawTitleArt(int startX, int startY) {
    const char* art[] = {
        " _____   ___  ______ _____ ",
        "/  __ \\ / _ \\ | ___ \\  _  |",
        "| /  \\// /_\\ \\| |_/ / | | |",
        "|    /|  _  ||    /| | | |",
        "| \\__/\\| | | || |\\ \\\\ \\_/ /",
        " \\____/\\_| |_/\\_| \\_|\\___/ ",
    };
    int nLines = 6;

    for (int i = 0; i < nLines; i++) {
        // Vẽ shadow (lệch 1 xuống 1 phải, màu tối)
        GotoXY(startX + 1, startY + i + 1);
        printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;5;10;25m%s\x1b[0m",
            SHADOW_COL[0], SHADOW_COL[1], SHADOW_COL[2], art[i]);

        // Vẽ chữ chính với gradient
        GotoXY(startX, startY + i);
        printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;5;10;25m%s\x1b[0m",
            TITLE_GRAD[i][0], TITLE_GRAD[i][1], TITLE_GRAD[i][2], art[i]);
    }
}

// Vẫn giữ lại Menu gốc (GenericMenu) để đảm bảo không mất tính năng nào
int GenericMenu(string options[], int size, string title) {
    int currentSelect = 0;
    system("cls");
    DrawMenuBackground();

    while (true) {
        if (title == "GAME CARO") {
            DrawTitleArt(42, 3);
        }
        else {
            PrintTextWithBg(40, 5, "==============================", 11);
            int padding = (26 - title.length()) / 2;
            string titleLine = "||" + string(padding, ' ') + title + string(26 - title.length() - padding, ' ') + "||";
            PrintTextWithBg(40, 6, titleLine, 11);
            PrintTextWithBg(40, 7, "==============================", 11);
        }

        for (int i = 0; i < size; i++) {
            if (i == currentSelect) {
                string opt = ">> " + options[i] + " <<";
                PrintTextWithBg(45, 11 + i * 2, opt, 14);
            }
            else {
                string opt = "   " + options[i] + "   ";
                PrintTextWithBg(45, 11 + i * 2, opt, 11);
            }
        }

        int key = toupper(_getch());
        if (key == 'W' || key == 72) {
            currentSelect--;
            if (currentSelect < 0) currentSelect = size - 1;
        }
        else if (key == 'S' || key == 80) {
            currentSelect++;
            if (currentSelect >= size) currentSelect = 0;
        }
        else if (key == 13) {
            return currentSelect;
        }
    }
}

// --- Menu dùng Đồ Họa Cải Tiến (GraphicalMenu) ---
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
        if (c == 13 && res.length() > 0) {
            break;
        }
        else if (c == 8) {
            if (res.length() > 0) {
                res.pop_back();
                cout << "\b \b";
            }
        }
        else if ((isalnum(c)) && res.length() < 15) {
            res += c;
            cout << c;
        }
    }
    return res;
}

// --- GỘP THUẬT TOÁN NHẬP TÊN ĐỒ HỌA + QUÉT MÀU NỀN ---
void InputPlayerNames(bool isBotMode) {
    system("cls");
    DrawMenuBackground(); // Nền xịn từ Menu cũ

    int consoleW = 120;
    int frameW = 52;
    int frameX = (consoleW - frameW) / 2;
    int p1Y = 8;
    int p2Y = 14;

    // Frame Nhập Tên Player 1
    DrawFrame(frameX, p1Y, frameW, 3);
    PrintTextWithBg(frameX + 4, p1Y + 1, "Nhap ten Nguoi choi 1 (X): ", 12);

    UnhideCursor();
    GotoXY(frameX + 31, p1Y + 1);

    // Tích hợp hệ thống quét pixel nền thông minh
    int r1 = BG_MENU[(p1Y + 1) * 2][frameX + 31][0];
    int g1 = BG_MENU[(p1Y + 1) * 2][frameX + 31][1];
    int b1 = BG_MENU[(p1Y + 1) * 2][frameX + 31][2];
    printf("\x1b[38;2;255;255;0m\x1b[48;2;%d;%d;%dm", r1, g1, b1);

    _PLAYER1_NAME = TypeName();
    printf("\x1b[0m");
    HideCursor();

    // Frame Nhập Tên Player 2 (Hoặc Bot)
    if (isBotMode) {
        if (_BOT_DIFFICULTY == 1) _PLAYER2_NAME = "Bot (De)";
        else if (_BOT_DIFFICULTY == 2) _PLAYER2_NAME = "Bot (Trung Binh)";
        else _PLAYER2_NAME = "Bot (Kho)";

        DrawFrame(frameX, p2Y, frameW, 3);
        PrintTextWithBg(frameX + 4, p2Y + 1, "Ten Nguoi choi 2 (Bot): " + _PLAYER2_NAME, 11);
        Sleep(1000);
    }
    else {
        DrawFrame(frameX, p2Y, frameW, 3);

        while (true) {
            PrintTextWithBg(frameX + 4, p2Y + 1, "Nhap ten Nguoi choi 2 (O): ", 11);

            // Xóa vùng text nhập tên bên trong Frame
            GotoXY(frameX + 31, p2Y + 1);
            printf("\x1b[48;2;%d;%d;%dm", r1, g1, b1);
            cout << string(16, ' ');
            GotoXY(frameX + 31, p2Y + 1);

            UnhideCursor();
            int r2 = BG_MENU[(p2Y + 1) * 2][frameX + 31][0];
            int g2 = BG_MENU[(p2Y + 1) * 2][frameX + 31][1];
            int b2 = BG_MENU[(p2Y + 1) * 2][frameX + 31][2];
            printf("\x1b[38;2;255;255;0m\x1b[48;2;%d;%d;%dm", r2, g2, b2);

            _PLAYER2_NAME = TypeName();
            printf("\x1b[0m");
            HideCursor();

            if (_PLAYER1_NAME == _PLAYER2_NAME) {
                string errorMsg = "Ten bi trung voi Player 1! Nhan phim bat ky de nhap lai...";
                PrintTextWithBg((consoleW - errorMsg.length()) / 2, p2Y + 4, errorMsg, 12);
                _getch();

                // Tẩy xóa dòng báo lỗi
                PrintTextWithBg((consoleW - errorMsg.length()) / 2, p2Y + 4, string(errorMsg.length(), ' '), 15);
            }
            else {
                break;
            }
        }
    }
}