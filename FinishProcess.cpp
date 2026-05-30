#include "FinishProcess.h"
#include "GameStatus.h"
#include "ControlConsole.h"
#include "UserInfo.h"
#include "Sound.h"
#include "Language.h"

#include "Menu.h"
#include "btn_normal.h"
#include "btn_hover.h"
#include <utility>
#include <vector>
#include <windows.h>

using namespace std;

static int gLastFinishResult = 2;

static string FinishWinnerName() {
    if (gLastFinishResult == -1) return _PLAYER1_NAME;
    if (gLastFinishResult == 1) return _PLAYER2_NAME;
    return "";
}

static int FinishWinnerCharacter() {
    if (gLastFinishResult == -1) return CharacterASelect;
    if (gLastFinishResult == 1) return CharacterBSelect;
    return -1;
}

static string CharacterNameFromOption(int option) {
    switch (option) {
    case 0: return "KNIGHT";
    case 1: return "ASSASSIN";
    case 2: return "VAMPIRE";
    case 3: return "PALADIN";
    default: return "OFFICER";
    }
}

int GetLastFinishResult() {
    return gLastFinishResult;
}

static void PrintPlainText(int x, int y, const string& text, int colorCode) {
    int r = 255, g = 255, b = 255;
    if (colorCode == 11) { r = 0; g = 255; b = 255; }
    else if (colorCode == 14) { r = 255; g = 255; b = 0; }
    else if (colorCode == 12) { r = 255; g = 70; b = 70; }
    else if (colorCode == 10) { r = 90; g = 255; b = 90; }

    GotoXY(x, y);
    printf("\x1b[38;2;%d;%d;%dm", r, g, b);
    cout << text;
    printf("\x1b[0m");
}

static void DrawOutlineFrame(int x, int y, int w, int h, int color) {
    SetColor(color, 0);
    GotoXY(x, y);
    cout << "\xE2\x94\x8C";
    for (int i = 0; i < w - 2; i++) cout << "\xE2\x94\x80";
    cout << "\xE2\x94\x90";

    for (int row = 1; row < h - 1; row++) {
        GotoXY(x, y + row);
        cout << "\xE2\x94\x82";
        GotoXY(x + w - 1, y + row);
        cout << "\xE2\x94\x82";
    }

    GotoXY(x, y + h - 1);
    cout << "\xE2\x94\x94";
    for (int i = 0; i < w - 2; i++) cout << "\xE2\x94\x80";
    cout << "\xE2\x94\x98";
    SetColor(15, 0);
}

static void FillConsoleBlock(int x, int y, int w, int h, int bgColor) {
    SetColor(15, bgColor);
    string line(w, ' ');
    for (int row = 0; row < h; row++) {
        GotoXY(x, y + row);
        cout << line;
    }
    SetColor(0, 15);
}

static void DrawSparseLine(int x, int y, const string& text, int colorCode) {
    for (int i = 0; i < (int)text.length(); i++) {
        if (text[i] == ' ') continue;
        PrintPlainText(x + i, y, string(1, text[i]), colorCode);
    }
}

static void DrawSwordArt(int centerX, int y) {
    DrawSparseLine(centerX - 6, y,     "     /\\     ", 14);
    DrawSparseLine(centerX - 6, y + 1, "    /  \\    ", 14);
    DrawSparseLine(centerX - 6, y + 2, "   /====\\   ", 14);
    DrawSparseLine(centerX - 6, y + 3, "     ||     ", 15);
    DrawSparseLine(centerX - 6, y + 4, "  ---||---  ", 15);
    DrawSparseLine(centerX - 6, y + 5, "     []     ", 14);
}

static void DrawChoiceButton(int x, int y, int w, int h, const string& text, bool selected) {
    int bg = selected ? 12 : 1;
    int fg = 15;
    int border = selected ? 14 : 11;
    string fill(w, ' ');

    SetColor(fg, bg);
    for (int row = 0; row < h; row++) {
        GotoXY(x, y + row);
        cout << fill;
    }

    SetColor(border, bg);
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

    SetColor(fg, bg);
    GotoXY(x + (w - TextDisplayWidth(text)) / 2, y + h / 2);
    cout << text;
    SetColor(15, 0);
}

