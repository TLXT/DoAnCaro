#include "DrawBoard.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include <string>

using namespace std;

static string RepeatToken(const char* token, int count) {
    string result;
    for (int i = 0; i < count; i++) result += token;
    return result;
}

static string BuildBoardLine(const char* left, const char* mid, const char* right, int size) {
    string line = left;
    for (int i = 0; i < size; i++) {
        line += RepeatToken("\xE2\x94\x80", 3);
        line += (i == size - 1) ? right : mid;
    }
    return line;
}

void DrawBoard(int pSize) {
    SetColor(0, BOARD_BG_COLOR);

    int boardW = pSize * 4 + 1;
    for (int y = 0; y <= pSize * 2; y++) {
        GotoXY(LEFT, TOP + y);
        cout << string(boardW, ' ');
    }

    string topLine = BuildBoardLine("\xE2\x94\x8C", "\xE2\x94\xAC", "\xE2\x94\x90", pSize);
    string midLine = BuildBoardLine("\xE2\x94\x9C", "\xE2\x94\xBC", "\xE2\x94\xA4", pSize);
    string botLine = BuildBoardLine("\xE2\x94\x94", "\xE2\x94\xB4", "\xE2\x94\x98", pSize);

    string cellLine = "\xE2\x94\x82";
    for (int j = 0; j < pSize; j++) {
        cellLine += "   ";
        cellLine += "\xE2\x94\x82";
    }

    SetColor(8, BOARD_BG_COLOR);
    for (int i = 0; i <= pSize; i++) {
        GotoXY(LEFT, TOP + 2 * i);
        if (i == 0) cout << topLine;
        else if (i == pSize) cout << botLine;
        else cout << midLine;

        if (i < pSize) {
            GotoXY(LEFT, TOP + 2 * i + 1);
            cout << cellLine;
        }
    }

    SetColor(0, 15);
}
