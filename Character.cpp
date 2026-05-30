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

// Du lieu pixel cua 5 nhan vat.
static const vector<vector<int>> CHARACTER_PIXELS_0 = {
			{15, 15, 15, 15, 15, 15, 15,  7,  0,  0,  0,  0,  0,  0,  0,  0,  7, 15, 15, 15},
			{15, 15, 15, 15, 15, 15,  7,  0,  8,  8,  8,  8,  8,  8,  8,  0,  0,  7, 15, 15},
			{15, 15, 15, 15, 15,  7,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  7, 15},
			{15, 15, 15, 15, 7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 15},
			{15, 15, 15, 15, 7,  8,  8,  8,  8,  6,  6,  6,  6,  6,  6,  6,  6,  6,  8,  7},
			{15, 15, 15, 15, 7,  8,  8,  8,  8,  8,  6,  6,  8,  8,  8,  8,  8,  6,  8,  7},
			{15, 15, 15, 15, 7,  8,  8,  8,  8, 14,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7},
			{15, 15, 15, 15, 7,  8,  8,  8,  8, 14,  8,  8,  0,  8,  8,  8,  0,  8,  6,  7},
			{15, 15, 15, 15, 7,  8,  8,  8,  8, 14, 14, 14,  6,  8,  8,  8,  6,  6, 14,  7},
			{15, 15, 15, 15, 15,  8,  8,  8,  8,  8,  8,  8, 14,  7,  7,  7, 14, 14,  6,  7},
			{15, 15, 15, 15, 15,  8,  8,  8,  8,  8,  8,  8, 14,  7,  7,  7, 14, 14,  8,  7},
			{15, 15, 15, 15,  8,  8,  8,  8,  8,  8,  8,  8, 14,  14,  14, 14, 14, 14,  8,  7},
			{15, 15, 15,  7,  4,  4,  8,  8,  8,  8,  8,  6,  6,  6,  6,  6,  6,  6,  8, 15},
			{15, 15,  7,  4, 12,  4,  4, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  4,  8, 15},
			{15, 15,  7,  4, 12,  4,  4,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  4,  7, 15},
			{15, 15,  7,  4, 12,  4,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0, 7, 15},
			{15, 15,  7,  4, 12,  4,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0, 7, 15},
			{15,  7,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 7, 15},
			{ 7,  4,  4,  4,  4,  4,  7,  8,  0,  0,  8,  8,  8,  8,  8,  0,  0,  7, 15, 15},
			{ 7,  4,  4,  4, 15,  7, 15, 15,  7,  0, 15, 15, 15, 15, 15,  7,  0, 15, 15, 15}
		};

static const vector<vector<int>> CHARACTER_PIXELS_1 = {
		{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15,  0,  0,  0,  0,  0,  0,  0,  0, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0, 15, 15, 15, 15, 15},
		{15, 15, 15, 15,  0,  0,  0,  0,  0,  0,  0,  4,  4,  0,  0,  0, 15, 15, 15, 15},
		{15, 15, 15, 15,  0,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0, 15, 15, 15, 15},
		{15, 15, 15, 15, 15,  0,  0,  0,  0,  4,  4,  4,  4,  4,  4,  0, 15, 15, 15, 15},
		{15, 15, 15, 15, 15,  0,  0,  0,  4,  4,  6,  0,  0,  6,  4,  0, 15, 15, 15, 15},
		{15, 15, 15,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  4, 15, 15, 15, 15},
		{15, 15, 15,  0,  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, 15, 15, 15, 15, 15},
		{15, 15, 15,  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, 15, 15, 15, 15, 15},
		{15, 15, 15,  0,  4,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15,  0,  0,  0,  0,  0,  4,  0,  0, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15, 15,  0,  0,  0,  0,  4,  0,  0, 15, 15, 15, 15, 15, 15},
		{15, 15, 15, 15, 15, 15, 15,  0, 15, 15, 15, 15,  0, 15, 15, 15, 15, 15, 15, 15}
		};

static const vector<vector<int>> CHARACTER_PIXELS_2 = {
	{15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0,  0,  0, 15, 15, 15, 15, 15, 15},
	{15,  0, 15, 15, 15, 15, 15, 15,  0,  0,  0,  0,  0,  0,  0,  0, 15,  0,  0, 15},
	{15,  0,  0,  0, 15, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 15},
	{15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 15},
	{15,  0,  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  8,  8,  0,  0,  0, 15},
	{15, 15,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  0,  8,  8,  8,  8,  0,  0, 15},
	{15, 15,  0,  0,  0,  0,  8,  8,  0,  0,  0,  8,  8,  8,  0,  0,  8,  0,  0, 15},
	{15, 15, 15,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0, 15},
	{15,  4,  4,  0,  0,  0,  8,  8,  8,  0,  8,  8,  8,  8,  8,  0,  8,  0,  0,  0},
	{15,  4,  4,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,  4,  0},
	{15, 15,  4,  4,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  4,  4,  0},
	{15, 15,  4,  4,  4,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  4,  4,  4, 15},
	{15, 15,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, 15, 15},
	{15, 15,  4,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 15, 15, 15},
	{15, 15,  4,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 15, 15, 15},
	{15,  4,  4,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 15, 15, 15},
	{15,  4,  4,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 15, 15, 15},
	{ 15,  4,  4,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 15, 15, 15},
	{ 15,  4,  4, 15, 15, 15,  0,  0, 15, 15, 15, 15, 15,  0,  0,  15, 15, 15, 15, 15}
		};

