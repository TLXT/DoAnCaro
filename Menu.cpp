#include "Menu.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "Character.h"
#include "Sound.h"
#include "bg_menu.h"
#include "DrawBackground.hpp"
#include "btn_normal.h"
#include "btn_hover.h"
#include <windows.h>
#include <cctype>

using namespace std;

// --- CÁC HÀM WRAPPER ĐỂ TEMPLATE TRONG H CÓ THỂ GỌI ĐƯỢC ---
void DrawUIBackground() {
    DrawMenuBackground();
}

void DrawTitleArtWrapper(int startX, int startY) {
    // Gọi hàm DrawTitleArt gốc bên dưới
    void DrawTitleArt(int, int);
    DrawTitleArt(startX, startY);
}

// 1. Hàm in chữ có hộp nền tối (Dark Box) để chữ không cắn rách ảnh
void PrintTextWithBg(int startX, int startY, string text, int textColorCode) {
    int tr = 255, tg = 255, tb = 255;
    if (textColorCode == 11) { tr = 0; tg = 255; tb = 255; }
    else if (textColorCode == 14) { tr = 255; tg = 255; tb = 0; }
    else if (textColorCode == 12) { tr = 255; tg = 0; tb = 0; }

    GotoXY(startX, startY);
    // \x1b[48;2;15;15;20m là màu nền đen tuyền để lót dưới chữ
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;15;15;20m", tr, tg, tb);
    cout << text;
    printf("\x1b[0m");
}

// Gradient màu xanh băng từ trên xuống (7 màu cho 6 dòng ASCII art)
static const int TITLE_GRAD[7][3] = {
    {220, 240, 255},  // trắng băng (dòng 0)
    {140, 220, 255},  // xanh nhạt  (dòng 1)
    { 80, 200, 255},  // cyan sáng  (dòng 2)
    { 60, 180, 240},  // cyan       (dòng 3)
    { 40, 150, 220},  // xanh vừa   (dòng 4)
    { 20, 120, 200},  // xanh đậm   (dòng 5)
    { 10,  90, 180},
};

static const int SHADOW_COL[3] = { 20, 60, 120 }; // Màu outline/shadow

static const int TITLE_ART_W = 46;

static void SampleMenuBackground(int consoleX, int consoleY, int& r, int& g, int& b) {
    int x = consoleX;
    int y = consoleY * 2;
    if (x < 0) x = 0;
    if (x >= BG_MENU_W) x = BG_MENU_W - 1;
    if (y < 0) y = 0;
    if (y >= BG_MENU_H) y = BG_MENU_H - 1;
    r = BG_MENU[y][x][0];
    g = BG_MENU[y][x][1];
    b = BG_MENU[y][x][2];
}

