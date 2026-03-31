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

                        //lưu lịch sử di chuyển của bot. Edit: thêm xóa lịch sử cũ khi đã đánh nước mới
                        int r = (_Y - TOP - 1) / 2;
                        int c = (_X - LEFT - 2) / 4;
                        if (currentStep < (int)moveHistory.size()) 
                            moveHistory.erase(moveHistory.begin() + currentStep, moveHistory.end()); // thêm
                        moveHistory.push_back({ r, c, checkRes }); 
                        currentStep++;

                        GotoXY(60, 20);
                        SetColor(0, 15);
                        cout << "                             ";

                        switch (ProcessFinish(TestBoard())) {
                        case -1: case 1: case 0:
                            //nhấn enter xong sẽ xuất hiện replay
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
                if (_COMMAND == 27) { // Phím ESC
                    isPlaying = false;
                }
                else if (_COMMAND == 'L') { SaveGame(); }
                else if (_COMMAND == 'T') {
                    if (LoadGame() == false) {
                        system("cls"); DrawBoard(BOARD_SIZE); DrawPlayerInfo(); UpdateTurnInfo();
                        for (int i = 0; i < BOARD_SIZE; i++)
                            for (int j = 0; j < BOARD_SIZE; j++)
                                DrawCell(_A[i][j].x, _A[i][j].y, 15);
                        DrawCell(_X, _Y, 11);
                    }
                }

                else if (_COMMAND == 'Z') { if (!_BOT_MODE || _TURN) UndoMove(); }
                else if (_COMMAND == 'Y') { if (!_BOT_MODE || _TURN) RedoMove(); }

                else {
                    if (_COMMAND == 'A' || _COMMAND == 75) MoveLeft();
                    else if (_COMMAND == 'W' || _COMMAND == 72) MoveUp();
                    else if (_COMMAND == 'S' || _COMMAND == 80) MoveDown();
                    else if (_COMMAND == 'D' || _COMMAND == 77) MoveRight();
                    else if (_COMMAND == 13) {
                        int checkRes = CheckBoard(_X, _Y);
                        switch (checkRes) {
                        case -1: DrawCell(_X, _Y, 11); break;
                        case 1:  DrawCell(_X, _Y, 11); break;
                        case 0:  validEnter = false;
                        }

                        if (validEnter == true) {
                            //lưu lịch sử di chuyển của người chơi. Edit: thêm xóa lịch sử cũ sau khi đã đánh nước mới 
                            int r = (_Y - TOP - 1) / 2;
                            int c = (_X - LEFT - 2) / 4;
                            if (currentStep < (int)moveHistory.size())         
                                moveHistory.erase(moveHistory.begin() + currentStep, moveHistory.end());  // thêm
                            moveHistory.push_back({ r, c, checkRes });
                            currentStep++;

                            switch (ProcessFinish(TestBoard())) {
                            case -1: case 1: case 0:
                                //nhấn enter tới menu replay
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
                                }
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
