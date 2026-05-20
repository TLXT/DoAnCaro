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
#include "CharSprite.h"   // <<< THÊM MỚI

using namespace std;

int main() {
    FixConsoleWindow();
    SetConsoleWindow(1000, 800);   // <<< Tăng chiều cao để chứa layout mới
    HideCursor();

    while (true) {
        int choice = MainMenu();
        bool isPlaying = false;

        if (choice == 0) {  // CHỌN: PLAY GAME
            int mode = PlayGameMenu();

            if (mode == 0) {  // Player vs Player
                _BOT_MODE = false;
                InputPlayerNames(false);
                CharacterSelectMenu();   // <<< THÊM MỚI: chọn nhân vật
                StartGame();
                isPlaying = true;
            }
            else if (mode == 1) {  // Player vs Bot
                int diff = DifficultyMenu();
                if (diff == 3) continue;

                _BOT_MODE = true;
                _BOT_DIFFICULTY = diff + 1;
                InputPlayerNames(true);
                CharacterSelectMenu();   // <<< THÊM MỚI: chọn nhân vật
                StartGame();
                isPlaying = true;
            }
            else if (mode == 2) {
                continue;
            }
        }
        else if (choice == 1) {  // CHỌN: LOAD GAME
            if (LoadGame() == true) {
                DrawBothSprites();       // <<< THÊM MỚI: vẽ lại sprite sau khi load
                isPlaying = true;
            }
        }
        else if (choice == 2) {  // CHỌN: SETTINGS
            while (true) {
                int setChoice = SettingsMenu();
                if (setChoice == 0) ClearAllData();
                else if (setChoice == 1) break;
            }
        }
        else if (choice == 3) {  // CHỌN: EXIT
            ExitGame();
            return 0;
        }

        // ============================================================
        //  VÒNG LẶP CHÍNH - GAMEPLAY
        // ============================================================
        if (isPlaying) {
            bool validEnter = true;
            StartTimerThread();  // Kích hoạt luồng đếm ngược

            while (isPlaying) {

                // --------------------------------------------------------
                // 1. XỬ LÝ TIMEOUT (HẾT GIỜ)
                // --------------------------------------------------------
                if (timeLeft <= 0) {
                    PlayRandomMove();
                    int finish = ProcessFinish(TestBoard());

                    if (finish != 2) {
                        isPaused = true;
                        {
                            lock_guard<mutex> lock(consoleMutex);
                            GotoXY(TIMER_X, TIMER_Y);     cout << "                                        ";
                            GotoXY(TIMER_X, TIMER_Y + 1); cout << "                                        ";
                        }

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
                            isPaused = false;
                        }
                    }
                    else {
                        timeLeft = TURN_TIME_LIMIT;
                    }
                    continue;
                }

                // --------------------------------------------------------
                // 2. LƯỢT CỦA BOT
                // --------------------------------------------------------
                if (_BOT_MODE == true && _TURN == false) {
                    isPaused = true;
                    {
                        lock_guard<mutex> lock(consoleMutex);
                        GotoXY(BOT_MSG_X, BOT_MSG_Y);
                        SetColor(12, 15);
                        cout << "Bot dang suy nghi...            ";
                    }

                    _POINT botMove = FindBotMove(1, _BOT_DIFFICULTY);

                    if (botMove.x != -1) {
                        DrawCell(_X, _Y, 15);
                        _X = botMove.x;
                        _Y = botMove.y;

                        int checkRes = CheckBoard(_X, _Y);
                        DrawCell(_X, _Y, 11);

                        int finishStatus;
                        {
                            lock_guard<mutex> lock(consoleMutex);

                            // Lưu lịch sử nước đi của bot
                            int r = (_Y - TOP - 1) / 2;
                            int c = (_X - LEFT - 2) / 4;
                            if (currentStep < (int)moveHistory.size())
                                moveHistory.erase(moveHistory.begin() + currentStep, moveHistory.end());
                            moveHistory.push_back({ r, c, checkRes });
                            currentStep++;

                            // Xóa thông báo "suy nghĩ"
                            GotoXY(BOT_MSG_X, BOT_MSG_Y);
                            SetColor(0, 15);
                            cout << "                                ";

                            // Kiểm tra thắng/thua trong lúc giữ mutex
                            finishStatus = ProcessFinish(TestBoard());
                            if (finishStatus != 2) {
                                GotoXY(TIMER_X, TIMER_Y);     cout << "                                        ";
                                GotoXY(TIMER_X, TIMER_Y + 1); cout << "                                        ";
                            }
                        }

                        // Xử lý kết thúc ván (ngoài mutex)
                        switch (finishStatus) {
                        case -1: case 1: case 0:
                            isPaused = true;
                            {
                                lock_guard<mutex> lock(consoleMutex);
                                GotoXY(TIMER_X, TIMER_Y);     cout << "                                        ";
                                GotoXY(TIMER_X, TIMER_Y + 1); cout << "                                        ";
                            }

                            {
                                char ch;
                                do {
                                    ch = _getch();
                                    if (ch == -32 || ch == 0) _getch();
                                } while (ch != 13);
                            }

                            HandleReplayOption();

                            if (AskContinue() != 'Y') {
                                isPlaying = false;
                            }
                            else {
                                StartGame();
                                timeLeft = TURN_TIME_LIMIT;
                            }
                            break;

                        case 2:  // Ván cờ tiếp tục
                            timeLeft = TURN_TIME_LIMIT;
                            break;
                        }

                        isPaused = false;
                        timeLeft = TURN_TIME_LIMIT;
                        continue;
                    }
                }

                // --------------------------------------------------------
                // 3. XỬ LÝ PHÍM BẤM CỦA NGƯỜI CHƠI
                // --------------------------------------------------------
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

                    // --- MỞ MENU PHỤ (M hoặc ESC) ---
                    if (_COMMAND == 'M' || _COMMAND == 27) {
                        isPaused = true;
                        future<int> SecondThreadchoice = async(launch::async, GameMenu);
                        int gamechoice = SecondThreadchoice.get();

                        if (gamechoice == 0) {
                            isPlaying = false;
                        }
                        else if (gamechoice == 1) {
                            SaveGame();
                            if (loadPresent()) {
                                DrawBothSprites();     // <<< Vẽ lại sprite sau save/load
                                timeLeft = TURN_TIME_LIMIT;
                            }
                        }
                        else if (gamechoice == 2) {
                            if (LoadGame()) {
                                DrawBothSprites();     // <<< Vẽ lại sprite sau load
                                timeLeft = TURN_TIME_LIMIT;
                            }
                            else {
                                loadPresent();
                                DrawBothSprites();     // <<< Vẽ lại sprite
                            }
                        }
                        else if (gamechoice == 3) {
                            loadPresent();
                            DrawBothSprites();         // <<< Vẽ lại sprite
                        }

                        isPaused = false;
                    }

                    // --- TẠM DỪNG / TIẾP TỤC (P) ---
                    else if (_COMMAND == 'P') {
                        isPaused = !isPaused;
                        lock_guard<mutex> lock(consoleMutex);
                        GotoXY(TIMER_X, TIMER_Y + 1);
                        if (isPaused) {
                            SetColor(14, 0);
                            cout << " >>> DANG TAM DUNG (PAUSED) <<< ";
                            SetColor(0, 15);
                        }
                        else {
                            SetColor(10, 15);
                            cout << "      DANG CHOI (PLAYING)       ";
                        }
                        GotoXY(_X, _Y);
                    }

                    // --- CÁC PHÍM DI CHUYỂN / ĐÁNH CỜ (chỉ khi đang chơi) ---
                    else if (!isPaused) {
                        if (_COMMAND == 'Z') {
                            if (!_BOT_MODE || _TURN) { UndoMove(); timeLeft = TURN_TIME_LIMIT; }
                        }
                        else if (_COMMAND == 'Y') {
                            if (!_BOT_MODE || _TURN) { RedoMove(); timeLeft = TURN_TIME_LIMIT; }
                        }
                        else if (_COMMAND == 'A' || _COMMAND == 75) MoveLeft();
                        else if (_COMMAND == 'W' || _COMMAND == 72) MoveUp();
                        else if (_COMMAND == 'S' || _COMMAND == 80) MoveDown();
                        else if (_COMMAND == 'D' || _COMMAND == 77) MoveRight();

                        else if (_COMMAND == 13) {  // ENTER: Đánh cờ
                            int checkRes = CheckBoard(_X, _Y);
                            switch (checkRes) {
                            case -1: DrawCell(_X, _Y, 11); break;
                            case  1: DrawCell(_X, _Y, 11); break;
                            case  0: validEnter = false;   break;
                            }

                            if (validEnter == true) {
                                // Lưu lịch sử nước đi người chơi
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
                                        GotoXY(TIMER_X, TIMER_Y);     cout << "                                        ";
                                        GotoXY(TIMER_X, TIMER_Y + 1); cout << "                                        ";
                                    }

                                    {
                                        char ch_player;
                                        do {
                                            ch_player = _getch();
                                            if (ch_player == -32 || ch_player == 0) _getch();
                                        } while (ch_player != 13);
                                    }

                                    HandleReplayOption();

                                    if (AskContinue() != 'Y') {
                                        isPlaying = false;
                                    }
                                    else {
                                        StartGame();
                                        timeLeft = TURN_TIME_LIMIT;
                                        isPaused = false;
                                    }
                                    break;

                                case 2:  // Ván cờ tiếp tục
                                    timeLeft = TURN_TIME_LIMIT;
                                    break;
                                }
                            }
                            validEnter = true;
                        }
                    }
                }

                // Giải phóng CPU - tránh vòng lặp nuốt 100% CPU
                Sleep(10);
            }

            StopTimerThread();
        }
    }

    return 0;
}
