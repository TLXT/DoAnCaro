#include"Proccesor.h"
#include "Language.h"

void loadmusic() {
    while (true) {
        int choice = MusicMenu();
        if (choice >= 0 && choice <= 4) {
            PlayMusic(choice, volumeLevel);
        }
        else if (choice == 5) {
            PlayMusic(5, 0);
        }
        else if (choice == 6) {
            VolumeMenu();
            setVolume();
        }
        else if (choice == 7) {
            // 8. Sound Effect: [ON/OFF]
            isSFXOn = !isSFXOn;
        }
        else if (choice == 8) {
            break;
        }
    }
}
void loadSettingMenu() {
    while (true) {
        int setChoice = SettingsMenu();

        if (setChoice == 0) {
            ClearAllData();
        }
        else if (setChoice == 1) {
            loadmusic();
        }
        else if (setChoice == 2) {
            ToggleLanguage();
            PlayMenuSound();
        }
        else if (setChoice == 3) {
            break;
        }
    }
}
void loadBotMove(bool& isPlaying){
    isPaused = true;
    {
        lock_guard<mutex>lock(consoleMutex);
        PrintHudTextWithBg(BOT_MSG_X, BOT_MSG_Y, L(TextId::BotThinking) + "              ", 12);
    }

    _POINT botMove = FindBotMove(1, _BOT_DIFFICULTY);

    if (botMove.x != -1) {
        DrawCell(_X, _Y, BOARD_BG_COLOR);
        _X = botMove.x;
        _Y = botMove.y;

        int checkRes = CheckBoard(_X, _Y); //lưu giá trị c
        DrawCell(_X, _Y, BOARD_CURSOR_COLOR);
        int finishStatus; // Biến tạm lưu kết quả kiểm tra thắng thua
        {
            lock_guard<mutex>lock(consoleMutex);

            //lưu lịch sử di chuyển của bot
            int r = (_Y - TOP - 1) / 2;
            int c = (_X - LEFT - 2) / 4;
            if (currentStep < (int)moveHistory.size())
                moveHistory.erase(moveHistory.begin() + currentStep, moveHistory.end());
            moveHistory.push_back({ r, c, checkRes });
            currentStep++;

            PrintHudTextWithBg(BOT_MSG_X, BOT_MSG_Y, "                                  ", 15);
        } // <-- GIẢI PHÓNG MUTEX TẠI ĐÂY

        finishStatus = ProcessFinish(TestBoard());
        if (finishStatus != 2) {
            lock_guard<mutex>lock(consoleMutex);
            PrintHudTextWithBg(TIMER_X, TIMER_Y, string(40, ' '), 15);
            PrintHudTextWithBg(TIMER_X, TIMER_Y + 1, string(40, ' '), 15);
        }

            // Xử lý Replay bên ngoài khóa mutex
        switch (finishStatus) {
		case -1: case 1: case 0: // Ván cờ kết thúc, hỏi người chơi có muốn xem lại không
            isPaused = true;
            PrintHudTextWithBg(TIMER_X, TIMER_Y, string(40, ' '), 15);
            PrintHudTextWithBg(TIMER_X, TIMER_Y + 1, string(40, ' '), 15);

            HandleReplayOption();
            if (AskContinue() != 'Y') {
                isPlaying = false;
            }
            else {
                StartGame();
                timeLeft = turnTimeLimit;
            }
            break;
        case 2: // Ván cờ vẫn tiếp tục, chuyển lượt
            timeLeft = turnTimeLimit; // Đặt lại đồng hồ cho người tiếp theo
            break;
        }

        isPaused = false;
        timeLeft = turnTimeLimit;
    }
    else {
        isPaused = false;
        ProcessFinish(TestBoard());
    }
}
bool loadGameMenu(bool& isPlaying) {
    int mode = PlayGameMenu();

    if (mode == 0) {
        _BOT_MODE = false;
        if (!InputPlayerNames(false)) return false;
        StartGame();
        isPlaying = true;
        return true;
    }
    else if (mode == 1) {
        int diff = DifficultyMenu();
        if (diff == 3) {
            return false;
        }

        _BOT_MODE = true;
        _BOT_DIFFICULTY = diff + 1;
        if (!InputPlayerNames(true)) return false;

        StartGame();
        isPlaying = true;
        return true;
    }
    else if (mode == 2) {
        return false;
    }
    return false;
}