static const char* GlyphRow(char ch, int row) {
    static const char* SPACE[7] = { "00000", "00000", "00000", "00000", "00000", "00000", "00000" };
    static const char* A[7] = { "01110", "10001", "10001", "11111", "10001", "10001", "10001" };
    static const char* B[7] = { "11110", "10001", "10001", "11110", "10001", "10001", "11110" };
    static const char* C[7] = { "01111", "10000", "10000", "10000", "10000", "10000", "01111" };
    static const char* D[7] = { "11110", "10001", "10001", "10001", "10001", "10001", "11110" };
    static const char* E[7] = { "11111", "10000", "10000", "11110", "10000", "10000", "11111" };
    static const char* F[7] = { "11111", "10000", "10000", "11110", "10000", "10000", "10000" };
    static const char* G[7] = { "01111", "10000", "10000", "10111", "10001", "10001", "01111" };
    static const char* H[7] = { "10001", "10001", "10001", "11111", "10001", "10001", "10001" };
    static const char* I[7] = { "11111", "00100", "00100", "00100", "00100", "00100", "11111" };
    static const char* J[7] = { "00111", "00010", "00010", "00010", "00010", "10010", "01100" };
    static const char* K[7] = { "10001", "10010", "10100", "11000", "10100", "10010", "10001" };
    static const char* L[7] = { "10000", "10000", "10000", "10000", "10000", "10000", "11111" };
    static const char* M[7] = { "10001", "11011", "10101", "10101", "10001", "10001", "10001" };
    static const char* N[7] = { "10001", "11001", "10101", "10011", "10001", "10001", "10001" };
    static const char* O[7] = { "01110", "10001", "10001", "10001", "10001", "10001", "01110" };
    static const char* P[7] = { "11110", "10001", "10001", "11110", "10000", "10000", "10000" };
    static const char* Q[7] = { "01110", "10001", "10001", "10001", "10101", "10010", "01101" };
    static const char* R[7] = { "11110", "10001", "10001", "11110", "10100", "10010", "10001" };
    static const char* S[7] = { "01111", "10000", "10000", "01110", "00001", "00001", "11110" };
    static const char* T[7] = { "11111", "00100", "00100", "00100", "00100", "00100", "00100" };
    static const char* U[7] = { "10001", "10001", "10001", "10001", "10001", "10001", "01110" };
    static const char* V[7] = { "10001", "10001", "10001", "10001", "10001", "01010", "00100" };
    static const char* W[7] = { "10001", "10001", "10001", "10101", "10101", "10101", "01010" };
    static const char* X[7] = { "10001", "10001", "01010", "00100", "01010", "10001", "10001" };
    static const char* Y[7] = { "10001", "10001", "01010", "00100", "00100", "00100", "00100" };
    static const char* Z[7] = { "11111", "00001", "00010", "00100", "01000", "10000", "11111" };
    static const char* N0[7] = { "01110", "10001", "10011", "10101", "11001", "10001", "01110" };
    static const char* N1[7] = { "00100", "01100", "00100", "00100", "00100", "00100", "01110" };
    static const char* N2[7] = { "01110", "10001", "00001", "00010", "00100", "01000", "11111" };
    static const char* N3[7] = { "11110", "00001", "00001", "01110", "00001", "00001", "11110" };
    static const char* N4[7] = { "10010", "10010", "10010", "11111", "00010", "00010", "00010" };
    static const char* N5[7] = { "11111", "10000", "10000", "11110", "00001", "00001", "11110" };
    static const char* N6[7] = { "01111", "10000", "10000", "11110", "10001", "10001", "01110" };
    static const char* N7[7] = { "11111", "00001", "00010", "00100", "01000", "01000", "01000" };
    static const char* N8[7] = { "01110", "10001", "10001", "01110", "10001", "10001", "01110" };
    static const char* N9[7] = { "01110", "10001", "10001", "01111", "00001", "00001", "11110" };

    switch (toupper((unsigned char)ch)) {
    case 'A': return A[row]; case 'B': return B[row]; case 'C': return C[row]; case 'D': return D[row];
    case 'E': return E[row]; case 'F': return F[row]; case 'G': return G[row]; case 'H': return H[row];
    case 'I': return I[row]; case 'J': return J[row]; case 'K': return K[row]; case 'L': return L[row];
    case 'M': return M[row]; case 'N': return N[row]; case 'O': return O[row]; case 'P': return P[row];
    case 'Q': return Q[row]; case 'R': return R[row]; case 'S': return S[row]; case 'T': return T[row];
    case 'U': return U[row]; case 'V': return V[row]; case 'W': return W[row]; case 'X': return X[row];
    case 'Y': return Y[row]; case 'Z': return Z[row];
    case '0': return N0[row]; case '1': return N1[row]; case '2': return N2[row]; case '3': return N3[row];
    case '4': return N4[row]; case '5': return N5[row]; case '6': return N6[row]; case '7': return N7[row];
    case '8': return N8[row]; case '9': return N9[row];
    default: return SPACE[row];
    }
}

static int PixelTextWidth(const string& text, int scaleX) {
    if (text.empty()) return 0;
    return static_cast<int>(((text.length() * 6) - 1) * scaleX);
}

static void DrawPixelBlock(int x, int y, int scaleX, int scaleY, int r, int g, int b) {
    string fill(scaleX, ' ');
    for (int dy = 0; dy < scaleY; dy++) {
        GotoXY(x, y + dy);
        printf("\x1b[48;2;%d;%d;%dm", r, g, b);
        cout << fill;
        printf("\x1b[0m");
    }
}

