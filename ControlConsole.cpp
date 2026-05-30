#include "ControlConsole.h"

std::mutex consoleMutex;

void GotoXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HideCursor() {
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

void UnhideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(hOut, &info);

    info.bVisible = TRUE;
    info.dwSize = 25;
    SetConsoleCursorInfo(hOut, &info);
}

void SetColor(int color, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    color = bgColor * 16 + color;
    SetConsoleTextAttribute(hConsole, color);
}

void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow();
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME | WS_SIZEBOX);
    SetWindowLong(consoleWindow, GWL_STYLE, style);

    HMENU systemMenu = GetSystemMenu(consoleWindow, FALSE);
    if (systemMenu != NULL) {
        DeleteMenu(systemMenu, SC_SIZE, MF_BYCOMMAND);
        DeleteMenu(systemMenu, SC_MAXIMIZE, MF_BYCOMMAND);
        DeleteMenu(systemMenu, SC_MINIMIZE, MF_BYCOMMAND);
    }

    SetWindowPos(consoleWindow, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void SetConsoleWindow(int width, int height) {
    (void)width;
    (void)height;

    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow == NULL) return;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);

    int cols = 0;
    int lines = 0;
    int cellW = 12;
    int cellH = 16;

    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        if (cols <= 0) cols = csbi.dwSize.X;
        if (lines <= 0) lines = csbi.dwSize.Y;
    }

    if (GetCurrentConsoleFontEx(hOut, FALSE, &cfi)) {
        if (cfi.dwFontSize.X > 0) cellW = cfi.dwFontSize.X;
        if (cfi.dwFontSize.Y > 0) cellH = cfi.dwFontSize.Y;
    }

    RECT r;
    RECT client;
    GetWindowRect(consoleWindow, &r);
    GetClientRect(consoleWindow, &client);

    int frameW = (r.right - r.left) - (client.right - client.left);
    int frameH = (r.bottom - r.top) - (client.bottom - client.top);
    int targetW = (cols > 0 ? cols * cellW + frameW : width);
    int targetH = (lines > 0 ? lines * cellH + frameH : height);

    MoveWindow(consoleWindow, r.left, r.top, targetW, targetH, TRUE);
    FixConsoleWindow();
}

void ConfigureConsoleSize(int cols, int lines) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    SMALL_RECT minimalWindow = { 0, 0, 1, 1 };
    SetConsoleWindowInfo(hOut, TRUE, &minimalWindow);

    COORD bufferSize;
    bufferSize.X = static_cast<SHORT>(cols);
    bufferSize.Y = static_cast<SHORT>(lines);
    SetConsoleScreenBufferSize(hOut, bufferSize);

    SMALL_RECT windowRect;
    windowRect.Left = 0;
    windowRect.Top = 0;
    windowRect.Right = static_cast<SHORT>(cols - 1);
    windowRect.Bottom = static_cast<SHORT>(lines - 1);
    SetConsoleWindowInfo(hOut, TRUE, &windowRect);
    FixConsoleWindow();
}
