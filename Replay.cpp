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

//vẽ lại bàn cờ đến bước nhất định
void RedrawBoardToStep(int step) {
    //reset bàn cờ
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j].c = 0;
        }
    }
    //vẽ lại bàn cờ
    system("cls");
    DrawBoard(BOARD_SIZE);

    //ghi chú
    GotoXY(LEFT + 67, TOP + 9);
    SetColor(12, 15);
    cout << "<- / A : Tua lui 5s";
    GotoXY(LEFT + 67, TOP + 10);
    SetColor(10, 15);
    cout << "-> / D : Tua toi 5s";

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
    int movesToSkipFor5s = 5000 / delay; //số nước đi tương đương 5 giây để phục vụ việc tua 5s

    RedrawBoardToStep(0);

    while (step < totalMoves) {
        bool skipTriggered = false;

        //bắt phím không đồng bộ trong khi chờ
        DWORD startTime = GetTickCount64();
        while (GetTickCount64() - startTime < delay) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == -32 || ch == 0) ch = _getch();

                if (ch == 75 || ch == 'a' || ch == 'A') { //tua lui
                    step -= movesToSkipFor5s;
                    if (step < 0) step = 0;

                    RedrawBoardToStep(step);

                    GotoXY(LEFT + 67, TOP + 13); //tọa độ in dòng chữ << -5s
                    SetColor(12, 15);
                    cout << "<< -5s";

                    skipTriggered = true;
                    break;
                }
                else if (ch == 77 || ch == 'd' || ch == 'D') { //tua tới
                    step += movesToSkipFor5s;
                    if (step > totalMoves) step = totalMoves;

                    RedrawBoardToStep(step);

                    GotoXY(LEFT + 67, TOP + 14); //tọa độ in dòng chữ >> +5s
                    SetColor(12, 15);
                    cout << ">> +5s";

                    skipTriggered = true;
                    break;
                }
            }
            Sleep(10); 
        }

        if (skipTriggered) {
            Sleep(500); // Tạm dừng nửa giây để người dùng thấy chữ << -5s / >> +5s
            //xóa chữ tua
            GotoXY(LEFT + 67, TOP + 13); cout << "      ";
            GotoXY(LEFT + 67, TOP + 14); cout << "      ";
            continue;
        }

        //đánh nước đi hiện tại (nếu chưa skip qua)
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

        //chạy xong Replay, màn hình sẽ xóa đi và hỏi lại vòng lặp while
        system("cls");
    }
    //nếu chọn No hoặc xem chán rồi thoát -> quay về hệ thống Menu mặc định
    system("cls");
}