#include"Character.h"
#include "GameStatus.h"
#include"ControlConsole.h"
#include "bg_game.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>

int CharacterASelect = -1;
int CharacterBSelect = -1;

static bool IsCharacterPixel(int color) {
    return color >= 0 && color < 15;
}

static void RestoreGameBackgroundRect(int startX, int startY, int width, int height) {
    char pixel[64];

    for (int row = 0; row < height; row++) {
        int consoleY = startY + row;
        GotoXY(startX, consoleY);

        string rowBuf;
        for (int col = 0; col < width; col++) {
            int consoleX = startX + col;
            int bgY = consoleY * 2;

            if (consoleX >= 0 && consoleX < BG_GAME_W && bgY >= 0 && bgY + 1 < BG_GAME_H) {
                int rt = BG_GAME[bgY][consoleX][0];
                int gt = BG_GAME[bgY][consoleX][1];
                int bt = BG_GAME[bgY][consoleX][2];
                int rb = BG_GAME[bgY + 1][consoleX][0];
                int gb = BG_GAME[bgY + 1][consoleX][1];
                int bb = BG_GAME[bgY + 1][consoleX][2];
                snprintf(pixel, sizeof(pixel), "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm\xE2\x96\x80",
                    rt, gt, bt, rb, gb, bb);
                rowBuf += pixel;
            }
            else {
                rowBuf += ' ';
            }
        }
        cout << rowBuf;
    }
    cout << "\x1b[0m";
}

static void DrawCharacterPixels(const vector<vector<int>>& pixels, int startX, int startY, int cellWidth) {
    for (int row = 0; row < (int)pixels.size(); row++) {
        int currentX = startX;
        for (int color : pixels[row]) {
            if (color < 0 || color >= 15) {
                currentX += cellWidth;
            } else {
                GotoXY(currentX, startY + row);
                SetColor(color, color);
                for (int i = 0; i < cellWidth; i++) cout << ' ';
                currentX += cellWidth;
            }
        }
    }
    SetColor(0, 15);
}

static void DrawCharacterPixelsScaled(const vector<vector<int>>& pixels, int startX, int startY, int targetW, int targetH, bool flipHorizontal) {
    int top = (int)pixels.size();
    int bottom = -1;
    int left = 100000;
    int right = -1;

    for (int row = 0; row < (int)pixels.size(); row++) {
        for (int col = 0; col < (int)pixels[row].size(); col++) {
            if (IsCharacterPixel(pixels[row][col])) {
                top = min(top, row);
                bottom = max(bottom, row);
                left = min(left, col);
                right = max(right, col);
            }
        }
    }

    if (bottom < top || right < left) return;

    int srcW = right - left + 1;
    int srcH = bottom - top + 1;

    for (int row = 0; row < targetH; row++) {
        int srcRow = top + (row * srcH) / targetH;
        if (srcRow > bottom) srcRow = bottom;

        for (int col = 0; col < targetW; col++) {
            int srcOffset = (col * srcW) / targetW;
            int srcCol = flipHorizontal ? right - srcOffset : left + srcOffset;
            int color = 15;
            if (srcCol >= 0 && srcCol < (int)pixels[srcRow].size()) {
                color = pixels[srcRow][srcCol];
            }

            if (IsCharacterPixel(color)) {
                GotoXY(startX + col, startY + row);
                SetColor(color, color);
                cout << ' ';
            }
        }
    }
    SetColor(0, 15);
}

static void DrawCharacterName(const string& text, int startX, int startY, int width) {
    string label = text;
    if ((int)label.length() > width - 2) {
        label = label.substr(0, max(0, width - 5)) + "...";
    }

    int labelW = min(width, (int)label.length() + 2);
    int offset = (width - labelW) / 2;
    if (offset < 0) offset = 0;
    int labelX = startX + offset;
    GotoXY(startX, startY);
    printf("\x1b[38;2;235;240;245m\x1b[48;2;25;25;30m");
    cout << string(width, ' ');
    GotoXY(labelX, startY);
    cout << " " << label << " ";
    printf("\x1b[0m");
}

static string UpperAscii(string text) {
    for (char& ch : text) {
        ch = (char)toupper((unsigned char)ch);
    }
    return text;
}

static void FillConsoleRect(int x, int y, int w, int h, int bgColor) {
    SetColor(0, bgColor);
    string row(w, ' ');
    for (int i = 0; i < h; i++) {
        GotoXY(x, y + i);
        cout << row;
    }
    SetColor(0, 15);
}

static void DrawPreviewFrame(int x, int y, int w, int h, bool selected, bool disabled) {
    int borderColor = disabled ? 7 : (selected ? 14 : 11);
    int bgColor = disabled ? 8 : (selected ? 3 : 1);
    char horizontal = selected ? '=' : '-';

    FillConsoleRect(x, y, w, h, bgColor);
    SetColor(borderColor, bgColor);
    GotoXY(x, y);
    cout << "+";
    for (int i = 0; i < w - 2; i++) cout << horizontal;
    cout << "+";

    for (int row = 1; row < h - 1; row++) {
        GotoXY(x, y + row);
        cout << "|";
        GotoXY(x + w - 1, y + row);
        cout << "|";
    }

    GotoXY(x, y + h - 1);
    cout << "+";
    for (int i = 0; i < w - 2; i++) cout << horizontal;
    cout << "+";
    SetColor(0, 15);
}