static void DrawPixelText(const string& text, int startX, int startY, int scaleX, int scaleY) {
    for (int pass = 0; pass < 2; pass++) {
        for (int idx = 0; idx < static_cast<int>(text.length()); idx++) {
            for (int row = 0; row < 7; row++) {
                const char* glyph = GlyphRow(text[idx], row);
                for (int col = 0; col < 5; col++) {
                    if (glyph[col] != '1') continue;

                    int x = startX + (idx * 6 + col) * scaleX + pass;
                    int y = startY + row * scaleY + pass;

                    if (pass == 0) {
                        DrawPixelBlock(x + 1, y + 1, scaleX, scaleY, SHADOW_COL[0], SHADOW_COL[1], SHADOW_COL[2]);
                    }
                    else {
                        int gradRow = row < 7 ? row : 6;
                        DrawPixelBlock(x - 1, y - 1, scaleX, scaleY,
                            TITLE_GRAD[gradRow][0], TITLE_GRAD[gradRow][1], TITLE_GRAD[gradRow][2]);
                    }
                }
            }
        }
    }
}

void DrawTitleArt(int startX, int startY) {
    DrawPixelText("CARO", startX, startY, 2, 1);
    return;

    const char* art[] = {
        "   ____    _    ____    ___   ",
        "  / ___|  / \\  |  _ \\  / _ \\  ",
        " | |     / _ \\ | |_) || | | | ",
        " | |___ / ___ \\|  _ < | |_| | ",
        "  \\____/_/   \\_\\_| \\_\\ \\___/  ",
    };
    int nLines = 5;

    for (int i = 0; i < nLines; i++) {
        // Vẽ shadow (lệch 1 xuống 1 phải, màu tối)
        for (int j = 0; art[i][j] != '\0'; j++) {
            if (art[i][j] == ' ') continue;

            int sr, sg, sb;
            SampleMenuBackground(startX + j + 1, startY + i + 1, sr, sg, sb);
            GotoXY(startX + j + 1, startY + i + 1);
            printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm%c\x1b[0m",
                SHADOW_COL[0], SHADOW_COL[1], SHADOW_COL[2], sr, sg, sb, art[i][j]);

        // Vẽ chữ chính với gradient
            int br, bg, bb;
            SampleMenuBackground(startX + j, startY + i, br, bg, bb);
            GotoXY(startX + j, startY + i);
            printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm%c\x1b[0m",
                TITLE_GRAD[i][0], TITLE_GRAD[i][1], TITLE_GRAD[i][2], br, bg, bb, art[i][j]);
        }
    }
}

// Vẫn giữ lại Menu gốc (GenericMenu) để đảm bảo không mất tính năng nào
int GenericMenu(string options[], int size, string title) {
    int currentSelect = 0;
    int lastSelect = -1;
    const int titleW = 30;
    const int titleX = CenterConsoleX(titleW, CONSOLE_COLS);
    const int itemX = CenterConsoleX(34, CONSOLE_COLS);
    system("cls");
    DrawMenuBackground();

    while (true) {
        if (currentSelect != lastSelect) {
            lastSelect = currentSelect;
        if (title == "GAME CARO") {
            DrawMenuTitle("CARO", 1, CONSOLE_COLS);
        }
        else {
            DrawMenuTitle(title, 2, CONSOLE_COLS);
        }

        for (int i = 0; i < size; i++) {
            if (i == currentSelect) {
                string opt = ">> " + options[i] + " <<";
                PrintTextWithBg(itemX, 12 + i * 2, opt, 14);
            }
            else {
                string opt = "   " + options[i] + "   ";
                PrintTextWithBg(itemX, 12 + i * 2, opt, 11);
            }
        }

        }
        int key = ReadMenuKey();
        if (key == 'W' || key == 72) {
            currentSelect--;
            if (currentSelect < 0) currentSelect = size - 1;
            PlayMenuSound();
        }
        else if (key == 'S' || key == 80) {
            currentSelect++;
            if (currentSelect >= size) currentSelect = 0;
            PlayMenuSound();
        }
        else if (key == 13) { // Phím Enter
            PlayMenuSound();
            return currentSelect;
        }
        else if (key == 27) {
            PlayMenuSound();
            return size - 1;
        }
    }
}

