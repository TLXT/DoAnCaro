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
#include"Sound.h"
#include"Proccesor.h"

using namespace std;

int main() {
    FixConsoleWindow();
    SetConsoleWindow(1000, 600);
    HideCursor();

    while (true) {
        int choice = MainMenu();
        bool isPlaying = false;
        if (choice == 0) { // CHỌN: PLAY GAME
            if (!loadGameMenu(isPlaying)) continue;
        }
        else if (choice == 1) { // CHỌN: LOAD GAME
            if (LoadGame() == true) {
                isPlaying = true;
            }
        }
        else if (choice == 2) { // CHỌN: SETTINGS
            loadSettingMenu();
        }
        else if (choice == 3) {
            loadmusic();
        }
        else if (choice == 4) { // CHỌN: EXIT
            ExitGame();
            return 0;
        }

        if (isPlaying) {
            bool validEnter = true;
            StartTimerThread(); // kích hoạt luồng đếm ngược 

            while (isPlaying) {
                // 1. XỬ LÝ TIMEOUT (HẾT GIỜ)
                if (timeLeft <= 0) {
                    PlayRandomMove();
                    int finish = ProcessFinish(TestBoard());
                    if (finish != 2) {
                        isPaused = true;
                        {
                            lock_guard<mutex> lock(consoleMutex);
                            GotoXY(60, TOP + 21); cout << "                                ";
                            GotoXY(60, TOP + 22); cout << "                                ";
                        }

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

                // 2. LƯỢT CỦA BOT
                if (_BOT_MODE == true && _TURN == false) {
                    loadBotMove(isPlaying);
                    continue;
                }

                // 3. XỬ LÝ PHÍM BẤM CỦA NGƯỜI CHƠI
                if (_kbhit()) {
                    int ch = _getch();
                    if (ch == 0 || ch == 224) {
                        ch = _getch();
                        if (ch == 72) _COMMAND = 'W';      // Lên
                        else if (ch == 80) _COMMAND = 'S'; // Xuống
                        else if (ch == 75) _COMMAND = 'A'; // Trái
                        else if (ch == 77) _COMMAND = 'D'; // Phải
                        else _COMMAND = 0;
                    }
                    else {
                        _COMMAND = toupper(ch);
                    }

                    if (_COMMAND == 0) continue;

                    // --- MỞ MENU PHỤ ---
                    if (_COMMAND == 'M' || _COMMAND == 27) {
                        isPaused = true;
                        future<int> SecondThreadchoice = async(launch::async, GameMenu);
                        int gamechoice = SecondThreadchoice.get();

                        if (gamechoice == 0) { isPlaying = false; }
                        else if (gamechoice == 1) {
                            SaveGame();
                            if (loadPresent()) { timeLeft = TURN_TIME_LIMIT; }
                        }
                        else if (gamechoice == 2) {
                            if (LoadGame()) { timeLeft = TURN_TIME_LIMIT; }
                            else {
                                loadPresent();
                            }
                        }
                        else if (gamechoice == 3) {
                            loadmusic();
                        }
                        else if (gamechoice == 4) {
                            loadPresent();
                        }
                        isPaused = false;
                    }
                    // --- TẠM DỪNG / TIẾP TỤC ---
                    else if (_COMMAND == 'P') {
                        isPaused = !isPaused;
                        lock_guard<mutex> lock(consoleMutex);
                        GotoXY(60, TOP + 21);
                        if (isPaused) {
                            SetColor(14, 0);
                            cout << " >>> DANG TAM DUNG (PAUSED) <<< ";
                            SetColor(0, 15);
                            GotoXY(60, TOP + 22);
                            cout << "                               ";
                        }
                        else {
                            SetColor(10, 15);
                            cout << "      DANG CHOI (PLAYING)       ";
                        }
                        GotoXY(_X, _Y);
                    }
                    else if (!isPaused) {
                        if (_COMMAND == 'Z') {
                            if (!_BOT_MODE || _TURN) { UndoMove(); timeLeft = TURN_TIME_LIMIT; } // Thêm reset thời gian
                        }
                        else if (_COMMAND == 'Y') {
                            if (!_BOT_MODE || _TURN) { RedoMove(); timeLeft = TURN_TIME_LIMIT; } // Thêm reset thời gian
                        }
                        else if (_COMMAND == 'A' || _COMMAND == 75) MoveLeft();
                        else if (_COMMAND == 'W' || _COMMAND == 72) MoveUp();
                        else if (_COMMAND == 'S' || _COMMAND == 80) MoveDown();
                        else if (_COMMAND == 'D' || _COMMAND == 77) MoveRight();
                        else if (_COMMAND == 13) { // ENTER
                            int checkRes = CheckBoard(_X, _Y);
                            switch (checkRes) {
                            case -1: DrawCell(_X, _Y, 11); break;
                            case 1:  DrawCell(_X, _Y, 11); break;
                            case 0:  validEnter = false; break;
                            }

                            if (validEnter == true) {
                                // Lưu lịch sử di chuyển người chơi
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
                                        GotoXY(60, TOP + 21); cout << "                                ";
                                        GotoXY(60, TOP + 22); cout << "                                ";
                                    }

                                    char ch_player;
                                    do {
                                        ch_player = _getch();
                                        if (ch_player == -32 || ch_player == 0) _getch();
                                    } while (ch_player != 13);

                                    HandleReplayOption();

                                    if (AskContinue() != 'Y') {
                                        isPlaying = false;
                                    }
                                    else {
                                        StartGame();
										timeLeft = TURN_TIME_LIMIT;
                                    }
                                    break;

								case 2: // Ván cờ vẫn tiếp tục, chuyển lượt
									timeLeft = TURN_TIME_LIMIT; // Đặt lại đồng hồ về 30 giây cho người tiếp theo
									break;
                                }
                            }
                            validEnter = true;
                        }
                    }
                }

                // GIẢI PHÓNG CPU - Ngăn vòng lặp while nuốt 100% CPU gây đơ game
                Sleep(10);
            }
            StopTimerThread();
        }
    }
    return 0;
}