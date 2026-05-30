#include "GameTimer.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "Menu.h"
#include "UserInfo.h"
#include "Language.h"
#include "Sound.h"
#include <chrono>
#include <string>

int turnTimeLimit = DEFAULT_TURN_TIME_LIMIT;
atomic<int> timeLeft(DEFAULT_TURN_TIME_LIMIT);
atomic<bool> isPaused(false);
atomic<bool> isTimerRunning(false);

std::string _PLAYER1_NAME = "Player 1";
std::string _PLAYER2_NAME = "Player 2";

void TimerLogic() {
    while (isTimerRunning) {

        for (int i = 0; i < 10; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (!isTimerRunning) return;
        }

        if (!isPaused && timeLeft > 0) {
            int remaining = timeLeft.fetch_sub(1) - 1;
            if (remaining > 0 && remaining <= 5) {
                PlayCountdownBeep();
            }

            lock_guard<mutex> lock(consoleMutex);
            int curX = _X, curY = _Y;

            DrawTurnTimers(remaining);

            if (isPaused) {
                DrawStatusInfo(true);
            }
            else {
                DrawStatusInfo(false);
            }

            GotoXY(curX, curY);
        }
    }
}

static std::thread timerThread;

void StartTimerThread() {
    if (isTimerRunning) return;

    if (timerThread.joinable()) timerThread.join();
    isTimerRunning = true;
    timeLeft = turnTimeLimit;
    isPaused = false;
    {
        lock_guard<mutex> lock(consoleMutex);
        DrawTurnTimers(timeLeft.load());
        DrawStatusInfo(false);
        GotoXY(_X, _Y);
    }
    timerThread = std::thread(TimerLogic);
}

void StopTimerThread() {
    isTimerRunning = false;
    if (timerThread.joinable()) timerThread.join();
}
