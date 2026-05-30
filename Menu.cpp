#include "Menu.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include "Character.h"
#include "Sound.h"
#include "bg_menu.h"
#include "DrawBackground.hpp"
#include "btn_normal.h"
#include "btn_hover.h"
#include "Language.h"
#include "GameTimer.h"
#include <windows.h>
#include <cctype>
#include <vector>

using namespace std;


void DrawUIBackground() {
    DrawMenuBackground();
}

void DrawTitleArtWrapper(int startX, int startY) {

    void DrawTitleArt(int, int);
    DrawTitleArt(startX, startY);
}


void PrintTextWithBg(int startX, int startY, string text, int textColorCode) {
    int tr = 255, tg = 255, tb = 255;
    if (textColorCode == 11) { tr = 0; tg = 255; tb = 255; }
    else if (textColorCode == 14) { tr = 255; tg = 255; tb = 0; }
    else if (textColorCode == 12) { tr = 255; tg = 0; tb = 0; }

    GotoXY(startX, startY);

    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;15;15;20m", tr, tg, tb);
    cout << text;
    printf("\x1b[0m");
}

static void FillConsoleRect(int x, int y, int w, int h, int bgColor) {
    SetColor(0, bgColor);
    string row(w, ' ');
    for (int i = 0; i < h; i++) {
        GotoXY(x, y + i);
        cout << row;
    }
    SetColor(15, 0);
}


static const int TITLE_GRAD[7][3] = {
    {220, 240, 255},
    {140, 220, 255},
    { 80, 200, 255},
    { 60, 180, 240},
    { 40, 150, 220},
    { 20, 120, 200},
    { 10,  90, 180},
};

static const int SHADOW_COL[3] = { 20, 60, 120 };

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

        for (int j = 0; art[i][j] != '\0'; j++) {
            if (art[i][j] == ' ') continue;

            int sr, sg, sb;
            SampleMenuBackground(startX + j + 1, startY + i + 1, sr, sg, sb);
            GotoXY(startX + j + 1, startY + i + 1);
            printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm%c\x1b[0m",
                SHADOW_COL[0], SHADOW_COL[1], SHADOW_COL[2], sr, sg, sb, art[i][j]);


            int br, bg, bb;
            SampleMenuBackground(startX + j, startY + i, br, bg, bb);
            GotoXY(startX + j, startY + i);
            printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm%c\x1b[0m",
                TITLE_GRAD[i][0], TITLE_GRAD[i][1], TITLE_GRAD[i][2], br, bg, bb, art[i][j]);
        }
    }
}


static void DrawMenuButton(const string& text, int x, int y, bool selected) {
    const int btnCols = BTN_NORMAL_W * 2;
    int bg = selected ? BTN_HOVER[BTN_HOVER_H / 2][BTN_HOVER_W / 2] : BTN_NORMAL[BTN_NORMAL_H / 2][BTN_NORMAL_W / 2];

    if (selected) {
        DrawSolidImage(BTN_HOVER, BTN_HOVER_W, BTN_HOVER_H, x, y);
    }
    else {
        DrawSolidImage(BTN_NORMAL, BTN_NORMAL_W, BTN_NORMAL_H, x, y);
    }

    int textLen = TextDisplayWidth(text);
    GotoXY(x + (btnCols - textLen) / 2, y + BTN_NORMAL_H / 2);
    SetColor(0, bg);
    cout << text;
    SetColor(0, 15);
}