// --- Menu dùng Đồ Họa Cải Tiến (GraphicalMenu) ---
int MainMenu() {
    string options[5] = { "Play Game", "Load Game", "Settings", "Music Setting", "Exit" };
    return GraphicalMenu(options, 5, "GAME CARO", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int PlayGameMenu() {
    string options[3] = { "Player vs Player", "Player vs Bot", "Quay lai" };
    return GraphicalMenu(options, 3, "CHON CHE DO", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int DifficultyMenu() {
    string options[4] = { "De (Easy)", "Trung binh", "Kho (Hard)", "Quay lai" };
    return GraphicalMenu(options, 4, "CHON DO KHO", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int SettingsMenu() {
    string options[2] = { "Clear Data", "Quay lai" };
    return GraphicalMenu(options, 2, "CAI DAT", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int GameMenu() {
    string options[5] = { "Thoat game", "Luu game", "Tai game", "Music Setting", "Thoat menu" };
    return GraphicalMenu(options, 5, "MENU TAM DUNG", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int MusicMenu() {
    string sfxToggle = isSFXOn ? "ON" : "OFF";
    string options[9] = { "1. Music 1", "2. Music 2", "3. Music 3", "4. Music 4", "5. Music 5","6. Tat nhac", "7. Thay doi am luong","8. Sound Effect: [" + sfxToggle + "]","9. Thoat menu"};
    return GenericMenu(options, 9, "MUSIC SETTINGS");
}

void VolumeMenu() {
    int currentSelect = volumeLevel;
    while (true) {
        if (currentSelect < 0) currentSelect = 0;
        if (currentSelect > 1000) currentSelect = 1000;

        system("cls");
        DrawMenuBackground();
        DrawMenuTitle("MENU AM LUONG", 4, CONSOLE_COLS);

        int panelW = 112;
        int panelX = CenterConsoleX(panelW, CONSOLE_COLS);
        int panelY = 12;
        int barX = panelX + 6;
        int barY = panelY + 5;
        int percent = currentSelect / 10;

        DrawFrame(panelX, panelY, panelW, 10);
        PrintTextWithBg(panelX + 6, panelY + 2, "Am luong: " + to_string(percent) + "%   ", 14);

        SetColor(0, 15);
        GotoXY(barX, barY);
        cout << string(101, '-');
        GotoXY(barX + percent, barY - 1);
        SetColor(12, 15);
        cout << "V";

        for (int i = 0; i <= 100; i += 10) {
            GotoXY(barX + i, barY + 1);
            SetColor(i == percent ? 0 : 11, i == percent ? 11 : 15);
            cout << i;
        }

        PrintTextWithBg(panelX + 6, panelY + 8, "A/D hoac mui ten trai/phai de chinh, Enter de luu", 11);

        int key = ReadMenuKey();
        if (key == 'A' || key == 75) {
            currentSelect -= 50;
            if (currentSelect < 0) currentSelect = 1000;
            PlayMenuSound();
        }
        else if (key == 'D' || key == 77) {
            currentSelect += 50;
            if (currentSelect > 1000) currentSelect = 0;
            PlayMenuSound();
        }
        else if (key == 13) {
            PlayMenuSound();
            volumeLevel = currentSelect;
            system("cls");
            return;
        }
        else if (key == 27) {
            system("cls");
            return;
        }
    }
    /*
    while (true) {
        system("cls");
        system("color F0");

        SetColor(12, 15);
        GotoXY(40, 5); cout << "==============================";
        GotoXY(40, 6); cout << "||       Menu am luong       ||";
        GotoXY(40, 7); cout << "==============================";
        GotoXY(10, 17);
        SetColor(0, 15);
        for (int i = 0; i < 103; i++) cout << "-";
        cout << ">";
        for (int i = 0; i <= 100; i += 5) {
            GotoXY(10 + i, 18); // Tọa độ X chạy từ 10 đến 110

            if (i == currentSelect/10) {
                SetColor(0, 11); // Màu nền xanh, chữ đen cho mục đang chọn
                cout << i;
                GotoXY(30 + i, 16); cout << "V";
                SetColor(0, 15); // Reset lại màu cho các số sau
            }
            else {
                cout << i;
            }
        }
        int key = ReadMenuKey();
		if (key == 'A' || key == 75) {
			currentSelect -= 50;
			if (currentSelect < 0) currentSelect = 1000;
		}
		else if (key == 'D' || key == 77) {
			currentSelect += 50;
			if (currentSelect > 1000) currentSelect = 0;
		}
        else if (key == 13) { // Phím Enter
            volumeLevel = currentSelect;
            system("cls");
            return;
        }
    }
}
    */
}

string TypeName() {
    string res = "";
    while (true) {
        char c = _getch();
        if (c == 13 && res.length() > 0) {
            break;
        }
        else if (c == 8) {
            if (res.length() > 0) {
                res.pop_back();
                cout << "\b \b";
            }
        }
        else if ((isalnum((unsigned char)c)) && res.length() < 15) {
            res += c;
            cout << c;
        }
    }
    return res;
}

void InputPlayerNames(bool isBotMode) {
    system("cls");
    DrawMenuBackground(); // Nền xịn từ Menu cũ

    int consoleW = CONSOLE_COLS;
    int frameW = 52;
    int frameX = CenterConsoleX(frameW, consoleW);
    int p1Y = 11;
    int p2Y = 17;

    // Frame Nhập Tên Player 1
    DrawFrame(frameX, p1Y, frameW, 3);
    PrintTextWithBg(frameX + 4, p1Y + 1, "Nhap ten Nguoi choi 1 (X): ", 12);

    UnhideCursor();
    GotoXY(frameX + 31, p1Y + 1);

    // Tích hợp hệ thống quét pixel nền thông minh
    int r1, g1, b1;
    SampleMenuBackground(frameX + 31, p1Y + 1, r1, g1, b1);
    printf("\x1b[38;2;255;255;0m\x1b[48;2;%d;%d;%dm", r1, g1, b1);

    _PLAYER1_NAME = TypeName();
    printf("\x1b[0m");
    HideCursor();
    
    // Gọi menu chọn nhân vật cho Player 1
    CharacterASelect = CharacterSelectionMenu(); 

    // Xóa màn hình và vẽ lại background sau khi chọn xong Player 1
    system("cls");
    DrawMenuBackground();
    DrawMenuTitle("GAME CARO", 1, consoleW);
    DrawFrame(frameX, p1Y, frameW, 3);
    PrintTextWithBg(frameX + 4, p1Y + 1, "Nguoi choi 1: " + _PLAYER1_NAME, 11);

    // Frame Nhập Tên Player 2 (Hoặc Bot)
    if (isBotMode) {
        if (_BOT_DIFFICULTY == 1) _PLAYER2_NAME = "Bot (De)";
        else if (_BOT_DIFFICULTY == 2) _PLAYER2_NAME = "Bot (Trung Binh)";
        else _PLAYER2_NAME = "Bot (Kho)";

        DrawFrame(frameX, p2Y, frameW, 3);
        PrintTextWithBg(frameX + 4, p2Y + 1, "Ten Nguoi choi 2 (Bot): " + _PLAYER2_NAME, 11);
        Sleep(1000);
        
        // Bot mặc định chọn nhân vật 4
        CharacterBSelect = 4;
    }
    else {
        DrawFrame(frameX, p2Y, frameW, 3);

        while (true) {
            PrintTextWithBg(frameX + 4, p2Y + 1, "Nhap ten Nguoi choi 2 (O): ", 11);

            // Xóa vùng text nhập tên bên trong Frame
            GotoXY(frameX + 31, p2Y + 1);
            int r2, g2, b2;
            SampleMenuBackground(frameX + 31, p2Y + 1, r2, g2, b2);
            printf("\x1b[48;2;%d;%d;%dm", r2, g2, b2);
            cout << string(16, ' ');
            GotoXY(frameX + 31, p2Y + 1);

            UnhideCursor();
            printf("\x1b[38;2;255;255;0m\x1b[48;2;%d;%d;%dm", r2, g2, b2);

            _PLAYER2_NAME = TypeName();
            printf("\x1b[0m");
            HideCursor();

            if (_PLAYER1_NAME == _PLAYER2_NAME) {
                string errorMsg = "Ten bi trung voi Player 1! Nhan phim bat ky de nhap lai...";
                int errorX = CenterConsoleX(static_cast<int>(errorMsg.length()), consoleW);
                PrintTextWithBg(errorX, p2Y + 4, errorMsg, 12);
                _getch();

                // Tẩy xóa dòng báo lỗi
                PrintTextWithBg(errorX, p2Y + 4, string(errorMsg.length(), ' '), 15);
            }
            else {
                break;
            }
        }
        
        // Gọi menu chọn nhân vật cho Player 2
        CharacterBSelect = CharacterSelectionMenu();
    }
}

int GenericCharacterMenu(string options[], int size, string title) {
    int currentSelect = 0;
    int marginX = 78;
    int lastSelect = -1;

    system("cls");
    DrawMenuBackground();

    while (true) {
        if (currentSelect != lastSelect) {
            // Xóa vùng vẽ nhân vật cũ bằng background
            system("cls");
            DrawMenuBackground();
            /*
            char pixel[64];
            for (int i = 2; i < 20; i++) {
                GotoXY(24, i);
                string row_buf = "";
                for (int j = 4; j < 60; j++) {
                    int r_top = BG_MENU[i * 2][j][0], g_top = BG_MENU[i * 2][j][1], b_top = BG_MENU[i * 2][j][2];
                    int r_bot = BG_MENU[i * 2 + 1][j][0], g_bot = BG_MENU[i * 2 + 1][j][1], b_bot = BG_MENU[i * 2 + 1][j][2];
                    snprintf(pixel, sizeof(pixel), "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm\xE2\x96\x80", r_top, g_top, b_top, r_bot, g_bot, b_bot);
                    row_buf += pixel;
                }
                cout << row_buf;
            }
            printf("\x1b[0m");
            */

            outsidedisplay(currentSelect);
            DrawMenuTitle(title, 2, CONSOLE_COLS);
            lastSelect = currentSelect;
        }

        // Vẽ nhân vật trước khi đợi nhập phím
        // Vẽ Menu ở phía bên phải
        SetColor(12, 15);

        for (int i = 0; i < size; i++) {
            if (i == currentSelect) {
                SetColor(0, 11); // Nền Cyan cho mục đang chọn
                GotoXY(marginX + 5, 12 + i * 2);
                cout << ">> " << options[i] << " <<";
            }
            else {
                SetColor(0, 15); // Nền trắng cho các mục khác
                GotoXY(marginX + 5, 12 + i * 2);
                cout << "   " << options[i] << "   ";
            }
        }
        SetColor(0, 15);

        // Đợi người dùng nhấn phím
        int key = ReadMenuKey();

        // Xử lý logic phím bấm
        if (key == 'W' || key == 72) {
            currentSelect--;
            if (currentSelect < 0) currentSelect = size - 1;
            PlayMenuSound();
        }
        else if (key == 'S' || key == 80) {
            currentSelect++;
            if (currentSelect >= size) currentSelect = 0;
            PlayMenuSound();
        }
        else if (key == 13) { // Phím Enter
            PlayMenuSound();
            return currentSelect;
        }
        else if (key == 27) {
            PlayMenuSound();
            return currentSelect;
        }
    }
}

int CharacterSelectionMenu() {
    string options[5] = { "1. Knight","2. Assassin","3. Vampire ","4. Paladin" ,"5. Officer"};
    return GenericCharacterMenu(options, 5, "CHARACTER MENU");
}

void DrawFrame(int x, int y, int w, int h) {
    printf("\x1b[38;2;255;255;255m\x1b[48;2;15;15;20m");
    GotoXY(x, y);
    cout << "+"; for (int i = 0; i < w - 2; i++) cout << "-"; cout << "+";
    for (int i = 1; i < h - 1; i++) {
        GotoXY(x, y + i); cout << "|";
        GotoXY(x + 1, y + i); cout << string(w - 2, ' ');
        GotoXY(x + w - 1, y + i); cout << "|";
    }
    GotoXY(x, y + h - 1);
    cout << "+"; for (int i = 0; i < w - 2; i++) cout << "-"; cout << "+";
    printf("\x1b[0m");
}

void DrawMenuTitle(const string& title, int y, int consoleW) {
    if (title == "GAME CARO" || title == "CARO") {
        DrawTitleArt(CenterConsoleX(TITLE_ART_W, consoleW), y);
    } else {
        int titleW = PixelTextWidth(title, 1);
        DrawPixelText(title, CenterConsoleX(titleW, consoleW), y, 1, 1);
    }
}
