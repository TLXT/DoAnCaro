#ifndef _GAMETIMER_H_
#define _GAMETIMER_H_

#include <atomic> // Đảm bảo việc tính và ghi thời gian không bị ảnh hưởng khi 2 luồng cùng tính
#include <thread>
#include <mutex>
#include <iostream>
using namespace std;

const int TURN_TIME_LIMIT = 30;

extern atomic<int> timeLeft;
extern atomic<bool> isPaused;
extern atomic<bool> isTimerRunning;

// Các hàm xử lý
void StartTimerThread();
void StopTimerThread();
void TimerLogic(); // Hàm chạy trong luồng phụ

#endif