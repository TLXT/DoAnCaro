#pragma once
#include <iostream>
#include <conio.h>
#include <string>
#include "CharSprite.h"

using namespace std;

int GenericMenu(string options[], int size, string title);
int MainMenu();
int PlayGameMenu();
int DifficultyMenu();
int SettingsMenu();
int GameMenu();
void InputPlayerNames(bool isBotMode);

// --- CÁC HÀM GIAO DIỆN CHUNG (Định nghĩa trực tiếp dùng Template) ---

template <size_t BtnW>
int GraphicalMenu(string options[], int size, string title,
    const int btnNormal[][BtnW], const int btnHover[][BtnW], int btnW, int btnH)
{
    system("cls");
    int consoleW = 120;

    // Vẽ khung câu hỏi/tiêu đề
    int frameW = title.length() + 12;
    int frameX = (consoleW - frameW) / 2;
    DrawFrame(frameX, 4, frameW, 5);
    GotoXY(frameX + 6, 6);
    SetColor(12, 15);
    cout << title;

    int btnCols = btnW * 2;
    int startX = (consoleW - btnCols) / 2;
    int startY_Base = 12;

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
                    GotoXY(startX + (btnCols - options[i].length()) / 2, startY + btnH / 2);
                    SetColor(0, bgHover); // Chữ đen, nền theo màu nút
                    cout << options[i];
                }
                else {
                    DrawSolidImage(btnNormal, btnW, btnH, startX, startY);
                    GotoXY(startX + (btnCols - options[i].length()) / 2, startY + btnH / 2);
                    SetColor(0, bgNormal);
                    cout << options[i];
                }
            }
            lastSelect = currentSelect;
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
        else if (key == 13) return currentSelect;
    }
}

template <size_t BtnW>
bool GraphicalYesNo(string prompt, int startY, bool clearScreen,
    const int btnNormal[][BtnW], const int btnHover[][BtnW], int btnW, int btnH)
{
    int consoleW = 120;
    int consoleH = 40; // Giới hạn chiều cao tuyệt đối của console

    // --- BẢO VỆ CHỐNG TRÀN MÀN HÌNH (Fix lỗi xéo khung) ---
    // Tính toán dòng sâu nhất mà menu sẽ vẽ tới. 
    // Nếu nó lớn hơn hoặc bằng 40, ép nó dịch lên trên để an toàn.
    if (startY + 7 + btnH >= consoleH) {
        startY = consoleH - 8 - btnH;
    }

    if (clearScreen) {
        system("cls");
    }
    else {
        // Tẩy trắng khu vực sẽ chứa Menu để đè gọn gàng lên bàn cờ cũ
        SetColor(0, 15);
        for (int i = 0; i <= 7 + btnH; i++) {
            GotoXY(4, startY + i);
            cout << string(112, ' '); // Xóa khoảng trắng ở giữa, chừa lề 2 bên
        }
    }

    int frameW = prompt.length() + 12;
    int frameX = (consoleW - frameW) / 2;

    DrawFrame(frameX, startY, frameW, 5); // Khung này sẽ tự xóa rác bên trong
    GotoXY(frameX + 6, startY + 2);
    SetColor(12, 15);
    cout << prompt;

    int btnCols = btnW * 2;
    int totalBtnW = btnCols * 2 + 10;
    int startX = (consoleW - totalBtnW) / 2;
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
        char ch = _getch();
        if (ch == -32 || ch == 0) ch = _getch();

        if (ch == 75 || ch == 'a' || ch == 'A') choice = 0;
        else if (ch == 77 || ch == 'd' || ch == 'D') choice = 1;
        else if (ch == 13) {
            SetColor(0, 15);
            return (choice == 0);
        }
    }
}