static const vector<vector<int>> CHARACTER_PIXELS_3 = {
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  6,  6,  0, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15,  7,  6,   6,  0, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15,  0,  8,  8,  8,  6,  0,  0, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15,  0,  8,  8,  8,  8,  6,  8,  8,  0, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  8,  8,  8,  8,  8,  6,  8,  8,  0, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  8,  8,  6,  6,  6, 6,  6,  6,  6, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  8,  8,  6,  8,  8,  6,  8,  8,  6, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  8,  8,  6,  1,  0,  0,  0,  1,  6, 15, 15, 15, 15, 15},
	{15, 15, 15,  0,  0,  8,  8,  8,  6,  0,  0,  0,  0,  0,  6,  0, 15, 15, 15, 15},
	{15, 15,  0,  6,  8,  8,  8,  0,  8,  8,  8,  8,  8,  8,  0,  8,  0, 15, 15, 15},
	{15, 15,  0,  8,  6,  6,  6,  0,  8,  8,  8,  8,  8,  8,  8,  6,  0, 15, 15, 15},
	{15, 15, 15,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  0,  0, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  0,  6,  8,  8,  8,  8,  8,  8,  0, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  0,  0,  0,  6,  6,  6,  6,  6,  0, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15,  0,  0, 15, 15, 15, 15,  0,  0, 15, 15, 15, 15, 15, 15}
		};

static const vector<vector<int>> CHARACTER_PIXELS_4 = {
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15,  0,  4,  4,  4,  0,  0,  0, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  0,  4,  7,  7,  7,  7,  7,  7,  0, 15, 15, 15, 15, 15},
	{15, 15, 15, 15,  0,  7,  7,  4,  7,  7,  7,  7,  7,  7,  4,  0, 15, 15, 15, 15},
	{15, 15, 15, 15,  0,  7,  7,  7,  7,  4,  4,  4,  4,  4,  7,  7,  0, 15, 15, 15},
	{15, 15, 15, 15,  0,  4,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  4, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  0,  7,  7,  7,  7,  7,  7,  7,  7,  4,  0, 15, 15, 15},
	{15, 15, 15, 15, 15,  0,  0,  8,  8,  4,  0,  0,  0,  4,  4, 15, 15, 14, 15, 15},
	{15, 15,  0,  0,  0,  0,  0,  8,  8,  4,  0,  0,  0,  6,  6, 14, 14, 14, 15, 15},
	{15, 15,  0,  0,  4,  4,  4,  4,  8,  4,  0,  0,  0,  4,  8, 15, 15, 15, 15, 15},
	{15, 15,  0,  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8, 15, 15, 15, 15, 15},
	{15, 15,  0,  4,  4,  4,  4,  4,  4,  4,  4,  4, 12,  4,  0, 15, 15, 15, 15, 15},
	{15, 15, 15,  0,  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  0, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15,  0,  0,  0,  0,  0,  0,  0, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15,  0,  0, 15, 15, 15,  0,  0, 15, 15, 15, 15, 15, 15}
		};

static const char* CHARACTER_NAMES[] = {
    "Knight",
    "Assassin",
    "Vampire",
    "Paladin",
    "Officer"
};

static int ClampCharacterOption(int option) {
    if (option < 0) return 0;
    if (option > 4) return 4;
    return option;
}

const vector<vector<int>>& GetCharacterPixels(int option) {
    switch (ClampCharacterOption(option)) {
    case 0: return CHARACTER_PIXELS_0;
    case 1: return CHARACTER_PIXELS_1;
    case 2: return CHARACTER_PIXELS_2;
    case 3: return CHARACTER_PIXELS_3;
    default: return CHARACTER_PIXELS_4;
    }
}

string GetCharacterName(int option) {
    return CHARACTER_NAMES[ClampCharacterOption(option)];
}

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

static void DrawIngameCharacterName(const string& text, int startX, int startY, int width) {
    string label = text;
    if ((int)label.length() > width - 2) {
        label = label.substr(0, max(0, width - 5)) + "...";
    }

    const int groundR = 92;
    const int groundG = 58;
    const int groundB = 32;
    int labelW = min(width, (int)label.length() + 2);
    int offset = (width - labelW) / 2;
    if (offset < 0) offset = 0;

    GotoXY(startX, startY);
    printf("\x1b[38;2;235;240;245m\x1b[48;2;%d;%d;%dm", groundR, groundG, groundB);
    cout << string(width, ' ');
    GotoXY(startX + offset, startY);
    cout << " " << label << " ";
    printf("\x1b[0m");
}

