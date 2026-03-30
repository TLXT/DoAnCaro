#include <iostream>
#include <conio.h>
#include <Windows.h>
#include<future>

#include "GameStatus.h"
#include "ControlConsole.h"
#include "GamePlay.h"
#include "FinishProcess.h"
#include "Menu.h"
#include "SaveLoad.h"
#include "UserInfo.h"
#include "DrawBoard.h"
#include "CaroBot.h"
#include"GameTimer.h"

using namespace std;

int main() {
    FixConsoleWindow();
    SetConsoleWindow(1000, 600);
    HideCursor();

    while (true) {
        int choice = MainMenu();
        bool isPlaying = false;
        if (choice == 0) { // CHỌN: PLAY GAME
            int mode = PlayGameMenu();

            if (mode == 0) {
                _BOT_MODE = false;
                StartGame();
                isPlaying = true;
            }
            else if (mode == 1) {
                int diff = DifficultyMenu();
                if (diff == 3) {
                    continue;
                }

                _BOT_MODE = true;
                _BOT_DIFFICULTY = diff + 1;

                StartGame();
                isPlaying = true;
            }
            else if (mode == 2) {

                continue;
            }
        }
        else if (choice == 1) { // CHỌN: LOAD GAME
            if (LoadGame() == true) {
                isPlaying = true;
            }
        }
        else if (choice == 2) { // CHỌN: SETTINGS
            while (true) {
                int setChoice = SettingsMenu();

                if (setChoice == 0) {
                    ClearAllData();
                }
                else if (setChoice == 1) {
                    break;
                }
            }
        }
        else if (choice == 3) { // CHỌN: EXIT
            ExitGame();
            return 0;
        }

        if (isPlaying) {
            bool validEnter = true;
            StartTimerThread();//kích hoạt luồng đếm ngược 
            while (isPlaying) {
                // Xử lý timeout
                if (timeLeft <= 0) {
                    PlayRandomMove();

                    // Kiểm tra thắng thua sau khi hệ thống tự đánh
                    int finish = ProcessFinish(TestBoard());
                    if (finish != 2) {
                        if (AskContinue() != 'Y') {
                            isPlaying = false;
                        }
                        else {
                            StartGame();
                            timeLeft = TURN_TIME_LIMIT;
                        }
                    }
                    else {
                        timeLeft = TURN_TIME_LIMIT;
                    }
                    continue;
                }
                if (_BOT_MODE == true && _TURN == false) {

                    isPaused = true;
                    {
                        lock_guard<mutex>lock(consoleMutex);
                        GotoXY(60, 18);
                        SetColor(12, 15);
                        cout << "Bot dang suy nghi...         ";
                    }
                    _POINT botMove = FindBotMove(1, _BOT_DIFFICULTY);

                    if (botMove.x != -1) {
                        DrawCell(_X, _Y, 15);
                        _X = botMove.x;
                        _Y = botMove.y;

                        CheckBoard(_X, _Y);
                        DrawCell(_X, _Y, 11);
                        {
                            lock_guard<mutex>lock(consoleMutex);
                            GotoXY(60, 18);
                            SetColor(0, 15);
                            cout << "                             ";
                        }
                        switch (ProcessFinish(TestBoard())) {
                        case -1: case 1: case 0:
                            if (AskContinue() != 'Y') {
                                isPlaying = false;
                            }
                            else {
                                StartGame();
                            }

                        }
                    }
                    isPaused = false;
                    timeLeft = TURN_TIME_LIMIT;
                    continue;
                }

                if (_kbhit()) {
                    _COMMAND = toupper(_getch());
                    if (_COMMAND == 'M' || _COMMAND == 77) {
                        isPaused = true;
                        future<int> SecondThreadchoice = async(launch::async, GameMenu);
                        int gamechoice = SecondThreadchoice.get();
                        if (gamechoice == 0) { // thoat
                            isPlaying = false;
                        }
                        else if (gamechoice == 1) {
                            string temp = SaveGame();
                            LoadGame(temp);
                        }
                        else if (gamechoice == 2) {
                            string filename = ChooseFileMenu();
                            LoadGame(filename);
                        }
                        else if (gamechoice == 3) {
                            //setting;
                        }
                        else if (gamechoice == 4) {
                            loadPresent();
                        }
                        isPaused = false;
                    }
                    else if (_COMMAND == 'P') { // Tạm dừng/Tiếp tục
                        isPaused = !isPaused;
                        {
                            lock_guard<mutex> lock(consoleMutex);
                            int oldX = _X, oldY = _Y;
                            GotoXY(60, TOP + 19); // Tọa độ dòng trạng thái
                            if (isPaused) {
                                SetColor(14, 0);
                                cout << " >>> DANG TAM DUNG (PAUSED) <<< ";
                            }
                            else {
                                SetColor(10, 15);
                                cout << "      DANG CHOI (PLAYING)       ";
                            }
                            GotoXY(oldX, oldY);
                        }
                    }
                    else if (!isPaused) {
                        ProcessMove(_COMMAND, validEnter, isPlaying);
                    }
                }

            }
            StopTimerThread();
        }
    }
    return 0;
}