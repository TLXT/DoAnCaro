#pragma once
#pragma comment(lib, "winmm.lib")
#include <iostream>
#include <conio.h>
#include <string>
#include <algorithm>
#include "Character.h"
#include "ControlConsole.h" 
#include "GameStatus.h"
#include "Sound.h"

using namespace std;

// --- Khai báo các hàm liên kết (Wrappers) để dùng đồ họa trong Template ---
void DrawUIBackground();
void DrawTitleArtWrapper(int startX, int startY);
void DrawMenuTitle(const string& title, int y, int consoleW);

// --- Khai báo các hàm Menu chính ---
int GenericMenu(string options[], int size, string title);
int GenericCharacterMenu(string options[], int size, string title);
int MainMenu();
int PlayGameMenu();
int DifficultyMenu();
int SettingsMenu();
int GameMenu();
int MusicMenu();
int CharacterSelectionMenu();
void VolumeMenu();
void InputPlayerNames(bool isBotMode);

template <size_t W>
void DrawSolidImage(const int sprite[][W], int w, int h, int startX, int startY) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int colorIdx = sprite[y][x];
            if (colorIdx >= 0 && colorIdx < 15) {
                GotoXY(startX + x * 2, startY + y);
                SetColor(colorIdx, colorIdx);
                cout << "  ";
            }
        }
    }
    SetColor(0, 15);
}

void DrawFrame(int x, int y, int w, int h);

inline int ReadMenuKey() {
    int key = _getch();
    if (key == 0 || key == 224) {
        key = _getch();
    }
    else {
        key = toupper((unsigned char)key);
    }
    return key;
}

inline int CenterConsoleX(int width, int consoleW = CONSOLE_COLS) {
    int x = (consoleW - width) / 2;
    return x < 0 ? 0 : x;
}

// --- CÁC HÀM GIAO DIỆN CHUNG (Định nghĩa trực tiếp dùng Template) ---

template <size_t BtnW>
int GraphicalMenu(string options[], int size, string title,
    const int btnNormal[][BtnW], const int btnHover[][BtnW], int btnW, int btnH)
{
    system("cls");

    // [Merged Feature]: Vẽ Background đồ họa xịn xò từ Menu gốc
    DrawUIBackground();

    int consoleW = CONSOLE_COLS;

    if (title == "GAME CARO") {
        DrawMenuTitle("CARO", 1, consoleW);
    }
    else {
        DrawMenuTitle(title, 2, consoleW);
    }

    int btnCols = btnW * 2;
    int startX = CenterConsoleX(btnCols, consoleW);
    int totalMenuH = size * btnH + (size - 1);
    int startY_Base = (title == "GAME CARO") ? 10 : 12;
    if (startY_Base + totalMenuH >= 40) {
        startY_Base = 39 - totalMenuH;
    }

    int currentSelect = 0;
    int lastSelect = -1;

    // Lấy màu nền ở giữa nút để đổ màu cho chữ
    int bgNormal = btnNormal[btnH / 2][btnW / 2];
    int bgHover = btnHover[btnH / 2][btnW / 2];

    while (true) {
        if (currentSelect != lastSelect) {
            for (int i = 0; i < size; i++) {
                int startY = startY_Base + i * (btnH + 1);

                if (i == currentSelect) {
                    DrawSolidImage(btnHover, btnW, btnH, startX, startY);
                    int textLen = static_cast<int>(options[i].length());
                    GotoXY(startX + (btnCols - textLen) / 2, startY + btnH / 2);
                    SetColor(0, bgHover); // Chữ đen, nền theo màu nút
                    cout << options[i];
                }
                else {
                    DrawSolidImage(btnNormal, btnW, btnH, startX, startY);
                    int textLen = static_cast<int>(options[i].length());
                    GotoXY(startX + (btnCols - textLen) / 2, startY + btnH / 2);
                    SetColor(0, bgNormal);
                    cout << options[i];
                }
            }
            lastSelect = currentSelect;
        }

        SetColor(0, 15);
        int key = ReadMenuKey();
        if (key == 'W' || key == 72) {
            currentSelect--;
            if (currentSelect < 0) currentSelect = size - 1;
            PlayMenuSound();
        }
        else if (key == 'S' || key == 80) {
            currentSelect++;
            if (currentSelect >= size) currentSelect = 0;
            PlayMenuSound();
        }
        else if (key == 13) {
            PlayMenuSound();
            return currentSelect;
        }
    }
}

