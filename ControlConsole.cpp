#include "ControlConsole.h"

std::mutex consoleMutex;

void GotoXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hIn, &mode);
    mode &= ~ENABLE_QUICK_EDIT_MODE;
	mode &= ~ENABLE_ECHO_INPUT;
	mode &= ~ENABLE_LINE_INPUT;
    mode |= ENABLE_EXTENDED_FLAGS;
    SetConsoleMode(hIn, mode);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(hOut, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &info);
}

void unhideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(hOut, &info);

    info.bVisible = TRUE;
    info.dwSize = 25;
    SetConsoleCursorInfo(hOut, &info);
}

void setColor(int color, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    color = bgColor * 16 + color;
    SetConsoleTextAttribute(hConsole, color);
}

void setBgColor(int color, int bgColor) {
    auto DecToHex = [](int x) {
        if (x < 0 || x > 15) return ' ';
        char a[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
        return a[x];
        };
    string systemCommand = "color ";
    systemCommand += DecToHex(bgColor);
    systemCommand += DecToHex(color);

    system(systemCommand.c_str());
}

void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow();
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void setConsoleWindow(int width, int height) {
    HWND consoleWindow = GetConsoleWindow();
    RECT r;
    GetWindowRect(consoleWindow, &r);
    MoveWindow(consoleWindow, r.left, r.top, width, height, TRUE);
}
