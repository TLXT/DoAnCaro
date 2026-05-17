#include "CharSprite.h"
#include "ControlConsole.h"
#include "GameStatus.h"
#include <iostream>
#include <conio.h>

#include "btn_normal.h"
#include "btn_hover.h"
using namespace std;

// ============================================================
//  CharSprite.cpp
// ============================================================

// Danh sách nhân vật có thể chọn
const CharOption CHAR_LIST[] = {
    { "Chien Binh (X)", SPRITE_WARRIOR },
    { "Phap Su  (O)",   SPRITE_WIZARD  },
    // Thêm nhân vật mới tại đây sau khi chạy Python:
    // { "Ten nhan vat", TEN_SPRITE },
};
const int CHAR_COUNT = sizeof(CHAR_LIST) / sizeof(CHAR_LIST[0]);

int _PLAYER1_CHAR = 0;  // Mặc định nhân vật 1 = WARRIOR
int _PLAYER2_CHAR = 1;  // Mặc định nhân vật 2 = WIZARD

// ============================================================
// DrawSprite: Vẽ sprite tại (startX, startY) trên console
// Mỗi pixel = 2 ký tự space với màu nền (bgColor)
// Pixel có màu SPRITE_TRANSPARENT thì bỏ qua (giữ nền)
// ============================================================
void DrawSprite(const int sprite[][SPRITE_W], int h, int startX, int startY) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < SPRITE_W; x++) {
            int colorIdx = sprite[y][x];
            GotoXY(startX + x * 2, startY + y);
            if (colorIdx == SPRITE_TRANSPARENT) {
                // Giữ màu nền mặc định (trắng)
                SetColor(0, 15);
                cout << "  ";
            }
            else {
                // Vẽ 2 ký tự space với màu nền = colorIdx
                SetColor(colorIdx, colorIdx);
                cout << "  ";
            }
        }
    }
    SetColor(0, 15);  // Reset màu
}

// ============================================================
// ClearSprite: Xóa vùng sprite (tô trắng)
// ============================================================
void ClearSprite(int startX, int startY, int w, int h) {
    for (int y = 0; y < h; y++) {
        GotoXY(startX, startY + y);
        SetColor(0, 15);
        for (int x = 0; x < w * 2; x++) cout << " ";
    }
}

// ============================================================
// DrawBothSprites: Vẽ cả 2 nhân vật lên màn hình game
// Gọi sau StartGame() để hiển thị sprite 2 bên bàn cờ
// ============================================================
void DrawBothSprites() {
    const int* p1Sprite = reinterpret_cast<const int*>(CHAR_LIST[_PLAYER1_CHAR].sprite);
    const int* p2Sprite = reinterpret_cast<const int*>(CHAR_LIST[_PLAYER2_CHAR].sprite);

    DrawSprite(CHAR_LIST[_PLAYER1_CHAR].sprite, SPRITE_H, LEFT_SPRITE_X, SPRITE_START_Y);
    DrawSprite(CHAR_LIST[_PLAYER2_CHAR].sprite, SPRITE_H, RIGHT_SPRITE_X, SPRITE_START_Y);

    SetColor(0, 15);
    // Vẽ tên nhân vật bên dưới sprite
    GotoXY(LEFT_SPRITE_X, SPRITE_START_Y + SPRITE_H + 1);
    cout << CHAR_LIST[_PLAYER1_CHAR].name;
    GotoXY(RIGHT_SPRITE_X, SPRITE_START_Y + SPRITE_H + 1);
    cout << CHAR_LIST[_PLAYER2_CHAR].name;
}

// Triển khai hàm vẽ khung câu hỏi Replay và tốc độ
void DrawFrame(int x, int y, int w, int h) {
    // Đặt màu: 0 (chữ đen) - 15 (nền trắng)
    SetColor(0, 15);

	// Vẽ từng dòng từ trên xuống dưới: w: rộng, h: cao
    for (int i = 0; i < h; i++) {
        GotoXY(x, y + i);

        // In ra w khoảng trắng để lấp đầy 1 dòng nền trắng
        for (int j = 0; j < w; j++) {
            cout << " ";
        }
    }

    // Reset màu lại sau khi vẽ xong
    SetColor(0, 15);
}