void DrawFinishCelebrationScreen() {
    system("cls");
    DrawUIBackground();

    int panelW = 94;
    int panelH = 34;
    int panelX = CenterConsoleX(panelW, CONSOLE_COLS);
    int panelY = 1;
    DrawOutlineFrame(panelX, panelY, panelW, panelH, 15);

    if (gLastFinishResult == -1 || gLastFinishResult == 1) {
        int character = FinishWinnerCharacter();
        DrawCharacterPreview(character, CenterConsoleX(34, CONSOLE_COLS), panelY + 2, 34, 14, true, false);

        string winner = L(Winner) + FinishWinnerName();

        PrintPlainText(CenterConsoleX(TextDisplayWidth(L(Victory)), CONSOLE_COLS), panelY + 17, L(Victory), 14);
        PrintPlainText(CenterConsoleX(TextDisplayWidth(winner), CONSOLE_COLS), panelY + 19, winner, 14);
    }
    else {
        string drawText = L(DrawMessage);
        PrintPlainText(CenterConsoleX(TextDisplayWidth(L(Draw)), CONSOLE_COLS), panelY + 10, L(Draw), 14);
        PrintPlainText(CenterConsoleX(TextDisplayWidth(drawText), CONSOLE_COLS), panelY + 13, drawText, 11);
    }
}

bool DrawFinishQuestion(const string& prompt) {
    DrawFinishCelebrationScreen();

    const int questionW = 64;
    const int questionH = 8;
    const int questionX = CenterConsoleX(questionW, CONSOLE_COLS);
    const int questionY = 22;
    const int promptY = questionY + 1;
    const int btnY = questionY + 4;
    const int btnW = 18;
    const int btnH = 3;
    const int totalBtnW = btnW * 2 + 12;
    const int startX = CenterConsoleX(totalBtnW, CONSOLE_COLS);

    FillConsoleBlock(questionX, questionY, questionW, questionH, 1);
    DrawOutlineFrame(questionX, questionY, questionW, questionH, 14);
    PrintTextWithBg(CenterConsoleX(TextDisplayWidth(prompt), CONSOLE_COLS), promptY, prompt, 14);

    int choice = 0;
    int lastChoice = -1;
    while (true) {
        if (choice != lastChoice) {
            int yesX = startX;
            int noX = startX + btnW + 12;

            DrawChoiceButton(yesX, btnY, btnW, btnH, L(Yes), choice == 0);
            DrawChoiceButton(noX, btnY, btnW, btnH, L(No), choice == 1);

            SetColor(0, 15);
            lastChoice = choice;
        }

        int ch = ReadMenuKey();
        int oldChoice = choice;
        if (ch == 75 || ch == 'A') choice = 0;
        else if (ch == 77 || ch == 'D') choice = 1;
        else if (ch == 13) {
            PlayMenuSound();
            return choice == 0;
        }

        if (choice != oldChoice) {
            PlayMenuSound();
        }
    }
}

static bool CollectWinningCells(int winner, vector<pair<int, int>>& cells) {
    if (winner == 0 || winner == 2) return false;

    const int dirs[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {-1, 1}
    };

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (_A[r][c].c != winner) continue;

            for (int d = 0; d < 4; d++) {
                cells.clear();
                bool ok = true;

                for (int k = 0; k < 5; k++) {
                    int rr = r + dirs[d][0] * k;
                    int cc = c + dirs[d][1] * k;

                    if (rr < 0 || rr >= BOARD_SIZE || cc < 0 || cc >= BOARD_SIZE || _A[rr][cc].c != winner) {
                        ok = false;
                        break;
                    }
                    cells.push_back({ rr, cc });
                }

                if (ok) return true;
            }
        }
    }

    cells.clear();
    return false;
}

static void DrawWinningCell(int r, int c) {
    int x = _A[r][c].x;
    int y = _A[r][c].y;
    char mark = (_A[r][c].c == -1) ? 'X' : 'O';

    GotoXY(x - 1, y);
    SetColor((_A[r][c].c == -1) ? 12 : 10, 14);
    cout << " " << mark << " ";
}

