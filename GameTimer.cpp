#include "GameTimer.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "Menu.h"
#include "UserInfo.h"
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
            lock_guard<mutex> lock(consoleMutex);
            int curX = _X, curY = _Y;

            PrintTextWithBg(TIMER_X, TIMER_Y, "THOI GIAN: " + to_string(timeLeft.load()) + "s      ", (timeLeft <= 5) ? 12 : 11);

            if (isPaused) {
                DrawStatusInfo(true);
            }
            else {
                DrawStatusInfo(false);
            }

            GotoXY(curX, curY); // Trả lại con trỏ cho người chơi
        }
    }
}

static std::thread timerThread;

void StartTimerThread() {
    if (isTimerRunning) return;
    // Join any old thread first
    if (timerThread.joinable()) timerThread.join();
    isTimerRunning = true;
    timeLeft = TURN_TIME_LIMIT;
    isPaused = false;
    {
        lock_guard<mutex> lock(consoleMutex);
        PrintTextWithBg(TIMER_X, TIMER_Y, "THOI GIAN: " + to_string(timeLeft.load()) + "s      ", 11);
        DrawStatusInfo(false);
        GotoXY(_X, _Y);
    }
    timerThread = std::thread(TimerLogic);
}

void StopTimerThread() {
    isTimerRunning = false;
    if (timerThread.joinable()) timerThread.join();
    {
        lock_guard<mutex> lock(consoleMutex);
        GotoXY(TIMER_X, TIMER_Y); cout << "                    ";
        GotoXY(TURN_INFO_X, TURN_INFO_Y); cout << "              ";
        GotoXY(STATUS_X, STATUS_Y); cout << "                              ";
        GotoXY(BOT_MSG_X, BOT_MSG_Y); cout << "                                  ";
    }
}
