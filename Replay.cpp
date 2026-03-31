#include "Replay.h"
#include "DrawBoard.h"
#include "ControlConsole.h"
#include "GamePlay.h"
#include "GameStatus.h"
#include <conio.h>
#include <windows.h>
#include <string>

using namespace std;

//hỏi người dùng có muốn replay không
bool AskForReplay() {
    int choice = 0; // 0: Yes, 1: No
    int startX = 40;
    int startY = 15;

    system("cls");

    while (true) {
        GotoXY(startX, startY);
        SetColor(0, 15);
        cout << "Ban co muon xem lai van dau khong?";

        //vẽ nút YES
        GotoXY(startX + 5, startY + 2);
        if (choice == 0) {
            SetColor(0, 11); //
            cout << ">> YES <<";
        }
        else {
            SetColor(0, 15);
            cout << "   YES   ";
        }

        //vẽ nút NO
        GotoXY(startX + 20, startY + 2);
        if (choice == 1) {
            SetColor(0, 11);
            cout << ">> NO <<";
        }
        else {
            SetColor(0, 15);
            cout << "   NO   ";
        }

        SetColor(0, 15);

        char ch = _getch();
        if (ch == -32 || ch == 0) ch = _getch(); //phím đặc biệt nên _getch() lại để lấy mã thật

        if (ch == 75 || ch == 'a' || ch == 'A') { //trái
            choice = 0;
        }
        else if (ch == 77 || ch == 'd' || ch == 'D') { //phải
            choice = 1;
        }
        else if (ch == 13) { //enter
            break;
        }
    }
    return (choice == 0);
}

//chọn tốc độ
void ChooseReplaySpeed(float& speed, int& delay) {
    int choice = 1; // 0: 0.5x, 1: 1x, 2: 1.5x
    int startX = 40;
    int startY = 15;

    system("cls");

    while (true) {
        GotoXY(startX, startY);
        SetColor(0, 15);
        cout << "Chon toc do xem lai (su dung phim mui ten trai/phai hoac A/D):";

        //nút 0.5x
        GotoXY(startX - 2, startY + 2);
        if (choice == 0) {
            SetColor(0, 11);
            cout << ">> 0.5x <<";
        }
        else {
            SetColor(0, 15);
            cout << "   0.5x   ";
        }

        //nút 1.0x
        GotoXY(startX + 12, startY + 2);
        if (choice == 1) {
            SetColor(0, 11);
            cout << ">> 1.0x (0.5s/buoc) <<";
        }
        else {
            SetColor(0, 15);
            cout << "   1.0x (0.5s/buoc)   ";
        }

        //nút 1.5x
        GotoXY(startX + 38, startY + 2);
        if (choice == 2) {
            SetColor(0, 11);
            cout << ">> 1.5x <<";
        }
        else {
            SetColor(0, 15);
            cout << "   1.5x   ";
        }

        SetColor(0, 15);

        char ch = _getch();
        if (ch == -32 || ch == 0) ch = _getch();

        if (ch == 75 || ch == 'a' || ch == 'A') {
            if (choice > 0) choice--;
        }
        else if (ch == 77 || ch == 'd' || ch == 'D') {
            if (choice < 2) choice++;
        }
        else if (ch == 13) {
            break;
        }
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