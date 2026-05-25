#include "Replay.h"
#include "DrawBoard.h"
#include "ControlConsole.h"
#include "GamePlay.h"
#include "GameStatus.h"
#include <conio.h>
#include <windows.h>
#include <string>

// --- THÊM THƯ VIỆN ẢNH VÀO ĐÂY ---
#include "Menu.h"
#include "btn_normal.h"
#include "btn_hover.h"

using namespace std;

//hỏi người dùng có muốn replay không
bool AskForReplay() {
    return GraphicalYesNo("Ban co muon xem lai van dau khong?", 10, true,
        BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

//chọn tốc độ
void ChooseReplaySpeed(float& speed, int& delay) {
    int choice = 1;
    system("cls");

    int consoleW = 120;
    string prompt = "Chon toc do xem lai (phim mui ten hoac A/D)";
    int frameW = prompt.length() + 12;
    int frameX = (consoleW - frameW) / 2;
    int startY = 8;

    DrawFrame(frameX, startY, frameW, 5);
    GotoXY(frameX + 6, startY + 2);
    SetColor(12, 15);
    cout << prompt;

    int btnCols = BTN_NORMAL_W * 2;
    int totalBtnW = btnCols * 3 + 20; // 3 nút, cách nhau 10 khoảng trắng
    int startX = (consoleW - totalBtnW) / 2;
    int btnY = startY + 8;

    int bgNorm = BTN_NORMAL[BTN_NORMAL_H / 2][BTN_NORMAL_W / 2];
    int bgHov = BTN_HOVER[BTN_HOVER_H / 2][BTN_HOVER_W / 2];
    int lastChoice = -1;

    while (true) {
        if (choice != lastChoice) {
            string opts[3] = { "0.5x", "1.0x", "1.5x" };
            for (int i = 0; i < 3; i++) {
                int bX = startX + i * (btnCols + 10);
                if (choice == i) {
                    DrawSolidImage(BTN_HOVER, BTN_HOVER_W, BTN_HOVER_H, bX, btnY);
                    GotoXY(bX + (btnCols - opts[i].length()) / 2, btnY + BTN_HOVER_H / 2);
                    SetColor(0, bgHov); cout << opts[i];
                }
                else {
                    DrawSolidImage(BTN_NORMAL, BTN_NORMAL_W, BTN_NORMAL_H, bX, btnY);
                    GotoXY(bX + (btnCols - opts[i].length()) / 2, btnY + BTN_NORMAL_H / 2);
                    SetColor(0, bgNorm); cout << opts[i];
                }
            }
            lastChoice = choice;
        }

        SetColor(0, 15);
        char ch = _getch();
        if (ch == -32 || ch == 0) ch = _getch();
        if (ch == 75 || ch == 'a' || ch == 'A') { if (choice > 0) choice--; }
        else if (ch == 77 || ch == 'd' || ch == 'D') { if (choice < 2) choice++; }
        else if (ch == 13) break;
    }

    if (choice == 0) { speed = 0.5f; delay = (int)(500 / 0.5f); }
    else if (choice == 1) { speed = 1.0f; delay = 500; }
    else { speed = 1.5f; delay = (int)(500 / 1.5f); }
}

//vẽ lại bàn cờ và ghi chú nút
void RedrawBoard(int step) {
    //reset bàn cờ
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j].c = 0;
        }
    }
    //vẽ lại bàn cờ
    system("cls");
    DrawBoard(BOARD_SIZE);

    //ghi chú các nút
    SetColor(0, 15);
    GotoXY(LEFT + 67, TOP + 5);
    cout << "<- / A : Tua lui 5s";
    GotoXY(LEFT + 67, TOP + 6);
    cout << "-> / D : Tua toi 5s";
    GotoXY(LEFT + 67, TOP + 7);
    cout << "Space/P: Dung/Phat";
    GotoXY(LEFT + 67, TOP + 8);
    cout << "Esc    : Thoat";

    //cập nhật mảng bàn cờ 
    for (int i = 0; i < step; i++) {
        int r = moveHistory[i].row;
        int c = moveHistory[i].col;
        _A[r][c].c = moveHistory[i].c;
        DrawCell(_A[r][c].x, _A[r][c].y, 15);
    }
}

