#include"Proccesor.h"
void loadmusic() {
    while (true) {
        int musicChoice = MusicMenu();
        if (musicChoice == 6) {
            VolumeMenu();//cập nhật biến volumeLevel
            if (MusicStatus())//kiểm tra an toàn trước khi phát nhạc
                setVolume();
        }
        if (musicChoice == 7) break;
        if (musicChoice >= 0 && musicChoice <= 5) {
            PlayMusic(musicChoice, volumeLevel);
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
            break;
        }
    }
}
void loadBotMove(bool& isPlaying){
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

            GotoXY(60, 20);
            SetColor(0, 15);
            cout << "                             ";
            // Kiểm tra trạng thái ván cờ ngay trong lúc khóa luồng
            finishStatus = ProcessFinish(TestBoard());
            if (finishStatus != 2) {
                GotoXY(60, TOP + 21); cout << "                                ";
                GotoXY(60, TOP + 22); cout << "                                ";
            }
        } // <-- GIẢI PHÓNG MUTEX TẠI ĐÂY

            // Xử lý Replay bên ngoài khóa mutex
        switch (finishStatus) {
		case -1: case 1: case 0: // Ván cờ kết thúc, hỏi người chơi có muốn xem lại không
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

        isPaused = false;
        timeLeft = TURN_TIME_LIMIT;
    }
}
bool loadGameMenu(bool& isPlaying) {
    int mode = PlayGameMenu();

    if (mode == 0) {
        _BOT_MODE = false;
        InputPlayerNames(false);
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
        InputPlayerNames(true);

        StartGame();
        isPlaying = true;
        return true;
    }
    else if (mode == 2) {
        return false;
    }
}