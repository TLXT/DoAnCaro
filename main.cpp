#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <future>
#include "GameStatus.h"
#include "ControlConsole.h"
#include "GamePlay.h"
#include "FinishProcess.h"
#include "Menu.h"
#include "SaveLoad.h"
#include "UserInfo.h"
#include "DrawBoard.h"
#include "CaroBot.h"
#include "GameTimer.h"
#include "Replay.h"
#include "Sound.h"
#include "Character.h"
#include "Proccesor.h"


using namespace std;


void EnableRGBColor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);
}

void SetConsoleFontSize(int width, int height) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = width;
    cfi.dwFontSize.Y = height;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}


int main() {

    // Khoi tao console va am thanh.
    EnableRGBColor();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleFontSize(12, 16);

    FixConsoleWindow();


    system("MODE CON COLS=120 LINES=40");
    ConfigureConsoleSize(CONSOLE_COLS, CONSOLE_LINES);
    SetConsoleWindow(CONSOLE_COLS * 12, CONSOLE_LINES * 16);
    HideCursor();
    PlayMusic(static_cast<int>(GetTickCount64() % 5), volumeLevel);

    while (true) {
        int choice = MainMenu();
        bool isPlaying = false;

        if (choice == 0) {
            int mode = PlayGameMenu();

            if (mode == 0) {
                _BOT_MODE = false;
                if (!InputPlayerNames(false)) continue;
                StartGame();
                isPlaying = true;
            }
            else if (mode == 1) {
                int diff = DifficultyMenu();
                if (diff == 3) continue;

                _BOT_MODE = true;
                _BOT_DIFFICULTY = diff + 1;
                if (!InputPlayerNames(true)) continue;
                StartGame();
                isPlaying = true;
            }
            else if (mode == 2) {
                continue;
            }
        }
        else if (choice == 1) {
            if (LoadGame() == true) {
                ingamedisplay(CharacterASelect, true);
                ingamedisplay(CharacterBSelect, false);
                isPlaying = true;
            }
        }
        else if (choice == 2) {
            ShowGuideScreen();
        }
        else if (choice == 3) {
            loadSettingMenu();
        }
        else if (choice == 4) {
            ExitGame();
            return 0;
        }


        if (isPlaying) {
            // Vong lap gameplay chinh.
            bool validEnter = true;
            bool actionBarActive = false;
            int actionSelect = 0;
            auto RestartCurrentMatch = [&]() {
                isPaused = true;
                StopTimerThread();
                StartGame();
                StartTimerThread();
                validEnter = true;
                actionBarActive = false;
                actionSelect = 0;
                isPaused = false;
            };
            StartTimerThread();

            while (isPlaying) {


                if (timeLeft <= 0) {
                    PlayRandomMove();
                    int finish = ProcessFinish(TestBoard());

                    if (finish != 2) {
                        isPaused = true;
                        {
                            lock_guard<mutex> lock(consoleMutex);
                            PrintHudTextWithBg(TIMER_X, TIMER_Y, string(40, ' '), 15);
                            PrintHudTextWithBg(TIMER_X, TIMER_Y + 1, string(40, ' '), 15);
                        }

                        HandleReplayOption();

                        if (AskContinue() != 'Y') {
                            isPlaying = false;
                        }
                        else {
                            StartGame();
                            timeLeft = turnTimeLimit;
                            isPaused = false;
                        }
                    }
                    else {
                        timeLeft = turnTimeLimit;
                    }
                    continue;
                }


                if (_BOT_MODE == true && _TURN == false) {
                    loadBotMove(isPlaying);
                    continue;
                }


                if (_kbhit()) {
                    int ch = _getch();
                    if (ch == 0 || ch == 224) {
                        ch = _getch();
                        if (ch == 72) _COMMAND = 'W';
                        else if (ch == 80) _COMMAND = 'S';
                        else if (ch == 75) _COMMAND = 'A';
                        else if (ch == 77) _COMMAND = 'D';
                        else               _COMMAND = 0;
                    }
                    else {
                        _COMMAND = toupper(ch);
                    }

                    if (_COMMAND == 0) continue;

                    if (actionBarActive) {
                        if (_COMMAND == 'A') {
                            actionSelect--;
                            if (actionSelect < 0) actionSelect = GetActionCount() - 1;
                            DrawActionBar(actionSelect);
                            PlayMenuSound();
                            continue;
                        }
                        if (_COMMAND == 'D') {
                            actionSelect++;
                            if (actionSelect >= GetActionCount()) actionSelect = 0;
                            DrawActionBar(actionSelect);
                            PlayMenuSound();
                            continue;
                        }
                        if (_COMMAND == 'W') {
                            actionBarActive = false;
                            DrawActionBar(-1);
                            GotoXY(_X, _Y);
                            PlayMenuSound();
                            continue;
                        }
                        if (_COMMAND == 'S') {
                            continue;
                        }
                        if (_COMMAND == 13) {
                            actionBarActive = false;
                            DrawActionBar(-1);
                            PlayMenuSound();

                            if (actionSelect == 0) _COMMAND = 'Y';
                            else if (actionSelect == 1) _COMMAND = 'U';
                            else if (actionSelect == 2) _COMMAND = 'R';
                            else if (actionSelect == 3) _COMMAND = ' ';
                        }
                    }


                    if (_COMMAND == 'R') {
                        RestartCurrentMatch();
                        continue;
                    }

                    if (_COMMAND == 'M' || _COMMAND == 'P' || _COMMAND == ' ' || _COMMAND == 27) {
                        isPaused = true;
                        future<int> SecondThreadchoice = async(launch::async, GameMenu);
                        int gamechoice = SecondThreadchoice.get();

                        if (gamechoice == 0) {
                            isPlaying = false;
                        }
                        else if (gamechoice == 1) {
                            SaveGame();
                            if (loadPresent()) {
                                ingamedisplay(CharacterASelect, true);
                                ingamedisplay(CharacterBSelect, false);
                                timeLeft = turnTimeLimit;
                            }
                        }
                        else if (gamechoice == 2) {
                            if (LoadGame()) {
                                ingamedisplay(CharacterASelect, true);
                                ingamedisplay(CharacterBSelect, false);
                                timeLeft = turnTimeLimit;
                            }
                            else {
                                loadPresent();
                                ingamedisplay(CharacterASelect, true);
                                ingamedisplay(CharacterBSelect, false);
                            }
                        }
                        else if (gamechoice == 3) {
                            loadSettingMenu();
                            loadPresent();
                        }
                        else if (gamechoice == 4) {
                            loadPresent();
                            ingamedisplay(CharacterASelect, true);
                            ingamedisplay(CharacterBSelect, false);
                        }

                        isPaused = false;
                    }


                    else if (!isPaused) {
                        if (_COMMAND == 'Y') {
                            if (!_BOT_MODE || _TURN) { UndoMove(); timeLeft = turnTimeLimit; }
                        }
                        else if (_COMMAND == 'U') {
                            if (!_BOT_MODE || _TURN) { RedoMove(); timeLeft = turnTimeLimit; }
                        }
                        else if (_COMMAND == 'A' || _COMMAND == 75) MoveLeft();
                        else if (_COMMAND == 'W' || _COMMAND == 72) MoveUp();
                        else if (_COMMAND == 'S' || _COMMAND == 80) {
                            if (_Y >= _A[BOARD_SIZE - 1][0].y) {
                                actionBarActive = true;
                                actionSelect = 0;
                                DrawActionBar(actionSelect);
                                PlayMenuSound();
                            }
                            else {
                                MoveDown();
                            }
                        }
                        else if (_COMMAND == 'D' || _COMMAND == 77) MoveRight();

                        else if (_COMMAND == 13) {
                            int checkRes = CheckBoard(_X, _Y);
                            switch (checkRes) {
                            case -1: DrawCell(_X, _Y, BOARD_CURSOR_COLOR); break;
                            case  1: DrawCell(_X, _Y, BOARD_CURSOR_COLOR); break;
                            case  0: validEnter = false;   break;
                            }

                            if (validEnter == true) {

                                int r = (_Y - TOP - 1) / 2;
                                int c = (_X - LEFT - 2) / 4;
                                if (currentStep < (int)moveHistory.size())
                                    moveHistory.erase(moveHistory.begin() + currentStep, moveHistory.end());
                                moveHistory.push_back({ r, c, checkRes });
                                currentStep++;

                                switch (ProcessFinish(TestBoard())) {
                                case -1: case 1: case 0:
                                    isPaused = true;
                                    {
                                        lock_guard<mutex> lock(consoleMutex);
                                        PrintHudTextWithBg(TIMER_X, TIMER_Y, string(40, ' '), 15);
                                        PrintHudTextWithBg(TIMER_X, TIMER_Y + 1, string(40, ' '), 15);
                                    }

                                    HandleReplayOption();

                                    if (AskContinue() != 'Y') {
                                        isPlaying = false;
                                    }
                                    else {
                                        StartGame();
                                        timeLeft = turnTimeLimit;
                                        isPaused = false;
                                    }
                                    break;

                                case 2:
                                    timeLeft = turnTimeLimit;
                                    break;
                                }
                            }
                            validEnter = true;
                        }
                    }
                }


                Sleep(10);
            }

            StopTimerThread();
        }
    }

    return 0;
}