//vòng lặp chính của Replay
void PlayReplay(int delay) {
    int totalMoves = currentStep;
    int step = 0;
    int movesToSkipFor5s = 5000 / delay;

    RedrawBoard(0); // vẽ khung và ghi chú

    bool isPaused = false;
    bool exitReplay = false; //kiểm tra nhấn esc thì rời replay

    while (step < totalMoves) {
        bool skipTriggered = false;
        DWORD startTime = GetTickCount64();

        //vòng lặp chờ để tạo delay, kết hợp bắt phím
        while (isPaused || (GetTickCount64() - startTime < delay)) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == -32 || ch == 0) ch = _getch();

                if (ch == 27) { // Esc
                    exitReplay = true;
                    break;
                }
                else if (ch == 'p' || ch == 'P' || ch == 32) { // Phím Space hoặc P
                    isPaused = !isPaused;
                    if (isPaused) {
                        GotoXY(LEFT + 67, TOP + 16);
                        SetColor(12, 15);
                        cout << "|| PAUSED  ";
                    }
                    else {
                        GotoXY(LEFT + 67, TOP + 16);
                        cout << "           ";
                        startTime = GetTickCount64(); //reset timer để đi tiếp mượt mà
                    }
                }
                else if (ch == 75 || ch == 'a' || ch == 'A') { //tua lui
                    int targetStep = step - movesToSkipFor5s;
                    if (targetStep < 0) targetStep = 0;

                    //xóa ngược từng con cờ
                    for (int i = step - 1; i >= targetStep; i--) {
                        int r = moveHistory[i].row;
                        int c = moveHistory[i].col;
                        _A[r][c].c = 0;
                        DrawCell(_A[r][c].x, _A[r][c].y, 15); //vẽ khoảng trắng khi c=0
                    }
                    step = targetStep;

                    GotoXY(LEFT + 67, TOP + 14);
                    SetColor(12, 15); cout << "<<  -5s  ";
                    skipTriggered = true;
                    break;
                }
                else if (ch == 77 || ch == 'd' || ch == 'D') { // tua tới
                    int targetStep = step + movesToSkipFor5s;
                    if (targetStep > totalMoves) targetStep = totalMoves;

                    //vẽ tới các con cờ mới
                    for (int i = step; i < targetStep; i++) {
                        int r = moveHistory[i].row;
                        int c = moveHistory[i].col;
                        _A[r][c].c = moveHistory[i].c;
                        DrawCell(_A[r][c].x, _A[r][c].y, 15);
                    }
                    step = targetStep;

                    GotoXY(LEFT + 67, TOP + 15);
                    SetColor(12, 15); cout << ">>  +5s  ";
                    skipTriggered = true;
                    break;
                }
            }
            Sleep(10);

            //giữ timer không chạy nếu đang pause
            if (isPaused) startTime = GetTickCount64();
        }

        if (exitReplay) break; //thoát nếu bấm Esc

        if (skipTriggered) {
            Sleep(400); //dừng xíu cho người ta kịp nhìn chữ tua
            GotoXY(LEFT + 67, TOP + 14); cout << "         ";
            GotoXY(LEFT + 67, TOP + 15); cout << "         ";
            continue; //đã cập nhật step lúc tua nên bỏ qua bước đánh thông thường
        }

        //đánh nước đi hiện tại (nếu chưa tua/pause)
        if (step < totalMoves) {
            int r = moveHistory[step].row;
            int c = moveHistory[step].col;
            _A[r][c].c = moveHistory[step].c;
            DrawCell(_A[r][c].x, _A[r][c].y, 15);
            step++;
        }
    }
}

//hàm quản lý tổng luồng
void HandleReplayOption() {
    system("cls");
    while (AskForReplay()) {
        float speed;
        int delay;
        ChooseReplaySpeed(speed, delay);
        PlayReplay(delay);

        //chạy xong Replay (hoặc nhấn Esc thoát), màn hình xóa đi và hỏi lại
        system("cls");
    }
    system("cls");
}