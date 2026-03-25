#include <iostream>
#include <conio.h>
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
            StartTimerThread(); // Kích hoạt luồng đếm ngược và menu đa luồng

            while (isPlaying) {
                // Xử lý timeout
                if (timeLeft <= 0) {
                    /*{
                        lock_guard<mutex> lock(consoleMutex);
                        GotoXY(LEFT, TOP + 20);
                        SetColor(12, 15);
                        cout << "HET GIO! DANG TU DONG DANH...   ";
                    }*/

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
                        lock_guard<mutex> lock(consoleMutex);
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
                            lock_guard<mutex> lock(consoleMutex);
                            GotoXY(60, 18);
                            SetColor(0, 15);
                            cout << "                         ";
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
                    isPaused = false; // Bot đánh xong, mở lại đồng hồ
                    timeLeft = TURN_TIME_LIMIT; // Reset thời gian cho người chơi
                    //continue;
                }

                if (_kbhit()) {
                    _COMMAND = toupper(_getch());

                    if (_COMMAND == 27) { // Phím ESC
                        isPlaying = false;
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
                    else if (_COMMAND == 'L') {
                        SaveGame();
                    }
                    else if (_COMMAND == 'T') {
                        if (LoadGame() == false) {
                            system("cls"); DrawBoard(BOARD_SIZE); DrawPlayerInfo(); UpdateTurnInfo();
                            for (int i = 0; i < BOARD_SIZE; i++)
                                for (int j = 0; j < BOARD_SIZE; j++)
                                    DrawCell(_A[i][j].x, _A[i][j].y, 15);
                            DrawCell(_X, _Y, 11);
                        }
                    }
                    else if (!isPaused) { // Chỉ cho phép di chuyển/đánh khi KHÔNG bị Pause
                        if (_COMMAND == 'A' || _COMMAND == 75) MoveLeft();
                        else if (_COMMAND == 'W' || _COMMAND == 72) MoveUp();
                        else if (_COMMAND == 'S' || _COMMAND == 80) MoveDown();
                        else if (_COMMAND == 'D' || _COMMAND == 77) MoveRight();
                        else if (_COMMAND == 13) { // Phím ENTER
                            switch (CheckBoard(_X, _Y)) {
                            case -1: DrawCell(_X, _Y, 11); break;
                            case 1: DrawCell(_X, _Y, 11); break;
                            case 0:
                                validEnter = false;
                                break;
                            }

                            if (validEnter == true) {
                                // Đánh thành công -> Reset đồng hồ về lại ban đầu
                                timeLeft = TURN_TIME_LIMIT;

                                switch (ProcessFinish(TestBoard())) {
                                case -1: case 1: case 0:
                                    if (AskContinue() != 'Y') {
                                        isPlaying = false;
                                    }
                                    else {
                                        StartGame();
                                        timeLeft = TURN_TIME_LIMIT; // Reset cho ván mới
                                    }
                                    break;
                                }
                            }
                            validEnter = true; // Reset biến cờ hiệu cho lần bấm tiếp theo
                        }
                    }
                }
            }
            StopTimerThread(); // Dừng luồng khi thoát khỏi vòng lặp chơi
        }
    }
    return 0;
}   