static void RestoreWinningCell(int r, int c) {
    int x = _A[r][c].x;
    int y = _A[r][c].y;

    GotoXY(x - 1, y);
    SetColor(0, BOARD_BG_COLOR);
    cout << "   ";

    if (_A[r][c].c == -1) {
        GotoXY(x, y);
        SetColor(12, BOARD_BG_COLOR);
        cout << "X";
    }
    else if (_A[r][c].c == 1) {
        GotoXY(x, y);
        SetColor(10, BOARD_BG_COLOR);
        cout << "O";
    }

    SetColor(0, 15);
}

static void HighlightWinningLine(int winner) {
    vector<pair<int, int>> cells;
    if (!CollectWinningCells(winner, cells)) return;

    PlayWinSound();

    for (const auto& cell : cells) {
        DrawWinningCell(cell.first, cell.second);
    }
    GotoXY(_X, _Y);

    for (int i = 0; i < 50; i++) {
        if (_kbhit()) {
            int key = ReadMenuKey();
            if (key == 13) break;
        }
        Sleep(100);
    }

    for (const auto& cell : cells) {
        RestoreWinningCell(cell.first, cell.second);
    }
    GotoXY(_X, _Y);
}

void GarbageCollect() {
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            _A[i][j].c = 0;
}

void ExitGame() {
    system("cls");
    GarbageCollect();
}

int ProcessFinish(int pWhoWin) {
    gLastFinishResult = pWhoWin;

    if (pWhoWin == -1 || pWhoWin == 1) {
        HighlightWinningLine(pWhoWin);
    }

    GotoXY(RESULT_X, RESULT_Y);
    cout << string(80, ' ');
    GotoXY(RESULT_X, RESULT_Y);

    SetColor(1, 15);

    switch (pWhoWin) {
    case -1:
        if (GetLanguage() == Vietnamese) {
            cout << _PLAYER1_NAME << " (X) da THANG va " << _PLAYER2_NAME << " (O) da THUA!          ";
        }
        else {
            cout << _PLAYER1_NAME << " (X) WON and " << _PLAYER2_NAME << " (O) LOST!          ";
        }
        break;
    case  1:
        if (GetLanguage() == Vietnamese) {
            cout << _PLAYER2_NAME << " (O) da THANG va " << _PLAYER1_NAME << " (X) da THUA!          ";
        }
        else {
            cout << _PLAYER2_NAME << " (O) WON and " << _PLAYER1_NAME << " (X) LOST!          ";
        }
        break;
    case  0:
        if (GetLanguage() == Vietnamese) {
            cout << "Tran dau HOA! Ban co da het o trong.                          ";
        }
        else {
            cout << "DRAW! The board has no empty cells.                          ";
        }
        break;
    case  2:
        _TURN = !_TURN;
        UpdateTurnInfo();
        break;
    }

    SetColor(0, 15);
    GotoXY(_X, _Y);
    return pWhoWin;
}

int AskContinue() {
    bool yes = DrawFinishQuestion(L(PlayAgainQuestion));
    return yes ? 'Y' : 'N';
}

int TestBoard() {

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c != 0) {
                int c = _A[i][j].c;
                if (j <= BOARD_SIZE - 5
                    && _A[i][j + 1].c == c && _A[i][j + 2].c == c
                    && _A[i][j + 3].c == c && _A[i][j + 4].c == c) return c;
                if (i <= BOARD_SIZE - 5
                    && _A[i + 1][j].c == c && _A[i + 2][j].c == c
                    && _A[i + 3][j].c == c && _A[i + 4][j].c == c) return c;
                if (i <= BOARD_SIZE - 5 && j <= BOARD_SIZE - 5
                    && _A[i + 1][j + 1].c == c && _A[i + 2][j + 2].c == c
                    && _A[i + 3][j + 3].c == c && _A[i + 4][j + 4].c == c) return c;
                if (i >= 4 && j <= BOARD_SIZE - 5
                    && _A[i - 1][j + 1].c == c && _A[i - 2][j + 2].c == c
                    && _A[i - 3][j + 3].c == c && _A[i - 4][j + 4].c == c) return c;
            }
        }
    }


    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (_A[i][j].c == 0) return 2;

    return 0;
}
