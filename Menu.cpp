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

static void FillConsoleRect(int x, int y, int w, int h, int bgColor) {
    SetColor(0, bgColor);
    string row(w, ' ');
    for (int i = 0; i < h; i++) {
        GotoXY(x, y + i);
        cout << row;
    }
    SetColor(15, 0);
}

static void DrawNameInputFrame(int frameX, int y, int frameW, const string& label, const string& value, bool active) {
    const int inputW = 20;
    const int inputX = frameX + frameW - inputW - 7;

    DrawFrame(frameX, y, frameW, 3);
    FillConsoleRect(inputX, y + 1, inputW, 1, active ? 1 : 8);

    PrintTextWithBg(frameX + 4, y + 1, label, active ? 11 : 14);

    if (!active && !value.empty()) {
        int valueX = inputX;
        GotoXY(valueX, y + 1);
        SetColor(15, 8);
        cout << value.substr(0, inputW);
        SetColor(15, 0);
    }
}

static void BeginNameInput(int inputX, int inputY, int inputW) {
    FillConsoleRect(inputX, inputY, inputW, 1, 1);
    GotoXY(inputX, inputY);
    SetColor(15, 1);
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
    system("cls");
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
    string options[4] = { L(TextId::MainPlayGame), L(TextId::MainLoadGame), L(TextId::MainSettings), L(TextId::MainExit) };
    return GraphicalMenu(options, 4, "GAME CARO", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int PlayGameMenu() {
    string options[3] = { L(TextId::ModePvp), L(TextId::ModePvb), L(TextId::Back) };
    return GraphicalMenu(options, 3, L(TextId::ModeTitle), BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int DifficultyMenu() {
    string options[4] = { L(TextId::DifficultyEasy), L(TextId::DifficultyMedium), L(TextId::DifficultyHard), L(TextId::Back) };
    return GraphicalMenu(options, 4, L(TextId::DifficultyTitle), BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int SettingsMenu() {
    string options[4] = {
        L(TextId::SettingsClearData),
        L(TextId::SettingsMusic),
        L(TextId::SettingsLanguage) + CurrentLanguageName(),
        L(TextId::Back)
    };
    return GraphicalMenu(options, 4, L(TextId::SettingsTitle), BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int GameMenu() {
    string options[5] = { L(TextId::PauseExitGame), L(TextId::PauseSaveGame), L(TextId::PauseLoadGame), L(TextId::MainSettings), L(TextId::PauseExitMenu) };
    return GraphicalMenu(options, 5, "MENU TAM DUNG", BTN_NORMAL, BTN_HOVER, BTN_NORMAL_W, BTN_NORMAL_H);
}

int MusicMenu() {
    string sfxToggle = isSFXOn ? "ON" : "OFF";
    string musicWord = (GetLanguage() == GameLanguage::Vietnamese) ? u8"NH\u1EA0C " : "MUSIC ";
    string options[9] = {
        "1. " + musicWord + "1",
        "2. " + musicWord + "2",
        "3. " + musicWord + "3",
        "4. " + musicWord + "4",
        "5. " + musicWord + "5",
        "6. " + L(TextId::MusicOff),
        "7. " + L(TextId::MusicVolume),
        "8. " + L(TextId::MusicSfx) + "[" + sfxToggle + "]",
        "9. " + L(TextId::MusicExit)
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

    system("cls");
    DrawMenuBackground();
    DrawMenuTitle(L(TextId::VolumeTitle), 4, CONSOLE_COLS);

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
    PrintVolumeCenteredRgb(panelX, panelY + 2, panelW, L(TextId::VolumeTitle), 255, 245, 80, panelR, panelG, panelB);
    PrintVolumeCenteredRgb(panelX, panelY + 4, panelW, L(TextId::VolumeHelp), 0, 255, 255, panelR, panelG, panelB);

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
            string label = L(TextId::VolumeLabel) + to_string(percent) + "%";
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

static string BotDisplayName() {
    if (_BOT_DIFFICULTY == 1) return "BOT (DE)";
    if (_BOT_DIFFICULTY == 2) return "BOT (TRUNG BINH)";
    return "BOT (KHO)";
}

static bool CanSubmitNames(const string& p1, const string& p2, bool isBotMode) {
    if (p1.empty()) return false;
    if (isBotMode) return true;
    return !p2.empty() && p1 != p2;
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
    string label = (GetLanguage() == GameLanguage::Vietnamese)
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
    const bool duplicate = !isBotMode && !p1.empty() && p1 == p2;

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
        system("cls");
        DrawMenuBackground();
        DrawMenuTitle("CARO", 1, CONSOLE_COLS);
        DrawFrame(panelX, panelY, panelW, panelH);

        string title = (GetLanguage() == GameLanguage::Vietnamese) ? u8"THI\u1EBET L\u1EACP NG\u01AF\u1EDCI CH\u01A0I" : "PLAYER SETUP";
        PrintTextWithBg(CenterConsoleX(TextDisplayWidth(title), CONSOLE_COLS), panelY + 2, title, 14);

        string help = (GetLanguage() == GameLanguage::Vietnamese)
            ? u8"M\u0168I T\u00CAN/TAB: CHUY\u1EC2N \u00D4  |  A/D: CH\u1EC8NH TH\u1EDCI GIAN"
            : "ARROWS/TAB: MOVE  |  A/D: CHANGE TIME";
        PrintTextWithBg(CenterConsoleX(TextDisplayWidth(help), CONSOLE_COLS), panelY + 18, help, 11);
    }

    string p1Label = (GetLanguage() == GameLanguage::Vietnamese) ? u8"NG\u01AF\u1EDCI CH\u01A0I 1 (X):" : "PLAYER 1 (X):";
    string p2Label = isBotMode
        ? ((GetLanguage() == GameLanguage::Vietnamese) ? u8"NG\u01AF\u1EDCI CH\u01A0I 2 (O):" : "PLAYER 2 (O):")
        : ((GetLanguage() == GameLanguage::Vietnamese) ? u8"NG\u01AF\u1EDCI CH\u01A0I 2 (O):" : "PLAYER 2 (O):");

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
            string msg = (GetLanguage() == GameLanguage::Vietnamese) ? u8"T\u00CAN KH\u00D4NG \u0110\u01AF\u1EE2C TR\u00D9NG" : "NAMES MUST BE UNIQUE";
            PrintTextWithBg(CenterConsoleX(TextDisplayWidth(msg), CONSOLE_COLS), panelY + 11, msg, 12);
        }
    }

    int buttonX = CenterConsoleX(buttonW, CONSOLE_COLS);
    if (forceDraw || valid != lastValid || focus == 3 || lastFocus == 3) {
        DrawNameSetupButton(buttonX, panelY + 14, buttonW, "NEXT", focus == 3, valid);
    }
    if (forceDraw || focus == 4 || lastFocus == 4) {
        DrawNameSetupButton(buttonX, panelY + 16, buttonW, L(TextId::Back), focus == 4, true);
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
                _PLAYER1_NAME = p1;
                _PLAYER2_NAME = isBotMode ? BotDisplayName() : p2;
                turnTimeLimit = setupTurnTime;
                timeLeft = turnTimeLimit;
                HideCursor();
                PlayMenuSound();
                CharacterASelect = CharacterSelectionMenu(-1, L(TextId::ChooseCharacterP1));
                if (isBotMode) {
                    CharacterBSelect = (CharacterASelect == 4) ? 0 : 4;
                }
                else {
                    CharacterBSelect = CharacterSelectionMenu(CharacterASelect, L(TextId::ChooseCharacterP2));
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
                else if ((isalnum((unsigned char)key) || key == ' ' || key == '_' || key == '-') && TextDisplayWidth(*target) < 15) {
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
#if 0
    system("cls");
    DrawMenuBackground(); // Nền xịn từ Menu cũ
    DrawMenuTitle("GAME CARO", 1, CONSOLE_COLS);

    int consoleW = CONSOLE_COLS;
    int frameW = 72;
    int frameX = CenterConsoleX(frameW, consoleW);
    int inputW = 20;
    int inputX = frameX + frameW - inputW - 7;
    int p1Y = 11;
    int p2Y = 17;

    // Frame Nhập Tên Player 1
    DrawNameInputFrame(frameX, p1Y, frameW, L(TextId::EnterPlayerX), "", true);
    UnhideCursor();
    BeginNameInput(inputX, p1Y + 1, inputW);

    // Tích hợp hệ thống quét pixel nền thông minh
    _PLAYER1_NAME = TypeName();
    printf("\x1b[0m");
    HideCursor();
    
    // Xóa màn hình và vẽ lại background sau khi nhập xong Player 1
    system("cls");
    DrawMenuBackground();
    DrawMenuTitle("GAME CARO", 1, consoleW);
    DrawNameInputFrame(frameX, p1Y, frameW, L(TextId::EnterPlayerX), _PLAYER1_NAME, false);

    // Frame Nhập Tên Player 2 (Hoặc Bot)
    if (isBotMode) {
        if (_BOT_DIFFICULTY == 1) _PLAYER2_NAME = "Bot (De)";
        else if (_BOT_DIFFICULTY == 2) _PLAYER2_NAME = "Bot (Trung Binh)";
        else _PLAYER2_NAME = "Bot (Kho)";

        DrawNameInputFrame(frameX, p2Y, frameW, L(TextId::EnterPlayerBot), _PLAYER2_NAME, false);
        Sleep(1000);
    }
    else {
        while (true) {
            DrawNameInputFrame(frameX, p2Y, frameW, L(TextId::EnterPlayerO), "", true);

            // Xóa vùng text nhập tên bên trong Frame
            UnhideCursor();
            BeginNameInput(inputX, p2Y + 1, inputW);

            _PLAYER2_NAME = TypeName();
            printf("\x1b[0m");
            HideCursor();

            if (_PLAYER1_NAME == _PLAYER2_NAME) {
                string errorMsg = L(TextId::DuplicateName);
                int errorX = CenterConsoleX(TextDisplayWidth(errorMsg), consoleW);
                PrintTextWithBg(errorX, p2Y + 4, errorMsg, 12);
                _getch();

                // Tẩy xóa dòng báo lỗi
                PrintTextWithBg(errorX, p2Y + 4, string(TextDisplayWidth(errorMsg), ' '), 15);
            }
            else {
                break;
            }
        }
    }

    CharacterASelect = CharacterSelectionMenu(-1, L(TextId::ChooseCharacterP1));

    if (isBotMode) {
        CharacterBSelect = (CharacterASelect == 4) ? 0 : 4;
    }
    else {
        CharacterBSelect = CharacterSelectionMenu(CharacterASelect, L(TextId::ChooseCharacterP2));
    }
#endif
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
    {
        int currentSelect = 0;
        int lastSelect = -1;

        if (disabledOption >= 0 && disabledOption < size && currentSelect == disabledOption) {
            currentSelect = NextEnabledCharacter(currentSelect, 1, size, disabledOption);
        }

        system("cls");
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

    int currentSelect = 0;
    int marginX = 78;
    int lastSelect = -1;

    if (disabledOption >= 0 && disabledOption < size && currentSelect == disabledOption) {
        currentSelect = (currentSelect + 1) % size;
    }

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
            lastSelect = currentSelect;
        }

        // Vẽ nhân vật trước khi đợi nhập phím
        // Vẽ Menu ở phía bên phải
        SetColor(12, 15);

        for (int i = 0; i < size; i++) {
            if (i == disabledOption) {
                SetColor(8, 0);
                GotoXY(marginX + 5, 12 + i * 2);
                cout << "   " << options[i] << " - " << L(TextId::Chosen) << "   ";
            }
            else if (i == currentSelect) {
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
            do {
                currentSelect--;
                if (currentSelect < 0) currentSelect = size - 1;
            } while (currentSelect == disabledOption);
            PlayMenuSound();
        }
        else if (key == 'S' || key == 80) {
            do {
                currentSelect++;
                if (currentSelect >= size) currentSelect = 0;
            } while (currentSelect == disabledOption);
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