static void DrawBadgePixel(int x, int y, int color) {
    GotoXY(x, y);
    SetColor(color, color);
    cout << "  ";
}

static void DrawTurnBadge(int startX, char mark, bool active) {
    static const char* X_GLYPH[5] = {
        "10001",
        "01010",
        "00100",
        "01010",
        "10001"
    };
    static const char* O_GLYPH[5] = {
        "01110",
        "10001",
        "10001",
        "10001",
        "01110"
    };

    const char** glyph = (mark == 'X') ? X_GLYPH : O_GLYPH;
    int color = active ? ((mark == 'X') ? 12 : 10) : 8;
    int badgeW = 10;
    int x = startX + (SPRITE_DRAW_W - badgeW) / 2;
    int y = 0;

    RestoreGameBackgroundRect(startX, y, SPRITE_DRAW_W, 5);

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            if (glyph[row][col] == '1') {
                DrawBadgePixel(x + col * 2, y + row, color);
            }
        }
    }
    SetColor(15, 0);
}

void DrawIngameTurnBadges() {
    DrawTurnBadge(LEFT_SPRITE_X, 'X', _TURN == true);
    DrawTurnBadge(RIGHT_SPRITE_X, 'O', _TURN == false);
    GotoXY(_X, _Y);
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

    FillConsoleRect(x, y, w, h, bgColor);
    SetColor(borderColor, bgColor);
    GotoXY(x, y);
    cout << "\xE2\x94\x8C";
    for (int i = 0; i < w - 2; i++) cout << (selected ? "\xE2\x95\x90" : "\xE2\x94\x80");
    cout << "\xE2\x94\x90";

    for (int row = 1; row < h - 1; row++) {
        GotoXY(x, y + row);
        cout << "\xE2\x94\x82";
        GotoXY(x + w - 1, y + row);
        cout << "\xE2\x94\x82";
    }

    GotoXY(x, y + h - 1);
    cout << "\xE2\x94\x94";
    for (int i = 0; i < w - 2; i++) cout << (selected ? "\xE2\x95\x90" : "\xE2\x94\x80");
    cout << "\xE2\x94\x98";
    SetColor(0, 15);
}

// Ve nhan vat trong menu chon va trong tran dau.
static void DrawCharacterPreviewBox(const vector<vector<int>>& pixels, const string& name, int x, int y, int boxW, int boxH, bool selected, bool disabled) {
    DrawPreviewFrame(x, y, boxW, boxH, selected, disabled);

    int spriteW = min(20, boxW - 6);
    int spriteH = min(9, boxH - 4);
    int spriteX = x + (boxW - spriteW) / 2;
    int spriteY = y + 1;
    DrawCharacterPixelsScaled(pixels, spriteX, spriteY, spriteW, spriteH, false);

    string label = disabled ? "DA CHON" : UpperAscii(name);
    if (selected && !disabled) label = "> " + label + " <";
    DrawCharacterName(label, x + 1, y + boxH - 2, boxW - 2);
}

void DrawCharacterPreview(int option, int x, int y, int boxW, int boxH, bool selected, bool disabled) {
    DrawCharacterPreviewBox(GetCharacterPixels(option), GetCharacterName(option), x, y, boxW, boxH, selected, disabled);
}

void outsidedisplay(int option) {
    DrawCharacterPixels(GetCharacterPixels(option), 18, 5, 2);
    DrawCharacterName(GetCharacterName(option), 18, 26, 40);
}

void ingamedisplay(int option, bool isPlayer1) {
    string playerName = isPlayer1 ? _PLAYER1_NAME : _PLAYER2_NAME;
    int x = isPlayer1 ? LEFT_SPRITE_X : RIGHT_SPRITE_X;
    bool flip = !isPlayer1;

    RestoreGameBackgroundRect(x, 0, SPRITE_DRAW_W, SPRITE_START_Y + SPRITE_DRAW_H + 2);
    DrawTurnBadge(x, isPlayer1 ? 'X' : 'O', isPlayer1 ? (_TURN == true) : (_TURN == false));
    DrawCharacterPixelsScaled(GetCharacterPixels(option), x, SPRITE_START_Y, SPRITE_DRAW_W, SPRITE_DRAW_H, flip);
    DrawIngameCharacterName(playerName, x, SPRITE_START_Y + SPRITE_DRAW_H + 1, SPRITE_DRAW_W);
}

void DrawBothSprites() {
    if (CharacterASelect < 0) CharacterASelect = 0;
    if (CharacterBSelect < 0) CharacterBSelect = 1;
    ingamedisplay(CharacterASelect, true);
    ingamedisplay(CharacterBSelect, false);
}