// ============================================================
// CharacterSelectMenu: Menu chọn nhân vật trước khi chơi
// Hiển thị preview sprite bên trái/phải khi di chuyển chọn
// ============================================================
void CharacterSelectMenu() {
    int maxDisplay = 5; // Chỉ hiển thị tối đa 5 lựa chọn để không tràn màn hình
    int btnCols = BTN_NORMAL_W * 2;
    int startX = 42;    // Giữ nguyên tọa độ cột của danh sách lựa chọn
    int startY_Base = 6;

    // Lấy màu nền của nút
    int bgNorm = BTN_NORMAL[BTN_NORMAL_H / 2][BTN_NORMAL_W / 2];
    int bgHov = BTN_HOVER[BTN_HOVER_H / 2][BTN_HOVER_W / 2];

    // --- Chọn nhân vật cho Player 1 ---
    system("color F0");
    int sel1 = 0;
    int lastSel1 = -1;
    int startIndex1 = 0;
    int lastStartIndex1 = -1;

    while (true) {
        // NẾU BỊ CUỘN TRANG: Xóa và vẽ lại tiêu đề
        if (startIndex1 != lastStartIndex1) {
            system("cls");
            SetColor(12, 15);
            GotoXY(40, 2); cout << "=== CHON NHAN VAT CHO PLAYER 1 (X) ===";
            GotoXY(40, 3); cout << "  (W/S de chon, Enter de xac nhan)   ";
            lastStartIndex1 = startIndex1;
            lastSel1 = -1; // Ép vẽ lại toàn bộ nút
        }

        // NẾU CHUYỂN LỰA CHỌN: Cập nhật lại nút và Preview
        if (sel1 != lastSel1) {
            int endIdx = min(startIndex1 + maxDisplay, CHAR_COUNT);

            // Vẽ danh sách nút
            for (int i = startIndex1; i < endIdx; i++) {
                int rowOffset = i - startIndex1;
                int startY = startY_Base + rowOffset * (BTN_NORMAL_H + 1);

                if (i == sel1) {
                    DrawSolidImage(BTN_HOVER, BTN_HOVER_W, BTN_HOVER_H, startX, startY);
                    GotoXY(startX + (btnCols - CHAR_LIST[i].name.length()) / 2, startY + BTN_HOVER_H / 2);
                    SetColor(0, bgHov); cout << CHAR_LIST[i].name;
                }
                else {
                    DrawSolidImage(BTN_NORMAL, BTN_NORMAL_W, BTN_NORMAL_H, startX, startY);
                    GotoXY(startX + (btnCols - CHAR_LIST[i].name.length()) / 2, startY + BTN_NORMAL_H / 2);
                    SetColor(0, bgNorm); cout << CHAR_LIST[i].name;
                }
            }

            // Xóa Sprite nhân vật cũ bằng khoảng trắng trước khi vẽ đè
            SetColor(0, 15);
            for (int r = 0; r < SPRITE_H + 5; r++) {
                GotoXY(10, 6 + r);
                cout << string(30, ' '); // Quét 30 khoảng trắng để làm sạch vùng Preview
            }

            // Vẽ Preview sprite nhân vật đang chọn (giữ nguyên logic gốc)
            GotoXY(10, 5); cout << "Preview:";
            DrawSprite(CHAR_LIST[sel1].sprite, SPRITE_H, 10, 6);

            lastSel1 = sel1;
        }

        SetColor(0, 15);
        int key = toupper(_getch());
        if (key == 0 || key == 224) key = _getch(); // phím mũi tên

        if (key == 'W' || key == 72) {
            sel1--;
            if (sel1 < 0) sel1 = CHAR_COUNT - 1;
        }
        else if (key == 'S' || key == 80) {
            sel1++;
            if (sel1 >= CHAR_COUNT) sel1 = 0;
        }
        else if (key == 13) {
            _PLAYER1_CHAR = sel1;
            break;
        }

        // Logic cuộn trang
        if (sel1 < startIndex1) startIndex1 = sel1;
        else if (sel1 >= startIndex1 + maxDisplay) startIndex1 = sel1 - maxDisplay + 1;
    }

    // --- Chọn nhân vật cho Player 2 (hoặc Bot) ---
    extern bool _BOT_MODE;
    if (_BOT_MODE) {
        _PLAYER2_CHAR = (_PLAYER1_CHAR == 0) ? 1 : 0;
        return;
    }

    int sel2 = (_PLAYER1_CHAR == 0) ? 1 : 0;
    int lastSel2 = -1;

    // Nếu gợi ý ban đầu nằm ngoài tầm hiển thị, phải setup lại startIndex
    int startIndex2 = 0;
    if (sel2 >= maxDisplay) startIndex2 = sel2 - maxDisplay + 1;
    int lastStartIndex2 = -1;

    while (true) {
        if (startIndex2 != lastStartIndex2) {
            system("cls");
            SetColor(1, 15);
            GotoXY(40, 2); cout << "=== CHON NHAN VAT CHO PLAYER 2 (O) ===";
            GotoXY(40, 3); cout << "  (W/S de chon, Enter de xac nhan)   ";
            lastStartIndex2 = startIndex2;
            lastSel2 = -1;
        }

        if (sel2 != lastSel2) {
            int endIdx = min(startIndex2 + maxDisplay, CHAR_COUNT);
            for (int i = startIndex2; i < endIdx; i++) {
                int rowOffset = i - startIndex2;
                int startY = startY_Base + rowOffset * (BTN_NORMAL_H + 1);

                if (i == sel2) {
                    DrawSolidImage(BTN_HOVER, BTN_HOVER_W, BTN_HOVER_H, startX, startY);
                    GotoXY(startX + (btnCols - CHAR_LIST[i].name.length()) / 2, startY + BTN_HOVER_H / 2);
                    SetColor(0, bgHov); cout << CHAR_LIST[i].name;
                }
                else {
                    DrawSolidImage(BTN_NORMAL, BTN_NORMAL_W, BTN_NORMAL_H, startX, startY);
                    GotoXY(startX + (btnCols - CHAR_LIST[i].name.length()) / 2, startY + BTN_NORMAL_H / 2);
                    SetColor(0, bgNorm); cout << CHAR_LIST[i].name;
                }
            }

            SetColor(0, 15);
            for (int r = 0; r < SPRITE_H + 5; r++) {
                GotoXY(10, 6 + r);
                cout << string(30, ' ');
            }
            GotoXY(10, 5); cout << "Preview:";
            DrawSprite(CHAR_LIST[sel2].sprite, SPRITE_H, 10, 6);

            lastSel2 = sel2;
        }

        SetColor(0, 15);
        int key = toupper(_getch());
        if (key == 0 || key == 224) key = _getch();

        if (key == 'W' || key == 72) {
            sel2--;
            if (sel2 < 0) sel2 = CHAR_COUNT - 1;
        }
        else if (key == 'S' || key == 80) {
            sel2++;
            if (sel2 >= CHAR_COUNT) sel2 = 0;
        }
        else if (key == 13) {
            _PLAYER2_CHAR = sel2;
            break;
        }

        if (sel2 < startIndex2) startIndex2 = sel2;
        else if (sel2 >= startIndex2 + maxDisplay) startIndex2 = sel2 - maxDisplay + 1;
    }
}
