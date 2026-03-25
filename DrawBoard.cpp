#include "DrawBoard.h"
#include "ControlConsole.h"
#include "GameStatus.h"

using namespace std;

void DrawBoard(int pSize) {
    lock_guard<std::mutex> lock(consoleMutex); //Cập nhật dòng khóa đảm bảo tránh xong đột khi dùng hàm GotoXY
    SetColor(0, 15); // Chữ đen, nền trắng

    for (int i = 0; i <= pSize; i++) {
        for (int j = 0; j <= pSize; j++) {
            GotoXY(LEFT + 4 * j, TOP + 2 * i);
            cout << "+";
            if (j < pSize) cout << "---";
        }
        if (i < pSize) {
            for (int j = 0; j <= pSize; j++) {
                GotoXY(LEFT + 4 * j, TOP + 2 * i + 1);
                cout << "|";
            }
        }
    }
}