int GenericMenu(string options[], int size, string title) {
    int currentSelect = 0;
    int lastSelect = -1;
    int itemW = 34;
    for (int i = 0; i < size; i++) {
        itemW = max(itemW, TextDisplayWidth(options[i]) + 8);
    }
    const int itemX = CenterConsoleX(itemW, CONSOLE_COLS);
    ClearScreenFast();
    DrawMenuBackground();

    if (size > 6) {
        const int rows = (size + 1) / 2;
        const int btnCols = BTN_NORMAL_W * 2;
        const int colGap = 8;
        const int rowGap = BTN_NORMAL_H + 2;
        const int totalW = btnCols * 2 + colGap;
        const int startX = CenterConsoleX(totalW, CONSOLE_COLS);
        const int startY = title.empty() ? 4 : 8;

        while (true) {
            if (currentSelect != lastSelect) {
                auto drawOption = [&](int index, bool selected) {
                    int col = index / rows;
                    int row = index % rows;
                    int x = startX + col * (btnCols + colGap);
                    int y = startY + row * rowGap;
                    DrawMenuButton(options[index], x, y, selected);
                };

                if (lastSelect == -1) {
                    if (!title.empty()) {
                        DrawMenuTitle(title, 2, CONSOLE_COLS);
                    }

                    for (int i = 0; i < size; i++) {
                        drawOption(i, i == currentSelect);
                    }
                }
                else {
                    drawOption(lastSelect, false);
                    drawOption(currentSelect, true);
                }
                lastSelect = currentSelect;
            }

            int key = ReadMenuKey();
            int col = currentSelect / rows;
            int row = currentSelect % rows;

            if (key == 'W' || key == 72) {
                int next;
                do {
                    row--;
                    if (row < 0) row = rows - 1;
                    next = col * rows + row;
                } while (next >= size);
                currentSelect = next;
                PlayMenuSound();
            }
            else if (key == 'S' || key == 80) {
                int next;
                do {
                    row++;
                    if (row >= rows) row = 0;
                    next = col * rows + row;
                } while (next >= size);
                currentSelect = next;
                PlayMenuSound();
            }
            else if (key == 'A' || key == 75 || key == 'D' || key == 77) {
                int otherCol = (col == 0) ? 1 : 0;
                int next = otherCol * rows + row;
                if (next >= size) next = size - 1;
                currentSelect = next;
                PlayMenuSound();
            }
            else if (key == 13) {
                PlayMenuSound();
                return currentSelect;
            }
            else if (key == 27) {
                PlayMenuSound();
                return size - 1;
            }
        }
    }

    auto drawTextOption = [&](int index, bool selected) {
        string opt = selected ? (">> " + options[index] + " <<") : ("   " + options[index] + "   ");
        int pad = itemW - TextDisplayWidth(opt);
        if (pad > 0) opt += string(pad, ' ');
        PrintTextWithBg(itemX, 12 + index * 2, opt, selected ? 14 : 11);
    };

    while (true) {
        if (currentSelect != lastSelect) {
            if (lastSelect == -1) {
                if (title == "GAME CARO") {
                    DrawMenuTitle("CARO", 1, CONSOLE_COLS);
                }
                else {
                    DrawMenuTitle(title, 2, CONSOLE_COLS);
                }

                for (int i = 0; i < size; i++) {
                    drawTextOption(i, i == currentSelect);
                }
            }
            else {
                drawTextOption(lastSelect, false);
                drawTextOption(currentSelect, true);
            }
            lastSelect = currentSelect;
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
        else if (key == 13) {
            PlayMenuSound();
            return currentSelect;
        }
        else if (key == 27) {
            PlayMenuSound();
            return size - 1;
        }
    }
}


static void DrawInfoTextLine(int x, int& y, const string& text, int color = 11) {
    PrintTextWithBg(x, y, text, color);
    y++;
}

static void WaitInfoScreenBack() {
    HideCursor();
    while (true) {
        int key = ReadMenuKey();
        if (key == 13 || key == 27 || key == 'B') {
            PlayMenuSound();
            return;
        }
    }
}

void ShowGuideScreen() {
    ClearScreenFast();
    DrawMenuBackground();
    DrawMenuTitle(L(MainGuide), 1, CONSOLE_COLS);

    const int panelW = 112;
    const int panelH = 30;
    const int panelX = CenterConsoleX(panelW, CONSOLE_COLS);
    const int panelY = 7;
    DrawFrame(panelX, panelY, panelW, panelH);

    const bool vi = GetLanguage() == Vietnamese;
    int leftY = panelY + 2;
    int rightY = panelY + 2;
    const int leftX = panelX + 4;
    const int rightX = panelX + 58;

    DrawInfoTextLine(leftX, leftY, vi ? u8"C\u00C1CH CH\u01A0I" : "HOW TO PLAY", 14);
    DrawInfoTextLine(leftX, leftY, vi ? u8"- Hai ng\u01B0\u1EDDi l\u1EA7n l\u01B0\u1EE3t \u0111\u1EB7t X/O tr\u00EAn b\u00E0n c\u1EDD." : "- Players take turns placing X/O.");
    DrawInfoTextLine(leftX, leftY, vi ? u8"- Ai c\u00F3 5 qu\u00E2n li\u00EAn ti\u1EBFp tr\u01B0\u1EDBc s\u1EBD th\u1EAFng." : "- First 5 connected marks wins.");
    DrawInfoTextLine(leftX, leftY, vi ? u8"- H\u1EBFt gi\u1EDD: h\u1EC7 th\u1ED1ng t\u1EF1 \u0111\u00E1nh m\u1ED9t n\u01B0\u1EDBc." : "- Timeout makes an automatic move.");
    DrawInfoTextLine(leftX, leftY, vi ? u8"- C\u00F3 l\u01B0u/t\u1EA3i v\u00E1n, replay, nh\u00E2n v\u1EADt v\u00E0 \u00E2m thanh." : "- Save/load, replay, characters and sound are supported.");
    leftY++;

    DrawInfoTextLine(leftX, leftY, vi ? u8"TRONG TR\u1EACN" : "IN GAME", 14);
    DrawInfoTextLine(leftX, leftY, vi ? u8"- WASD/M\u0169i t\u00EAn: di chuy\u1EC3n con tr\u1ECF." : "- WASD/Arrows: move cursor.");
    DrawInfoTextLine(leftX, leftY, vi ? u8"- ENTER: \u0111\u00E1nh c\u1EDD t\u1EA1i \u00F4 \u0111ang ch\u1ECDn." : "- ENTER: place a mark.");
    DrawInfoTextLine(leftX, leftY, vi ? u8"- Y: Undo | U: Redo | R: Restart." : "- Y: Undo | U: Redo | R: Restart.");
    DrawInfoTextLine(leftX, leftY, vi ? u8"- SPACE/P/ESC: m\u1EDF menu t\u1EA1m d\u1EEBng." : "- SPACE/P/ESC: open pause menu.");
    DrawInfoTextLine(leftX, leftY, vi ? u8"- \u1EDE h\u00E0ng cu\u1ED1i b\u1EA5m S: v\u00E0o thanh n\u00FAt d\u01B0\u1EDBi." : "- Press S on last row to enter buttons.");
    DrawInfoTextLine(leftX, leftY, vi ? u8"- A/D \u0111\u1ED5i n\u00FAt, ENTER ch\u1ECDn n\u00FAt." : "- A/D changes button, ENTER selects.");
    leftY++;

    DrawInfoTextLine(leftX, leftY, vi ? u8"MENU T\u1EA0M D\u1EEANG" : "PAUSE MENU", 14);
    DrawInfoTextLine(leftX, leftY, vi ? u8"- L\u01B0u v\u00E1n, t\u1EA3i v\u00E1n, c\u00E0i \u0111\u1EB7t, tho\u00E1t." : "- Save, load, settings, exit.");
    DrawInfoTextLine(leftX, leftY, vi ? u8"- Tho\u00E1t menu/ESC: quay l\u1EA1i tr\u1EADn \u0111\u1EA5u." : "- Close menu/ESC: return to match.");

    DrawInfoTextLine(rightX, rightY, vi ? u8"C\u00C1C M\u00C0N MENU" : "MENU SCREENS", 14);
    DrawInfoTextLine(rightX, rightY, vi ? u8"- Menu ch\u00EDnh: W/S ho\u1EB7c m\u0169i t\u00EAn, ENTER ch\u1ECDn." : "- Main menu: W/S or arrows, ENTER.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- \u0110\u1EB7t t\u00EAn: TAB/m\u0169i t\u00EAn \u0111\u1ED5i \u00F4 nh\u1EADp." : "- Player setup: TAB/arrows switch fields.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- A/D t\u1EA1i \u00F4 th\u1EDDi gian: \u0111\u1ED5i gi\u1EDBi h\u1EA1n l\u01B0\u1EE3t." : "- A/D on time field changes turn limit.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- TI\u1EBEP T\u1EE4C ch\u1EC9 s\u00E1ng khi t\u00EAn h\u1EE3p l\u1EC7, kh\u00F4ng tr\u00F9ng." : "- NEXT enables only when names are valid.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- Ch\u1ECDn nh\u00E2n v\u1EADt: WASD/m\u0169i t\u00EAn, ENTER." : "- Character select: WASD/arrows, ENTER.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- Kh\u00F4ng th\u1EC3 ch\u1ECDn tr\u00F9ng nh\u00E2n v\u1EADt." : "- Duplicate characters are blocked.");
    rightY++;

    DrawInfoTextLine(rightX, rightY, "SAVE / LOAD", 14);
    DrawInfoTextLine(rightX, rightY, vi ? u8"- WASD/m\u0169i t\u00EAn: ch\u1ECDn slot." : "- WASD/arrows: select slot.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- ENTER: l\u01B0u/t\u1EA3i slot \u0111ang ch\u1ECDn." : "- ENTER: save/load selected slot.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- X: x\u00F3a file l\u01B0u | ESC: quay l\u1EA1i." : "- X: delete save | ESC: back.");
    rightY++;

    DrawInfoTextLine(rightX, rightY, vi ? u8"REPLAY & \u00C2M THANH" : "REPLAY & SOUND", 14);
    DrawInfoTextLine(rightX, rightY, vi ? u8"- Replay: A/D tua 5s, SPACE/P d\u1EEBng/ph\u00E1t." : "- Replay: A/D seek 5s, SPACE/P pause/play.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- ESC: tho\u00E1t replay." : "- ESC: exit replay.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- \u00C2m l\u01B0\u1EE3ng: A/D ho\u1EB7c m\u0169i t\u00EAn tr\u00E1i/ph\u1EA3i." : "- Volume: A/D or left/right arrows.");
    DrawInfoTextLine(rightX, rightY, vi ? u8"- Menu nh\u1EA1c: ch\u1ECDn b\u00E0i, b\u1EADt/t\u1EAFt nh\u1EA1c, SFX." : "- Music menu: track, music on/off, SFX.");

    string backText = vi ? u8"ENTER / ESC / B: QUAY L\u1EA0I" : "ENTER / ESC / B: BACK";
    PrintTextWithBg(CenterConsoleX(TextDisplayWidth(backText), CONSOLE_COLS), panelY + panelH - 2, backText, 14);
    WaitInfoScreenBack();
}

void ShowAboutGameScreen() {
    ClearScreenFast();
    DrawMenuBackground();
    DrawMenuTitle(L(SettingsAbout), 1, CONSOLE_COLS);

    const int panelW = 94;
    const int panelH = 29;
    const int panelX = CenterConsoleX(panelW, CONSOLE_COLS);
    const int panelY = 7;
    DrawFrame(panelX, panelY, panelW, panelH);

    const bool vi = GetLanguage() == Vietnamese;
    int y = panelY + 2;
    const int x = panelX + 5;

    DrawInfoTextLine(x, y, vi ? u8"GI\u1EDAI THI\u1EC6U GAME" : "GAME INTRODUCTION", 14);
    DrawInfoTextLine(x, y, vi ? u8"CARO l\u00E0 tr\u00F2 ch\u01A1i c\u1EDD caro tr\u00EAn console v\u1EDBi phong c\u00E1ch pixel art." : "CARO is a console gomoku game with pixel-art style.");
    DrawInfoTextLine(x, y, vi ? u8"Ng\u01B0\u1EDDi ch\u01A1i c\u00F3 th\u1EC3 \u0111\u00E1nh PvP ho\u1EB7c PvBot, ch\u1ECDn nh\u00E2n v\u1EADt," : "Players can play PvP or PvBot, choose characters,");
    DrawInfoTextLine(x, y, vi ? u8"l\u01B0u/t\u1EA3i v\u00E1n, xem replay, t\u00F9y ch\u1EC9nh nh\u1EA1c v\u00E0 ng\u00F4n ng\u1EEF." : "save/load matches, watch replays, and adjust music/language.");
    y++;

    DrawInfoTextLine(x, y, vi ? u8"TH\u00C0NH VI\u00CAN TH\u1EF0C HI\u1EC6N" : "DEVELOPMENT TEAM", 14);
    DrawInfoTextLine(x, y, u8"24120136 - Tr\u1EA7n L\u00EA Xu\u00E2n T\u00E2n  (Nh\u00F3m tr\u01B0\u1EDFng)");
    DrawInfoTextLine(x, y, u8"24120148 - \u0110inh V\u00F5 Th\u1EE7y Ti\u00EAn");
    DrawInfoTextLine(x, y, u8"24120171 - Nguy\u1EC5n Kh\u00E1nh \u0110\u0103ng");
    DrawInfoTextLine(x, y, u8"24120185 - Nguy\u1EC5n Gia H\u01B0ng");
    DrawInfoTextLine(x, y, u8"24120199 - Tr\u1ECBnh Kim Mai");
    y++;

    DrawInfoTextLine(x, y, vi ? u8"M\u1EE4C TI\u00CAU" : "GOAL", 14);
    DrawInfoTextLine(x, y, vi ? u8"D\u1EF1 \u00E1n t\u1EADp trung v\u00E0o gameplay caro \u0111\u1EA7y \u0111\u1EE7, giao di\u1EC7n r\u00F5 r\u00E0ng," : "The project focuses on complete gomoku gameplay, clear UI,");
    DrawInfoTextLine(x, y, vi ? u8"\u00E2m thanh, l\u01B0u tr\u1EEF, replay v\u00E0 tr\u1EA3i nghi\u1EC7m console \u1ED5n \u0111\u1ECBnh." : "audio, save data, replay and a stable console experience.");

    string backText = vi ? u8"ENTER / ESC / B: QUAY L\u1EA0I" : "ENTER / ESC / B: BACK";
    PrintTextWithBg(CenterConsoleX(TextDisplayWidth(backText), CONSOLE_COLS), panelY + panelH - 2, backText, 14);
    WaitInfoScreenBack();
}

int MainMenu() {
    string options[5] = { L(MainPlayGame), L(MainLoadGame), L(MainGuide), L(MainSettings), L(MainExit) };
    return GraphicalMenu(options, 5, "GAME CARO", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int PlayGameMenu() {
    string options[3] = { L(ModePvp), L(ModePvb), L(Back) };
    return GraphicalMenu(options, 3, L(ModeTitle), BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int DifficultyMenu() {
    string options[4] = { L(DifficultyEasy), L(DifficultyMedium), L(DifficultyHard), L(Back) };
    return GraphicalMenu(options, 4, L(DifficultyTitle), BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int SettingsMenu() {
    string options[5] = {
        L(SettingsClearData),
        L(SettingsMusic),
        L(SettingsLanguage) + CurrentLanguageName(),
        L(SettingsAbout),
        L(Back)
    };
    return GraphicalMenu(options, 5, L(SettingsTitle), BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int GameMenu() {
    string options[5] = { L(PauseExitGame), L(PauseSaveGame), L(PauseLoadGame), L(MainSettings), L(PauseExitMenu) };
    return GraphicalMenu(options, 5, "MENU TAM DUNG", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int MusicMenu() {
    string sfxToggle = isSFXOn ? "ON" : "OFF";
    string musicWord = (GetLanguage() == Vietnamese) ? u8"NH\u1EA0C " : "MUSIC ";
    string musicToggle = MusicStatus() ? L(MusicOff) : L(MusicOn);
    string options[9] = {
        "1. " + musicWord + "1",
        "2. " + musicWord + "2",
        "3. " + musicWord + "3",
        "4. " + musicWord + "4",
        "5. " + musicWord + "5",
        "6. " + musicToggle,
        "7. " + L(MusicVolume),
        "8. " + L(MusicSfx) + "[" + sfxToggle + "]",
        "9. " + L(MusicExit)
    };
    return GenericMenu(options, 9, "");
}

static void FillVolumeRgbRect(int x, int y, int w, int h, int r, int g, int b) {
    printf("\x1b[48;2;%d;%d;%dm", r, g, b);
    string row(w, ' ');
    for (int i = 0; i < h; i++) {
        GotoXY(x, y + i);
        cout << row;
    }
    printf("\x1b[0m");
}

static void DrawVolumeRgbFrame(int x, int y, int w, int h, int fr, int fg, int fb, int br, int bg, int bb) {
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", fr, fg, fb, br, bg, bb);
    GotoXY(x, y);
    cout << "\xE2\x94\x8C";
    for (int i = 0; i < w - 2; i++) cout << "\xE2\x94\x80";
    cout << "\xE2\x94\x90";

    for (int row = 1; row < h - 1; row++) {
        GotoXY(x, y + row);
        cout << "\xE2\x94\x82";
        GotoXY(x + w - 1, y + row);
        cout << "\xE2\x94\x82";
    }

    GotoXY(x, y + h - 1);
    cout << "\xE2\x94\x94";
    for (int i = 0; i < w - 2; i++) cout << "\xE2\x94\x80";
    cout << "\xE2\x94\x98";
    printf("\x1b[0m");
}

static void PrintVolumeRgbText(int x, int y, const string& text, int fr, int fg, int fb, int br, int bg, int bb) {
    GotoXY(x, y);
    printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", fr, fg, fb, br, bg, bb);
    cout << text;
    printf("\x1b[0m");
}

static void PrintVolumeCenteredRgb(int x, int y, int w, const string& text, int fr, int fg, int fb, int br, int bg, int bb) {
    int textX = x + (w - TextDisplayWidth(text)) / 2;
    if (textX < x) textX = x;
    PrintVolumeRgbText(textX, y, text, fr, fg, fb, br, bg, bb);
}

static void DrawVolumePill(int x, int y, int w, const string& text, bool highlighted) {
    int bgR = highlighted ? 255 : 15;
    int bgG = highlighted ? 240 : 82;
    int bgB = highlighted ? 90 : 145;
    int fgR = highlighted ? 20 : 230;
    int fgG = highlighted ? 20 : 245;
    int fgB = highlighted ? 20 : 255;
    int borderR = highlighted ? 255 : 0;
    int borderG = highlighted ? 240 : 255;
    int borderB = highlighted ? 90 : 255;

    FillVolumeRgbRect(x, y, w, 3, bgR, bgG, bgB);
    DrawVolumeRgbFrame(x, y, w, 3, borderR, borderG, borderB, bgR, bgG, bgB);
    PrintVolumeCenteredRgb(x, y + 1, w, text, fgR, fgG, fgB, bgR, bgG, bgB);
}

static void DrawVolumeProgress(int x, int y, int w, int percent) {
    int barW = max(0, w - 2);
    int filled = (barW * percent) / 100;
    if (filled > barW) filled = barW;

    GotoXY(x, y);
    printf("\x1b[38;2;220;245;255m\x1b[48;2;12;16;24m[");
    printf("\x1b[48;2;55;210;190m");
    cout << string(filled, ' ');
    printf("\x1b[48;2;48;54;70m");
    cout << string(barW - filled, ' ');
    printf("\x1b[48;2;12;16;24m]\x1b[0m");

    int markerX = x + 1 + min(barW - 1, max(0, filled));
    GotoXY(markerX, y - 1);
    printf("\x1b[38;2;255;240;90m\x1b[48;2;12;16;24m%c\x1b[0m", 'V');
}

void VolumeMenu() {
    int currentSelect = volumeLevel;
    int lastPercent = -1;

    ClearScreenFast();
    DrawMenuBackground();
    DrawMenuTitle(L(VolumeTitle), 4, CONSOLE_COLS);

    const int panelR = 12;
    const int panelG = 16;
    const int panelB = 24;
    const int accentR = 0;
    const int accentG = 255;
    const int accentB = 255;
    int panelW = 94;
    int panelH = 17;
    int panelX = CenterConsoleX(panelW, CONSOLE_COLS);
    int panelY = 11;
    int barX = panelX + 10;
    int barY = panelY + 8;
    int barW = panelW - 20;

    FillVolumeRgbRect(panelX, panelY, panelW, panelH, panelR, panelG, panelB);
    DrawVolumeRgbFrame(panelX, panelY, panelW, panelH, accentR, accentG, accentB, panelR, panelG, panelB);
    PrintVolumeCenteredRgb(panelX, panelY + 2, panelW, L(VolumeTitle), 255, 245, 80, panelR, panelG, panelB);
    PrintVolumeCenteredRgb(panelX, panelY + 4, panelW, L(VolumeHelp), 0, 255, 255, panelR, panelG, panelB);

    int btnW = 18;
    int gap = 4;
    int totalBtnW = btnW * 3 + gap * 2;
    int btnX = CenterConsoleX(totalBtnW, CONSOLE_COLS);
    int btnY = panelY + 12;
    DrawVolumePill(btnX, btnY, btnW, "A / <-", false);
    DrawVolumePill(btnX + btnW + gap, btnY, btnW, "ENTER", true);
    DrawVolumePill(btnX + (btnW + gap) * 2, btnY, btnW, "D / ->", false);

    while (true) {
        if (currentSelect < 0) currentSelect = 0;
        if (currentSelect > 1000) currentSelect = 1000;

        int percent = currentSelect / 10;

        if (percent != lastPercent) {
            FillVolumeRgbRect(panelX + 4, panelY + 6, panelW - 8, 5, panelR, panelG, panelB);
            string label = L(VolumeLabel) + to_string(percent) + "%";
            PrintVolumeCenteredRgb(panelX, panelY + 6, panelW, label, 255, 245, 80, panelR, panelG, panelB);
            DrawVolumeProgress(barX, barY, barW, percent);

            for (int tick = 0; tick <= 100; tick += 25) {
                int tickX = barX + 1 + ((barW - 2) * tick) / 100;
                string tickText = to_string(tick);
                PrintVolumeRgbText(tickX - (int)tickText.length() / 2, barY + 2, tickText, 120, 245, 255, panelR, panelG, panelB);
            }
            lastPercent = percent;
        }

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
            ClearScreenFast();
            return;
        }
        else if (key == 27) {
            ClearScreenFast();
            return;
        }
    }


}

static string BotDisplayName() {
    if (GetLanguage() == Vietnamese) {
        if (_BOT_DIFFICULTY == 1) return u8"M\u00C1Y (D\u1EC4)";
        if (_BOT_DIFFICULTY == 2) return u8"M\u00C1Y (TRUNG B\u00CCNH)";
        return u8"M\u00C1Y (KH\u00D3)";
    }

    if (_BOT_DIFFICULTY == 1) return "BOT (EASY)";
    if (_BOT_DIFFICULTY == 2) return "BOT (MEDIUM)";
    return "BOT (HARD)";
}

static string TrimPlayerName(const string& name) {
    size_t first = name.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = name.find_last_not_of(' ');
    return name.substr(first, last - first + 1);
}

static string NormalizePlayerName(const string& name) {
    string normalized = TrimPlayerName(name);
    for (char& ch : normalized) {
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    }
    return normalized;
}

static bool IsAllowedPlayerNameChar(int key) {
    unsigned char ch = static_cast<unsigned char>(key);
    return isalnum(ch);
}

static bool CanSubmitNames(const string& p1, const string& p2, bool isBotMode) {
    string name1 = TrimPlayerName(p1);
    string name2 = TrimPlayerName(p2);

    if (name1.empty()) return false;
    if (isBotMode) return true;
    return !name2.empty() && NormalizePlayerName(name1) != NormalizePlayerName(name2);
}

static int ClampTurnTime(int seconds) {
    return max(MIN_TURN_TIME_LIMIT, min(MAX_TURN_TIME_LIMIT, seconds));
}

static int NextNameFocus(int focus, int delta, bool isBotMode) {
    int items[5] = { 0, 1, 2, 3, 4 };
    int count = isBotMode ? 4 : 5;
    if (isBotMode) {
        items[0] = 0;
        items[1] = 2;
        items[2] = 3;
        items[3] = 4;
    }

    int idx = 0;
    for (int i = 0; i < count; i++) {
        if (items[i] == focus) {
            idx = i;
            break;
        }
    }
    idx = (idx + delta + count) % count;
    return items[idx];
}

static void DrawNameSetupField(int x, int y, int w, const string& label, const string& value, bool focused, bool disabled) {
    PrintTextWithBg(x, y, label, disabled ? 15 : 11);

    int bg = disabled ? 8 : (focused ? 1 : 7);
    int fg = disabled ? 7 : 15;
    GotoXY(x + 28, y);
    SetColor(fg, bg);
    string shown = value;
    if (TextDisplayWidth(shown) > w - 2) shown = shown.substr(0, w - 5) + "...";
    cout << " " << shown;
    int remain = w - 1 - TextDisplayWidth(shown);
    if (remain > 0) cout << string(remain, ' ');
    SetColor(15, 0);
}

static void DrawNameSetupButton(int x, int y, int w, const string& text, bool focused, bool enabled) {
    int bg = enabled ? (focused ? 14 : 1) : 8;
    int fg = enabled ? (focused ? 0 : 11) : 7;
    GotoXY(x, y);
    SetColor(fg, bg);
    cout << string(w, ' ');
    GotoXY(x + (w - TextDisplayWidth(text)) / 2, y);
    cout << text;
    SetColor(15, 0);
}

static void DrawNameSetupTimeField(int x, int y, int w, int seconds, bool focused) {
    string label = (GetLanguage() == Vietnamese)
        ? u8"TH\u1EDCI GIAN M\u1ED6I L\u01AF\u1EE2T:"
        : "TURN TIME:";
    PrintTextWithBg(x, y, label, 11);

    int bg = focused ? 14 : 7;
    int fg = focused ? 0 : 15;
    string value = focused ? "< " + to_string(seconds) + "s >" : to_string(seconds) + "s";

    GotoXY(x + 28, y);
    SetColor(fg, bg);
    cout << " " << value;
    int remain = w - 1 - TextDisplayWidth(value);
    if (remain > 0) cout << string(remain, ' ');
    SetColor(15, 0);
}

static void DrawNameSetupScreen(const string& p1, const string& p2, bool isBotMode, int focus, int turnSeconds, bool fullRedraw) {
    const int panelW = 86;
    const int panelH = 20;
    const int panelX = CenterConsoleX(panelW, CONSOLE_COLS);
    const int panelY = 8;
    const int fieldW = 28;
    const int buttonW = 16;
    const bool valid = CanSubmitNames(p1, p2, isBotMode);
    const bool duplicate = !isBotMode
        && !TrimPlayerName(p1).empty()
        && !TrimPlayerName(p2).empty()
        && NormalizePlayerName(p1) == NormalizePlayerName(p2);

    static bool cacheReady = false;
    static string lastP1;
    static string lastP2;
    static int lastFocus = -1;
    static int lastTurnSeconds = -1;
    static bool lastBotMode = false;
    static bool lastValid = false;
    static bool lastDuplicate = false;

    bool forceDraw = fullRedraw || !cacheReady || lastBotMode != isBotMode;

    if (fullRedraw) {
        ClearScreenFast();
        DrawMenuBackground();
        DrawMenuTitle("CARO", 1, CONSOLE_COLS);
        DrawFrame(panelX, panelY, panelW, panelH);

        string title = (GetLanguage() == Vietnamese) ? u8"THI\u1EBET L\u1EACP NG\u01AF\u1EDCI CH\u01A0I" : "PLAYER SETUP";
        PrintTextWithBg(CenterConsoleX(TextDisplayWidth(title), CONSOLE_COLS), panelY + 2, title, 14);

        string help = (GetLanguage() == Vietnamese)
            ? u8"M\u0168I T\u00CAN/TAB: CHUY\u1EC2N \u00D4  |  A/D: CH\u1EC8NH TH\u1EDCI GIAN"
            : "ARROWS/TAB: MOVE  |  A/D: CHANGE TIME";
        PrintTextWithBg(CenterConsoleX(TextDisplayWidth(help), CONSOLE_COLS), panelY + 18, help, 11);
    }

    string p1Label = (GetLanguage() == Vietnamese) ? u8"NG\u01AF\u1EDCI CH\u01A0I 1 (X):" : "PLAYER 1 (X):";
    string p2Label = isBotMode
        ? ((GetLanguage() == Vietnamese) ? u8"M\u00C1Y (O):" : "BOT (O):")
        : ((GetLanguage() == Vietnamese) ? u8"NG\u01AF\u1EDCI CH\u01A0I 2 (O):" : "PLAYER 2 (O):");

    if (forceDraw || p1 != lastP1 || focus == 0 || lastFocus == 0) {
        DrawNameSetupField(panelX + 8, panelY + 5, fieldW, p1Label, p1, focus == 0, false);
    }
    if (forceDraw || p2 != lastP2 || focus == 1 || lastFocus == 1) {
        DrawNameSetupField(panelX + 8, panelY + 7, fieldW, p2Label, p2, focus == 1, isBotMode);
    }
    if (forceDraw || turnSeconds != lastTurnSeconds || focus == 2 || lastFocus == 2) {
        DrawNameSetupTimeField(panelX + 8, panelY + 9, fieldW, turnSeconds, focus == 2);
    }

    if (forceDraw || duplicate != lastDuplicate) {
        FillConsoleRect(panelX + 4, panelY + 11, panelW - 8, 1, 0);

        if (duplicate) {
            string msg = (GetLanguage() == Vietnamese) ? u8"T\u00CAN KH\u00D4NG \u0110\u01AF\u1EE2C TR\u00D9NG" : "NAMES MUST BE UNIQUE";
            PrintTextWithBg(CenterConsoleX(TextDisplayWidth(msg), CONSOLE_COLS), panelY + 11, msg, 12);
        }
    }

    int buttonX = CenterConsoleX(buttonW, CONSOLE_COLS);
    if (forceDraw || valid != lastValid || focus == 3 || lastFocus == 3) {
        string continueLabel = (GetLanguage() == Vietnamese) ? u8"TI\u1EBEP T\u1EE4C" : "NEXT";
        DrawNameSetupButton(buttonX, panelY + 14, buttonW, continueLabel, focus == 3, valid);
    }
    if (forceDraw || focus == 4 || lastFocus == 4) {
        DrawNameSetupButton(buttonX, panelY + 16, buttonW, L(Back), focus == 4, true);
    }

    if ((focus == 0) || (focus == 1 && !isBotMode)) {
        const string& value = (focus == 0) ? p1 : p2;
        int cursorX = panelX + 8 + 28 + 1 + min(TextDisplayWidth(value), fieldW - 2);
        int cursorY = panelY + ((focus == 0) ? 5 : 7);
        UnhideCursor();
        GotoXY(cursorX, cursorY);
    }
    else {
        HideCursor();
    }

    cacheReady = true;
    lastP1 = p1;
    lastP2 = p2;
    lastFocus = focus;
    lastTurnSeconds = turnSeconds;
    lastBotMode = isBotMode;
    lastValid = valid;
    lastDuplicate = duplicate;
}

static bool InputPlayerNamesSetup(bool isBotMode) {
    string p1 = "";
    string p2 = isBotMode ? BotDisplayName() : "";
    int setupTurnTime = ClampTurnTime(turnTimeLimit);
    int focus = 0;

    DrawNameSetupScreen(p1, p2, isBotMode, focus, setupTurnTime, true);

    while (true) {
        int key = _getch();
        bool redraw = true;
        bool playSound = false;

        if (key == 0 || key == 224) {
            key = _getch();
            if (key == 72) {
                focus = NextNameFocus(focus, -1, isBotMode);
                playSound = true;
            }
            else if (key == 80) {
                focus = NextNameFocus(focus, 1, isBotMode);
                playSound = true;
            }
            else if ((key == 75 || key == 77) && focus == 2) {
                int delta = (key == 75) ? -TURN_TIME_STEP : TURN_TIME_STEP;
                setupTurnTime = ClampTurnTime(setupTurnTime + delta);
                playSound = true;
            }
            else {
                redraw = false;
            }
            if (playSound) PlayMenuSound();
            if (redraw) DrawNameSetupScreen(p1, p2, isBotMode, focus, setupTurnTime, false);
            continue;
        }

        if (key == 27) {
            HideCursor();
            return false;
        }
        if (key == 9) {
            focus = NextNameFocus(focus, 1, isBotMode);
            playSound = true;
        }
        else if (key == 13) {
            if (focus == 3 && CanSubmitNames(p1, p2, isBotMode)) {
                _PLAYER1_NAME = TrimPlayerName(p1);
                _PLAYER2_NAME = isBotMode ? BotDisplayName() : TrimPlayerName(p2);
                turnTimeLimit = setupTurnTime;
                timeLeft = turnTimeLimit;
                HideCursor();
                PlayMenuSound();
                CharacterASelect = CharacterSelectionMenu(-1, L(ChooseCharacterP1));
                if (isBotMode) {
                    CharacterBSelect = (CharacterASelect == 4) ? 0 : 4;
                }
                else {
                    CharacterBSelect = CharacterSelectionMenu(CharacterASelect, L(ChooseCharacterP2));
                }
                return true;
            }
            if (focus == 4) {
                HideCursor();
                PlayMenuSound();
                return false;
            }
            focus = NextNameFocus(focus, 1, isBotMode);
            playSound = true;
        }
        else if (focus == 2 && (toupper((unsigned char)key) == 'A' || toupper((unsigned char)key) == 'D')) {
            int delta = (toupper((unsigned char)key) == 'A') ? -TURN_TIME_STEP : TURN_TIME_STEP;
            setupTurnTime = ClampTurnTime(setupTurnTime + delta);
            playSound = true;
        }
        else {
            string* target = nullptr;
            if (focus == 0) target = &p1;
            else if (focus == 1 && !isBotMode) target = &p2;

            if (target != nullptr) {
                if (key == 8) {
                    if (!target->empty()) target->pop_back();
                }
                else if (IsAllowedPlayerNameChar(key) && TextDisplayWidth(*target) < 15) {
                    target->push_back(static_cast<char>(key));
                }
            }
            else {
                redraw = false;
            }
        }

        if (playSound) PlayMenuSound();
        if (redraw) DrawNameSetupScreen(p1, p2, isBotMode, focus, setupTurnTime, false);
    }
}

bool InputPlayerNames(bool isBotMode) {
    return InputPlayerNamesSetup(isBotMode);
}

static int NextEnabledCharacter(int current, int delta, int size, int disabledOption) {
    int next = current;
    do {
        next = (next + delta + size) % size;
    } while (next == disabledOption);
    return next;
}

static void DrawCharacterGrid(int currentSelect, int size, int disabledOption) {
    const int cols = 3;
    const int boxW = 30;
    const int boxH = 12;
    const int gapX = 0;
    const int gapY = 1;
    const int totalW = cols * boxW + (cols - 1) * gapX;
    const int startX = CenterConsoleX(totalW, CONSOLE_COLS);
    const int startY = 3;

    for (int i = 0; i < size; i++) {
        int col = i % cols;
        int row = i / cols;
        int x = startX + col * (boxW + gapX);
        int y = startY + row * (boxH + gapY);
        DrawCharacterPreview(i, x, y, boxW, boxH, i == currentSelect, i == disabledOption);
    }
}

static void DrawCharacterGridItem(int index, bool selected, int disabledOption) {
    const int cols = 3;
    const int boxW = 30;
    const int boxH = 12;
    const int gapX = 0;
    const int gapY = 1;
    const int totalW = cols * boxW + (cols - 1) * gapX;
    const int startX = CenterConsoleX(totalW, CONSOLE_COLS);
    const int startY = 3;
    int col = index % cols;
    int row = index / cols;
    int x = startX + col * (boxW + gapX);
    int y = startY + row * (boxH + gapY);
    DrawCharacterPreview(index, x, y, boxW, boxH, selected, index == disabledOption);
}

int GenericCharacterMenu(string options[], int size, string title, int disabledOption) {
    int currentSelect = 0;
    int lastSelect = -1;

    if (disabledOption >= 0 && disabledOption < size && currentSelect == disabledOption) {
        currentSelect = NextEnabledCharacter(currentSelect, 1, size, disabledOption);
    }

    ClearScreenFast();
    DrawMenuBackground();
    if (!title.empty() && title != "CHARACTER MENU") {
        PrintTextWithBg(CenterConsoleX(TextDisplayWidth(title), CONSOLE_COLS), 1, title, 14);
    }

    while (true) {
        if (currentSelect != lastSelect) {
            if (lastSelect == -1) {
                DrawCharacterGrid(currentSelect, size, disabledOption);
            }
            else {
                DrawCharacterGridItem(lastSelect, false, disabledOption);
                DrawCharacterGridItem(currentSelect, true, disabledOption);
            }
            lastSelect = currentSelect;
        }

        int key = ReadMenuKey();
        if (key == 'A' || key == 75) {
            currentSelect = NextEnabledCharacter(currentSelect, -1, size, disabledOption);
            PlayMenuSound();
        }
        else if (key == 'D' || key == 77) {
            currentSelect = NextEnabledCharacter(currentSelect, 1, size, disabledOption);
            PlayMenuSound();
        }
        else if (key == 'W' || key == 72) {
            currentSelect = NextEnabledCharacter(currentSelect, -3, size, disabledOption);
            PlayMenuSound();
        }
        else if (key == 'S' || key == 80) {
            currentSelect = NextEnabledCharacter(currentSelect, 3, size, disabledOption);
            PlayMenuSound();
        }
        else if (key == 13) {
            PlayMenuSound();
            return currentSelect;
        }
        else if (key == 27) {
            PlayMenuSound();
            return currentSelect;
        }
    }
}

int CharacterSelectionMenu(int disabledOption, string chooserTitle) {
    string options[5] = { "1. KNIGHT", "2. ASSASSIN", "3. VAMPIRE", "4. PALADIN", "5. OFFICER" };
    return GenericCharacterMenu(options, 5, chooserTitle.empty() ? "CHARACTER MENU" : chooserTitle, disabledOption);
}

void DrawFrame(int x, int y, int w, int h) {
    printf("\x1b[38;2;255;255;255m\x1b[48;2;15;15;20m");
    GotoXY(x, y);
    cout << "\xE2\x94\x8C"; for (int i = 0; i < w - 2; i++) cout << "\xE2\x94\x80"; cout << "\xE2\x94\x90";
    for (int i = 1; i < h - 1; i++) {
        GotoXY(x, y + i); cout << "\xE2\x94\x82";
        GotoXY(x + 1, y + i); cout << string(w - 2, ' ');
        GotoXY(x + w - 1, y + i); cout << "\xE2\x94\x82";
    }
    GotoXY(x, y + h - 1);
    cout << "\xE2\x94\x94"; for (int i = 0; i < w - 2; i++) cout << "\xE2\x94\x80"; cout << "\xE2\x94\x98";
    printf("\x1b[0m");
}

void DrawMenuTitle(const string& title, int y, int consoleW) {
    if (title == "GAME CARO" || title == "CARO") {
        DrawTitleArt(CenterConsoleX(TITLE_ART_W, consoleW), y);
    }
    else if (!title.empty() && title != "MENU TAM DUNG" && title != "CHARACTER MENU") {
        int titleW = TextDisplayWidth(title) + 16;
        int titleX = CenterConsoleX(titleW, consoleW);
        DrawFrame(titleX, y, titleW, 5);
        int textX = CenterConsoleX(TextDisplayWidth(title), consoleW);
        PrintTextWithBg(textX, y + 2, title, 14);
    }
}