template <size_t BtnW>
bool GraphicalYesNo(string prompt, int startY, bool clearScreen,
    const int btnNormal[][BtnW], const int btnHover[][BtnW], int btnW, int btnH)
{
    int consoleW = CONSOLE_COLS;
    int consoleH = CONSOLE_LINES;

    // --- BẢO VỆ CHỐNG TRÀN MÀN HÌNH (Fix lỗi xéo khung) ---
    if (startY + 7 + btnH >= consoleH) {
        startY = consoleH - 8 - btnH;
    }

    if (clearScreen) {
        system("cls");
        DrawUIBackground(); // Vẽ lại Background nếu có xóa màn hình
    }
    else {
        // Tẩy trắng khu vực sẽ chứa Menu để đè gọn gàng lên bàn cờ cũ
        SetColor(0, 15);
        for (int i = 0; i <= 7 + btnH; i++) {
            GotoXY(0, startY + i);
            cout << string(CONSOLE_COLS, ' ');
        }
    }

    int frameW = static_cast<int>(prompt.length()) + 12;
    int frameX = CenterConsoleX(frameW, consoleW);

    DrawFrame(frameX, startY, frameW, 5); // Khung này sẽ tự xóa rác bên trong
    GotoXY(frameX + 6, startY + 2);
    SetColor(12, 15);
    cout << prompt;

    int btnCols = btnW * 2;
    int totalBtnW = btnCols * 2 + 10;
    int startX = CenterConsoleX(totalBtnW, consoleW);
    int btnY = startY + 7;

    int bgNormal = btnNormal[btnH / 2][btnW / 2];
    int bgHover = btnHover[btnH / 2][btnW / 2];

    int choice = 0;
    int lastChoice = -1;

    while (true) {
        if (choice != lastChoice) {
            // Nút YES
            int yesX = startX;
            if (choice == 0) {
                DrawSolidImage(btnHover, btnW, btnH, yesX, btnY);
                GotoXY(yesX + (btnCols - 3) / 2, btnY + btnH / 2);
                SetColor(0, bgHover); cout << "YES";
            }
            else {
                DrawSolidImage(btnNormal, btnW, btnH, yesX, btnY);
                GotoXY(yesX + (btnCols - 3) / 2, btnY + btnH / 2);
                SetColor(0, bgNormal); cout << "YES";
            }

            // Nút NO
            int noX = startX + btnCols + 10;
            if (choice == 1) {
                DrawSolidImage(btnHover, btnW, btnH, noX, btnY);
                GotoXY(noX + (btnCols - 2) / 2, btnY + btnH / 2);
                SetColor(0, bgHover); cout << "NO";
            }
            else {
                DrawSolidImage(btnNormal, btnW, btnH, noX, btnY);
                GotoXY(noX + (btnCols - 2) / 2, btnY + btnH / 2);
                SetColor(0, bgNormal); cout << "NO";
            }
            lastChoice = choice;
        }

        SetColor(0, 15);
        int ch = ReadMenuKey();

        int oldChoice = choice;
        if (ch == 75 || ch == 'A') choice = 0;
        else if (ch == 77 || ch == 'D') choice = 1;
        else if (ch == 13) {
            PlayMenuSound();
            SetColor(0, 15);
            return (choice == 0);
        }
        if (choice != oldChoice) PlayMenuSound();
    }
}

void PrintTextWithBg(int startX, int startY, string text, int textColorCode);
