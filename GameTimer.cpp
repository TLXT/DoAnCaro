#include "GameTimer.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include <chrono>
#include <string>

atomic<int> timeLeft(TURN_TIME_LIMIT);
atomic<bool> isPaused(false);
atomic<bool> isTimerRunning(false);

std::string _PLAYER1_NAME = "Player 1";
std::string _PLAYER2_NAME = "Player 2";

void TimerLogic() {
    while (isTimerRunning) {
        // Chia nhỏ 1 giây thành 10 lần 100ms
        for (int i = 0; i < 10; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (!isTimerRunning) return;
        }

        if (!isPaused && timeLeft > 0) {
            timeLeft--;
            {
                lock_guard<mutex> lock(consoleMutex);
                int curX = _X, curY = _Y;

                // Xóa vùng cũ trước khi in số mới để tránh bị lem chữ
                GotoXY(60, TOP + 21); cout << "                             ";

                GotoXY(60, TOP + 21);
                if (timeLeft <= 5) SetColor(12, 15);
                else SetColor(0, 15);
                cout << "THOI GIAN CON LAI: " << (int)timeLeft << "s";

                // In trạng thái Pause/Play
                GotoXY(60, TOP + 22);
                if (isPaused) {
                    SetColor(14, 0);
                    cout << " >>> DANG TAM DUNG (PAUSED) <<< ";
                }
                else {
                    SetColor(10, 15);
                    cout << "      DANG CHOI (PLAYING)       ";
                }
                GotoXY(curX, curY);
            }
        }
    }
}

void StartTimerThread() {
    if (isTimerRunning) return;
    isTimerRunning = true;
    timeLeft = TURN_TIME_LIMIT;
    isPaused = false;
    thread t(TimerLogic);
    t.detach(); // Chạy độc lập với luồng chính
}

void StopTimerThread() {
    isTimerRunning = false;
    this_thread::sleep_for(chrono::milliseconds(150));
    {
        lock_guard<mutex> lock(consoleMutex);
        GotoXY(60, TOP + 21); cout << "                             ";
        GotoXY(60, TOP + 22); cout << "                             ";
        GotoXY(60, TOP + 23); cout << "                             ";
    }
}