static void DrawCharacterPreviewBox(Character& character, int x, int y, int boxW, int boxH, bool selected, bool disabled) {
    DrawPreviewFrame(x, y, boxW, boxH, selected, disabled);

    int spriteW = min(20, boxW - 6);
    int spriteH = min(10, boxH - 5);
    int spriteX = x + (boxW - spriteW) / 2;
    int spriteY = y + 2;
    DrawCharacterPixelsScaled(character.GetDisplay(), spriteX, spriteY, spriteW, spriteH, false);

    string label = disabled ? "DA CHON" : UpperAscii(character.GetName());
    if (selected && !disabled) label = "> " + label + " <";
    DrawCharacterName(label, x + 1, y + boxH - 2, boxW - 2);
}

void DrawCharacterPreview(int option, int x, int y, int boxW, int boxH, bool selected, bool disabled) {
    if (option == 0) {
        Knight character;
        DrawCharacterPreviewBox(character, x, y, boxW, boxH, selected, disabled);
    }
    else if (option == 1) {
        Assassin character;
        DrawCharacterPreviewBox(character, x, y, boxW, boxH, selected, disabled);
    }
    else if (option == 2) {
        Vampire character;
        DrawCharacterPreviewBox(character, x, y, boxW, boxH, selected, disabled);
    }
    else if (option == 3) {
        Paladin character;
        DrawCharacterPreviewBox(character, x, y, boxW, boxH, selected, disabled);
    }
    else {
        Officer character;
        DrawCharacterPreviewBox(character, x, y, boxW, boxH, selected, disabled);
    }
}

void outsidedisplay(int option) {
    if (option == 0) {
        Knight a;
        a.DrawOnChoosingMenu();
    }
    else if (option == 1) {
        Assassin a;
        a.DrawOnChoosingMenu();
    }
    else if (option == 2) {
        Vampire a;
        a.DrawOnChoosingMenu();
    }
    else if (option == 3) {
        Paladin a;
        a.DrawOnChoosingMenu();
    }
    else if (option == 4) {
        Officer a;
        a.DrawOnChoosingMenu();
    }
}

void ingamedisplay(int option, bool isPlayer1) {
    string name = isPlayer1 ? _PLAYER1_NAME : _PLAYER2_NAME;

    if (option == 0) {
        Knight character(name);
        isPlayer1 ? character.DrawLeftSizeInGame() : character.DrawRightSizeInGame();
    }
    else if (option == 1) {
        Assassin character(name);
        isPlayer1 ? character.DrawLeftSizeInGame() : character.DrawRightSizeInGame();
    }
    else if (option == 2) {
        Vampire character(name);
        isPlayer1 ? character.DrawLeftSizeInGame() : character.DrawRightSizeInGame();
    }
    else if (option == 3) {
        Paladin character(name);
        isPlayer1 ? character.DrawLeftSizeInGame() : character.DrawRightSizeInGame();
    }
    else {
        Officer character(name);
        isPlayer1 ? character.DrawLeftSizeInGame() : character.DrawRightSizeInGame();
    }
}

void DrawBothSprites() {
    if (CharacterASelect < 0) CharacterASelect = 0;
    if (CharacterBSelect < 0) CharacterBSelect = 1;
    ingamedisplay(CharacterASelect, true);
    ingamedisplay(CharacterBSelect, false);
}

void Character::DrawOnChoosingMenu() {
    DrawCharacterPixels(GetDisplay(), 18, 5, 2);
    DrawCharacterName(GetName(), 18, 26, 40);
}
void Character::DrawLeftSizeInGame() {
    RestoreGameBackgroundRect(LEFT_SPRITE_X, SPRITE_START_Y, SPRITE_DRAW_W, SPRITE_DRAW_H + 2);
    DrawCharacterPixelsScaled(GetDisplay(), LEFT_SPRITE_X, SPRITE_START_Y, SPRITE_DRAW_W, SPRITE_DRAW_H, false);
    DrawCharacterName(GetName(), LEFT_SPRITE_X, SPRITE_START_Y + SPRITE_DRAW_H + 1, SPRITE_DRAW_W);
}
void Character::DrawRightSizeInGame() {
    RestoreGameBackgroundRect(RIGHT_SPRITE_X, SPRITE_START_Y, SPRITE_DRAW_W, SPRITE_DRAW_H + 2);
    DrawCharacterPixelsScaled(GetDisplay(), RIGHT_SPRITE_X, SPRITE_START_Y, SPRITE_DRAW_W, SPRITE_DRAW_H, true);
    DrawCharacterName(GetName(), RIGHT_SPRITE_X, SPRITE_START_Y + SPRITE_DRAW_H + 1, SPRITE_DRAW_W);
}
