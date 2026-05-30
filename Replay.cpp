#include "Replay.h"
#include "DrawBoard.h"
#include "ControlConsole.h"
#include "GamePlay.h"
#include "GameStatus.h"
#include "FinishProcess.h"
#include "Menu.h"
#include "Character.h"
#include "DrawBackground.hpp"
#include "UserInfo.h"
#include "Language.h"
#include <algorithm>
#include <cctype>
#include <conio.h>
#include <cstdio>
#include <string>
#include <windows.h>

using namespace std;

static const int REPLAY_GROUND_R = 92;
static const int REPLAY_GROUND_G = 58;
static const int REPLAY_GROUND_B = 32;
static const int REPLAY_PANEL_R = 16;
static const int REPLAY_PANEL_G = 22;
static const int REPLAY_PANEL_B = 32;
static const int REPLAY_ACCENT_R = 80;
static const int REPLAY_ACCENT_G = 245;
static const int REPLAY_ACCENT_B = 230;
static const int REPLAY_KEY_LEFT = 1000 + 75;
static const int REPLAY_KEY_RIGHT = 1000 + 77;

static int gReplayTotalMoves = 0;
static bool gReplayHudNeedsFullDraw = true;
static int gReplayHudLastStep = -1;
static int gReplayHudLastTotal = -1;
static bool gReplayHudLastPaused = false;
static string gReplayHudLastMessage;

static void InvalidateReplayHud() {
    gReplayHudNeedsFullDraw = true;
    gReplayHudLastStep = -1;
    gReplayHudLastTotal = -1;
    gReplayHudLastPaused = false;
    gReplayHudLastMessage.clear();
}

static int ReadReplayKey() {
    int key = _getch();
    if (key == 0 || key == 224) {
        return 1000 + _getch();
    }
    return toupper((unsigned char)key);
}

static string ReplayStatusText(bool paused) {
    return paused
        ? ((GetLanguage() == GameLanguage::Vietnamese) ? u8"T\u1EA0M D\u1EEANG" : "PAUSED")
        : ((GetLanguage() == GameLanguage::Vietnamese) ? u8"\u0110ANG PH\u00C1T" : "PLAYING");
}

static string ReplayDoneText() {
    return (GetLanguage() == GameLanguage::Vietnamese) ? u8"\u0110\u00C3 PH\u00C1T XONG" : "FINISHED";
}

static string ReplayStepLabel() {
    return (GetLanguage() == GameLanguage::Vietnamese) ? u8"B\u01AF\u1EDAC: " : "STEP: ";
}

static string ReplayProgressLabel() {
    return (GetLanguage() == GameLanguage::Vietnamese) ? u8"TI\u1EBEN TR\u00CCNH" : "PROGRESS";
}

static string ReplayControlsText() {
    return (GetLanguage() == GameLanguage::Vietnamese)
        ? u8"A: TUA L\u00D9I   D: TUA T\u1EDAI   SPACE/P: T\u1EA0M D\u1EEANG   ESC: THO\u00C1T"
        : "A: REWIND   D: FAST FORWARD   SPACE/P: PAUSE   ESC: EXIT";
}

static void PrintRgbText(int x, int y, const string& text, int r, int g, int b, int bgR, int bgG, int bgB) {
    if (y < 0 || y >= CONSOLE_LINES || x >= CONSOLE_COLS - 1) return;

    GotoXY(max(0, x), y);
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", r, g, b, bgR, bgG, bgB);
    cout << text;
    printf("\x1b[0m");
}

static void FillRgbRect(int x, int y, int w, int h, int r, int g, int b) {
    if (w <= 0 || h <= 0) return;

    int startX = max(0, x);
    int drawW = min(w, CONSOLE_COLS - startX - 1);
    if (drawW <= 0) return;

    string row(drawW, ' ');
    printf("\x1b[48;2;%d;%d;%dm", r, g, b);
    for (int i = 0; i < h; i++) {
        int yy = y + i;
        if (yy < 0 || yy >= CONSOLE_LINES) continue;
        GotoXY(startX, yy);
        cout << row;
    }
    printf("\x1b[0m");
}

static void DrawRgbFrame(int x, int y, int w, int h, int r, int g, int b, int bgR, int bgG, int bgB) {
    if (w < 2 || h < 2) return;

    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", r, g, b, bgR, bgG, bgB);
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
    printf("\x1b[0m");
}

