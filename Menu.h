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
#include "Language.h"

using namespace std;


void DrawUIBackground();
void DrawTitleArtWrapper(int startX, int startY);
void DrawMenuTitle(const string& title, int y, int consoleW);


int GenericMenu(string options[], int size, string title);
int GenericCharacterMenu(string options[], int size, string title, int disabledOption = -1);
int MainMenu();
int PlayGameMenu();
int DifficultyMenu();
int SettingsMenu();
int GameMenu();
int MusicMenu();
int CharacterSelectionMenu(int disabledOption = -1, string chooserTitle = "");
void ShowGuideScreen();
void ShowAboutGameScreen();
void VolumeMenu();
bool InputPlayerNames(bool isBotMode);

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

inline int TextDisplayWidth(const string& text) {
    int width = 0;
    for (unsigned char ch : text) {
        if ((ch & 0xC0) != 0x80) width++;
    }
    return width;
}


template <size_t BtnW>
int GraphicalMenu(string options[], int size, string title,
    const int btnNormal[][BtnW], const int btnHover[][BtnW], int btnW, int btnH)
{
    system("cls");


    DrawUIBackground();

    int consoleW = CONSOLE_COLS;
    bool hideTitle = title.empty() || title == "MENU TAM DUNG" || title == "CHARACTER MENU";

    if (title == "GAME CARO") {
        DrawMenuTitle("CARO", 1, consoleW);
    }
    else if (!hideTitle) {
        DrawMenuTitle(title, 2, consoleW);
    }

    int btnCols = btnW * 2;
    int startX = CenterConsoleX(btnCols, consoleW);
    int totalMenuH = size * btnH + (size - 1);
    int startY_Base = 12;
    if (title == "GAME CARO") {
        startY_Base = 10;
    }
    else if (title == "MENU TAM DUNG") {
        startY_Base = 9;
    }
    else if (!hideTitle) {
        startY_Base = 10;
    }
    else if (hideTitle) {
        startY_Base = (CONSOLE_LINES - totalMenuH) / 2;
    }
    if (startY_Base + totalMenuH >= 40) {
        startY_Base = 39 - totalMenuH;
    }

    int currentSelect = 0;
    int lastSelect = -1;


    int bgNormal = btnNormal[btnH / 2][btnW / 2];
    int bgHover = btnHover[btnH / 2][btnW / 2];

    while (true) {
        if (currentSelect != lastSelect) {
            auto drawMenuItem = [&](int i, bool selected) {
                int startY = startY_Base + i * (btnH + 1);

                if (selected) {
                    DrawSolidImage(btnHover, btnW, btnH, startX, startY);
                    int textLen = TextDisplayWidth(options[i]);
                    GotoXY(startX + (btnCols - textLen) / 2, startY + btnH / 2);
                    SetColor(0, bgHover);
                    cout << options[i];
                }
                else {
                    DrawSolidImage(btnNormal, btnW, btnH, startX, startY);
                    int textLen = TextDisplayWidth(options[i]);
                    GotoXY(startX + (btnCols - textLen) / 2, startY + btnH / 2);
                    SetColor(0, bgNormal);
                    cout << options[i];
                }
            };

            if (lastSelect == -1) {
                for (int i = 0; i < size; i++) {
                    drawMenuItem(i, i == currentSelect);
                }
            }
            else {
                drawMenuItem(lastSelect, false);
                drawMenuItem(currentSelect, true);
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


    if (startY + 7 + btnH >= consoleH) {
        startY = consoleH - 8 - btnH;
    }

    if (clearScreen) {
        system("cls");
        DrawUIBackground();
    }
    else {

        SetColor(0, 15);
        for (int i = 0; i <= 7 + btnH; i++) {
            GotoXY(0, startY + i);
            cout << string(CONSOLE_COLS, ' ');
        }
    }

    int frameW = min(consoleW - 4, TextDisplayWidth(prompt) + 12);
    int frameX = CenterConsoleX(frameW, consoleW);

    DrawFrame(frameX, startY, frameW, 5);
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

            int yesX = startX;
            string yesText = L(Yes);
            string noText = L(No);
            if (choice == 0) {
                DrawSolidImage(btnHover, btnW, btnH, yesX, btnY);
                GotoXY(yesX + (btnCols - TextDisplayWidth(yesText)) / 2, btnY + btnH / 2);
                SetColor(0, bgHover); cout << yesText;
            }
            else {
                DrawSolidImage(btnNormal, btnW, btnH, yesX, btnY);
                GotoXY(yesX + (btnCols - TextDisplayWidth(yesText)) / 2, btnY + btnH / 2);
                SetColor(0, bgNormal); cout << yesText;
            }


            int noX = startX + btnCols + 10;
            if (choice == 1) {
                DrawSolidImage(btnHover, btnW, btnH, noX, btnY);
                GotoXY(noX + (btnCols - TextDisplayWidth(noText)) / 2, btnY + btnH / 2);
                SetColor(0, bgHover); cout << noText;
            }
            else {
                DrawSolidImage(btnNormal, btnW, btnH, noX, btnY);
                GotoXY(noX + (btnCols - TextDisplayWidth(noText)) / 2, btnY + btnH / 2);
                SetColor(0, bgNormal); cout << noText;
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
