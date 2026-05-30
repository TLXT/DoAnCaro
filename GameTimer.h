#ifndef _GAMETIMER_H_
#define _GAMETIMER_H_

#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>
using namespace std;

constexpr int DEFAULT_TURN_TIME_LIMIT = 30;
constexpr int MIN_TURN_TIME_LIMIT = 5;
constexpr int MAX_TURN_TIME_LIMIT = 120;
constexpr int TURN_TIME_STEP = 5;

extern int turnTimeLimit;
extern atomic<int> timeLeft;
extern atomic<bool> isPaused;
extern atomic<bool> isTimerRunning;


void StartTimerThread();
void StopTimerThread();
void TimerLogic();

#endif