static void PrintCenteredRgb(int x, int y, int w, const string& text, int r, int g, int b, int bgR, int bgG, int bgB) {
    int textX = x + (w - TextDisplayWidth(text)) / 2;
    if (textX < x + 1) textX = x + 1;
    PrintRgbText(textX, y, text, r, g, b, bgR, bgG, bgB);
}

static void DrawReplayPill(int x, int y, int w, const string& text, bool highlighted) {
    int bgR = highlighted ? 255 : 15;
    int bgG = highlighted ? 240 : 82;
    int bgB = highlighted ? 90 : 145;
    int fgR = highlighted ? 20 : 230;
    int fgG = highlighted ? 20 : 245;
    int fgB = highlighted ? 20 : 255;
    int borderR = highlighted ? 255 : REPLAY_ACCENT_R;
    int borderG = highlighted ? 240 : REPLAY_ACCENT_G;
    int borderB = highlighted ? 90 : REPLAY_ACCENT_B;

    FillRgbRect(x, y, w, 3, bgR, bgG, bgB);
    DrawRgbFrame(x, y, w, 3, borderR, borderG, borderB, bgR, bgG, bgB);
    PrintCenteredRgb(x, y + 1, w, text, fgR, fgG, fgB, bgR, bgG, bgB);
}

static void DrawReplayProgress(int x, int y, int w, int step, int totalMoves) {
    int barW = max(0, w - 2);
    int filled = (totalMoves <= 0) ? 0 : (barW * step) / totalMoves;
    if (filled > barW) filled = barW;

    GotoXY(x, y);
    printf("\x1b[38;2;220;245;255m\x1b[48;2;%d;%d;%dm[", REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    printf("\x1b[48;2;55;210;190m");
    cout << string(filled, ' ');
    printf("\x1b[48;2;48;54;70m");
    cout << string(barW - filled, ' ');
    printf("\x1b[48;2;%d;%d;%dm]", REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    printf("\x1b[0m");
}

static void DrawReplaySideStatus(int step, int totalMoves, bool paused, const string& message) {
    string stepText = ReplayStepLabel() + to_string(step) + "/" + to_string(totalMoves);
    string statusText = message.empty() ? ReplayStatusText(paused) : message;

    const int boardW = BOARD_SIZE * 4 + 1;
    const int boxW = 26;
    const int boxH = 3;
    const int sideY = TOP + 3;
    const int leftX = max(2, LEFT - boxW - 3);
    const int rightX = min(CONSOLE_COLS - boxW - 2, LEFT + boardW + 3);

    FillRgbRect(leftX, sideY, boxW, boxH, REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    DrawRgbFrame(leftX, sideY, boxW, boxH, REPLAY_ACCENT_R, REPLAY_ACCENT_G, REPLAY_ACCENT_B,
        REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    PrintCenteredRgb(leftX, sideY + 1, boxW, stepText, 0, 255, 255, REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);

    FillRgbRect(rightX, sideY, boxW, boxH, REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    DrawRgbFrame(rightX, sideY, boxW, boxH, REPLAY_ACCENT_R, REPLAY_ACCENT_G, REPLAY_ACCENT_B,
        REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    PrintCenteredRgb(rightX, sideY + 1, boxW, statusText,
        paused ? 255 : 80, paused ? 240 : 255, paused ? 80 : 110,
        REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
}

static void DrawReplayHud(int step, int totalMoves, bool paused, const string& message = "") {
    const int panelX = 6;
    const int panelY = INFO_Y - 1;
    const int panelW = CONSOLE_COLS - 12;
    const int panelH = 9;
    string messageText = message.empty() ? ReplayStatusText(paused) : message;
    bool fullDraw = gReplayHudNeedsFullDraw || totalMoves != gReplayHudLastTotal;

    if (fullDraw) {
        FillRgbRect(0, panelY, CONSOLE_COLS, CONSOLE_LINES - panelY, REPLAY_GROUND_R, REPLAY_GROUND_G, REPLAY_GROUND_B);
        FillRgbRect(panelX, panelY + 1, panelW, panelH, REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
        DrawRgbFrame(panelX, panelY + 1, panelW, panelH, REPLAY_ACCENT_R, REPLAY_ACCENT_G, REPLAY_ACCENT_B,
            REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    }

    if (fullDraw || step != gReplayHudLastStep || paused != gReplayHudLastPaused || messageText != gReplayHudLastMessage) {
        DrawReplaySideStatus(step, totalMoves, paused, messageText);
    }

    if (fullDraw || step != gReplayHudLastStep) {
        DrawReplayProgress(panelX + 8, panelY + 2, panelW - 16, step, totalMoves);
    }

    int buttonY = panelY + 4;
    int buttonW = 18;
    int gap = 3;
    int totalButtonW = buttonW * 4 + gap * 3;
    int buttonX = CenterConsoleX(totalButtonW, CONSOLE_COLS);
    if (fullDraw) {
        DrawReplayPill(buttonX, buttonY, buttonW, "A  << TUA -5S", false);
        DrawReplayPill(buttonX + (buttonW + gap), buttonY, buttonW, "D  TUA +5S >>", false);
        DrawReplayPill(buttonX + (buttonW + gap) * 2, buttonY, buttonW, paused ? "PLAY" : "PAUSE", paused);
        DrawReplayPill(buttonX + (buttonW + gap) * 3, buttonY, buttonW, "ESC", false);
    }
    else if (paused != gReplayHudLastPaused) {
        DrawReplayPill(buttonX + (buttonW + gap) * 2, buttonY, buttonW, paused ? "PLAY" : "PAUSE", paused);
    }

    gReplayHudNeedsFullDraw = false;
    gReplayHudLastStep = step;
    gReplayHudLastTotal = totalMoves;
    gReplayHudLastPaused = paused;
    gReplayHudLastMessage = messageText;
}

bool AskForReplay() {
    return DrawFinishQuestion(L(TextId::ReplayQuestion));
}

void ChooseReplaySpeed(float& speed, int& delay) {
    int choice = 1;
    system("cls");
    DrawUIBackground();

    int consoleW = CONSOLE_COLS;
    string prompt = (GetLanguage() == GameLanguage::Vietnamese)
        ? u8"CH\u1ECCN T\u1ED0C \u0110\u1ED8 PH\u00C1T L\u1EA0I"
        : "SELECT REPLAY SPEED";
    string help = (GetLanguage() == GameLanguage::Vietnamese)
        ? u8"A/D HO\u1EB6C M\u0168I T\u00CAN \u0110\u1EC2 CH\u1ECCN, ENTER \u0110\u1EC2 B\u1EAET \u0110\u1EA6U"
        : "A/D OR ARROWS TO SELECT, ENTER TO START";
    int panelW = 76;
    int panelH = 16;
    int panelX = CenterConsoleX(panelW, consoleW);
    int panelY = 8;

    FillRgbRect(panelX, panelY, panelW, panelH, REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    DrawRgbFrame(panelX, panelY, panelW, panelH, REPLAY_ACCENT_R, REPLAY_ACCENT_G, REPLAY_ACCENT_B,
        REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    PrintCenteredRgb(panelX, panelY + 2, panelW, prompt, 255, 245, 80, REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);
    PrintCenteredRgb(panelX, panelY + 4, panelW, help, 0, 255, 255, REPLAY_PANEL_R, REPLAY_PANEL_G, REPLAY_PANEL_B);

    int btnW = 18;
    int gap = 5;
    int totalBtnW = btnW * 3 + gap * 2;
    int startX = CenterConsoleX(totalBtnW, consoleW);
    int btnY = panelY + 8;
    int lastChoice = -1;

    while (true) {
        if (choice != lastChoice) {
            string opts[3] = { "0.5x", "1.0x", "1.5x" };
            for (int i = 0; i < 3; i++) {
                int bX = startX + i * (btnW + gap);
                DrawReplayPill(bX, btnY, btnW, opts[i], choice == i);
            }
            lastChoice = choice;
        }

        SetColor(0, 15);
        int ch = ReadReplayKey();
        if (ch == REPLAY_KEY_LEFT || ch == 'A') {
            if (choice > 0) {
                choice--;
                PlayMenuSound();
            }
        }
        else if (ch == REPLAY_KEY_RIGHT || ch == 'D') {
            if (choice < 2) {
                choice++;
                PlayMenuSound();
            }
        }
        else if (ch == 13) {
            PlayMenuSound();
            break;
        }
    }

    if (choice == 0) { speed = 0.5f; delay = (int)(500 / 0.5f); }
    else if (choice == 1) { speed = 1.0f; delay = 500; }
    else { speed = 1.5f; delay = (int)(500 / 1.5f); }
}

void RedrawBoard(int step) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j].c = 0;
        }
    }

    system("cls");
    DrawIngameBackground();
    DrawBoard(BOARD_SIZE);
    InvalidateReplayHud();
    DrawReplayHud(step, gReplayTotalMoves, false);

    for (int i = 0; i < step; i++) {
        int r = moveHistory[i].row;
        int c = moveHistory[i].col;
        _A[r][c].c = moveHistory[i].c;
        DrawCell(_A[r][c].x, _A[r][c].y, BOARD_BG_COLOR);
    }
}

void PlayReplay(int delay) {
    int totalMoves = currentStep;
    gReplayTotalMoves = totalMoves;
    int step = 0;
    int movesToSkipFor5s = 5000 / delay;
    if (movesToSkipFor5s < 1) movesToSkipFor5s = 1;

    RedrawBoard(0);

    bool isPaused = false;
    bool exitReplay = false;

    while (step < totalMoves) {
        bool skipTriggered = false;
        ULONGLONG startTime = GetTickCount64();

        while (isPaused || (GetTickCount64() - startTime < delay)) {
            if (_kbhit()) {
                int ch = ReadReplayKey();

                if (ch == 27) {
                    PlayMenuSound();
                    exitReplay = true;
                    break;
                }
                else if (ch == 'P' || ch == 32) {
                    PlayMenuSound();
                    isPaused = !isPaused;
                    DrawReplayHud(step, totalMoves, isPaused);
                    if (!isPaused) {
                        startTime = GetTickCount64();
                    }
                }
                else if (ch == REPLAY_KEY_LEFT || ch == 'A') {
                    PlayMenuSound();
                    int targetStep = step - movesToSkipFor5s;
                    if (targetStep < 0) targetStep = 0;

                    for (int i = step - 1; i >= targetStep; i--) {
                        int r = moveHistory[i].row;
                        int c = moveHistory[i].col;
                        _A[r][c].c = 0;
                        DrawCell(_A[r][c].x, _A[r][c].y, BOARD_BG_COLOR);
                    }
                    step = targetStep;

                    DrawReplayHud(step, totalMoves, isPaused, "<<  -5S");
                    skipTriggered = true;
                    break;
                }
                else if (ch == REPLAY_KEY_RIGHT || ch == 'D') {
                    PlayMenuSound();
                    int targetStep = step + movesToSkipFor5s;
                    if (targetStep > totalMoves) targetStep = totalMoves;

                    for (int i = step; i < targetStep; i++) {
                        int r = moveHistory[i].row;
                        int c = moveHistory[i].col;
                        _A[r][c].c = moveHistory[i].c;
                        DrawCell(_A[r][c].x, _A[r][c].y, BOARD_BG_COLOR);
                    }
                    step = targetStep;

                    DrawReplayHud(step, totalMoves, isPaused, ">>  +5S");
                    skipTriggered = true;
                    break;
                }
            }
            Sleep(10);

            if (isPaused) startTime = GetTickCount64();
        }

        if (exitReplay) break;

        if (skipTriggered) {
            Sleep(400);
            DrawReplayHud(step, totalMoves, isPaused);
            continue;
        }

        if (step < totalMoves) {
            int r = moveHistory[step].row;
            int c = moveHistory[step].col;
            _A[r][c].c = moveHistory[step].c;
            DrawCell(_A[r][c].x, _A[r][c].y, BOARD_BG_COLOR);
            step++;
            DrawReplayHud(step, totalMoves, isPaused);
        }
    }

    if (!exitReplay) {
        DrawReplayHud(step, totalMoves, true, ReplayDoneText());
        Sleep(900);
    }
}

void HandleReplayOption() {
    _POINT savedBoard[BOARD_SIZE][BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            savedBoard[i][j] = _A[i][j];
        }
    }

    system("cls");
    while (AskForReplay()) {
        float speed;
        int delay;
        ChooseReplaySpeed(speed, delay);
        PlayReplay(delay);
        system("cls");
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j] = savedBoard[i][j];
        }
    }

    system("cls");
    DrawIngameBackground();
    DrawBoard(BOARD_SIZE);
    DrawPlayerInfo();
    UpdateTurnInfo();
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c != 0) {
                DrawCell(_A[i][j].x, _A[i][j].y, BOARD_BG_COLOR);
            }
        }
    }
    DrawCell(_X, _Y, BOARD_CURSOR_COLOR);
    ingamedisplay(CharacterASelect, true);
    ingamedisplay(CharacterBSelect, false);
}
