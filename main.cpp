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

// =========================================================
// CÁC HÀM CẤU HÌNH CONSOLE TỪ MAIN CŨ (RẤT QUAN TRỌNG ĐỂ VẼ MÀU)
// =========================================================
void EnableRGBColor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(hOut, dwMode);
}

void SetConsoleFontSize(int width, int height) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = width;  // Chiều rộng pixel của 1 ký tự
    cfi.dwFontSize.Y = height; // Chiều cao pixel của 1 ký tự
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas"); // Font Consolas hỗ trợ UTF-8 tốt
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

// =========================================================
// HÀM MAIN CHÍNH
// =========================================================
int main() {
    // 1. Khởi tạo môi trường đồ họa cho Console
    EnableRGBColor();
    SetConsoleOutputCP(CP_UTF8); // Bật Font UTF-8 để vẽ box và text tiếng Việt
    SetConsoleFontSize(12, 16);

    FixConsoleWindow();

    // Đảm bảo lưới Console đủ 120 cột và 50 dòng để chứa được giao diện mới (800px)
    system("MODE CON COLS=120 LINES=40");
    ConfigureConsoleSize(CONSOLE_COLS, CONSOLE_LINES);
    SetConsoleWindow(1460, 730);
    HideCursor();

    while (true) {
        int choice = MainMenu();
        bool isPlaying = false;

        if (choice == 0) {  // CHỌN: PLAY GAME
            int mode = PlayGameMenu();

            if (mode == 0) {  // Player vs Player
                _BOT_MODE = false;
                InputPlayerNames(false);
                StartGame();
                isPlaying = true;
            }
            else if (mode == 1) {  // Player vs Bot
                int diff = DifficultyMenu();
                if (diff == 3) continue;

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
        else if (choice == 1) {  // CHỌN: LOAD GAME
            if (LoadGame() == true) {
                ingamedisplay(CharacterASelect, true);
                ingamedisplay(CharacterBSelect, false);
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
                    loadBotMove(isPlaying);
                    continue;
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
                                ingamedisplay(CharacterASelect, true);
                                ingamedisplay(CharacterBSelect, false);
                                timeLeft = TURN_TIME_LIMIT;
                            }
                        }
                        else if (gamechoice == 2) {
                            if (LoadGame()) {
                                ingamedisplay(CharacterASelect, true);
                                ingamedisplay(CharacterBSelect, false);
                                timeLeft = TURN_TIME_LIMIT;
                            }
                            else {
                                loadPresent();
                                ingamedisplay(CharacterASelect, true);
                                ingamedisplay(CharacterBSelect, false);
                            }
                        }
                        else if (gamechoice == 3) {
                            loadmusic();
                            loadPresent();
                        }
                        else if (gamechoice == 4) {
                            loadPresent();
                            ingamedisplay(CharacterASelect, true);
                            ingamedisplay(CharacterBSelect, false);
                        }

                        isPaused = false;
                    }

                    // --- TẠM DỪNG / TIẾP TỤC (P) ---
                    else if (_COMMAND == 'P') {
                        isPaused = !isPaused;
                        lock_guard<mutex> lock(consoleMutex);
                        DrawStatusInfo(isPaused);
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
                            case -1: DrawCell(_X, _Y, BOARD_CURSOR_COLOR); break;
                            case  1: DrawCell(_X, _Y, BOARD_CURSOR_COLOR); break;
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
