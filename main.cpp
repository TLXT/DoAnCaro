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
#include "Replay.h"

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
                InputPlayerNames(false);
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
                InputPlayerNames(true);

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
                    isPaused = true;
                    {
                        lock_guard<mutex>lock(consoleMutex);
                        GotoXY(60, 20);
                        SetColor(12, 15);
                        cout << "Bot dang suy nghi...         ";
                    }

                    _POINT botMove = FindBotMove(1, _BOT_DIFFICULTY);

                    if (botMove.x != -1) {
                        DrawCell(_X, _Y, 15);
                        _X = botMove.x;
                        _Y = botMove.y;

                        int checkRes = CheckBoard(_X, _Y); //lưu giá trị c
                        DrawCell(_X, _Y, 11);
                        {
                            lock_guard<mutex>lock(consoleMutex);

                            //lưu lịch sử di chuyển của bot
                            int r = (_Y - TOP - 1) / 2;
                            int c = (_X - LEFT - 2) / 4;
                            if (currentStep < (int)moveHistory.size())
                                moveHistory.erase(moveHistory.begin() + currentStep, moveHistory.end());
                            moveHistory.push_back({ r, c, checkRes });
                            currentStep++;

                            GotoXY(60, 20);
                            SetColor(0, 15);
                            cout << "                             ";

                            switch (ProcessFinish(TestBoard())) {
                            case -1: case 1: case 0:
                                isPaused = true;
                                GotoXY(60, TOP + 21); cout << "                                ";
                                GotoXY(60, TOP + 22); cout << "                                ";

                                char ch;
                                do {
                                    ch = _getch();
                                    if (ch == -32 || ch == 0) _getch();
                                } while (ch != 13);

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
                        isPaused = false;
                        timeLeft = TURN_TIME_LIMIT;
                        continue;
                    }
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
                            string temp = SaveGame();
                            if (LoadGame(temp)) { moveHistory.clear(); currentStep = 0; timeLeft = TURN_TIME_LIMIT; }
                        }
                        else if (gamechoice == 2) {
                            string filename = ChooseFileMenu();
                            if (LoadGame(filename)) { moveHistory.clear(); currentStep = 0; timeLeft = TURN_TIME_LIMIT; }
                        }
                        else if (gamechoice == 4) { loadPresent(); }

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
                        }
                        else {
                            SetColor(10, 15);
                            cout << "      DANG CHOI (PLAYING)       ";
                        }
                        GotoXY(_X, _Y);
                    }
                    // --- CÁC PHÍM CHỈ HOẠT ĐỘNG KHI TẠM DỪNG ---
                    //else if (isPaused) {
                    //    if (_COMMAND == 27) { // ESC: Thoát
                    //        isPlaying = false;
                    //    }
                    //    else if (_COMMAND == 'L') { SaveGame(); }
                    //    else if (_COMMAND == 'T') {
                    //        if (LoadGame() == true) {
                    //            moveHistory.clear(); currentStep = 0; timeLeft = TURN_TIME_LIMIT;
                    //        }
                    //        else {
                    //            system("cls"); DrawBoard(BOARD_SIZE); DrawPlayerInfo(); UpdateTurnInfo();
                    //            for (int i = 0; i < BOARD_SIZE; i++)
                    //                for (int j = 0; j < BOARD_SIZE; j++)
                    //                    DrawCell(_A[i][j].x, _A[i][j].y, 15);
                    //            DrawCell(_X, _Y, 11);
                    //        }
                    //    }
                    //}
                    // --- CÁC PHÍM DI CHUYỂN / ĐÁNH CỜ (CHỈ KHI ĐANG CHƠI) ---
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