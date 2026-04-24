#include "graphics.h"

int screen_dirty = 0;

int rgb(int r, int g, int b) {
    return r << 11 | g << 5 | b;
}

void Draw(FbDesc *fb, int x, int y, int r, int g, int b) {
    if ((uint32_t)x >= fb->width || (uint32_t)y >= fb->height) return;
    screen_dirty = 1;
    fb->shadow[y * fb->width + x] = (uint16_t)rgb(r, g, b);
}

void ClearScreen(FbDesc *fb, int r, int g, int b) {
    uint16_t color = (uint16_t)rgb(r, g, b);
    uint32_t total = fb->width * fb->height;
    uint16_t *buf = fb->shadow;
    for (uint32_t i = 0; i < total; i++)
        buf[i] = color;
    screen_dirty = 1;
}

void DrawRect(FbDesc *fb, int x, int y, int w, int h, int r, int g, int b) {
    for (int j = y; j < y + h; j++)
        for (int i = x; i < x + w; i++)
            Draw(fb, i, j, r, g, b);
}

void DrawCharacter(FbDesc *fb, int (*f)(int,int), int fw, int fh, char character, int x, int y, int r, int g, int b) {
    for (int j = 0; j < fh; j++) {
        unsigned int row = (*f)((int)character, j);
        int shift = fw - 1;
        for (int i = 0; i < fw; i++) {
            if ((row >> shift) & 1)
                Draw(fb, x + i, y + j, r, g, b);
            shift--;
        }
    }
}

void DrawString(FbDesc *fb, int (*f)(int,int), int fw, int fh, char *string, int x, int y, int r, int g, int b) {
    int i = 0, j = 0;
    for (int k = 0; string[k] != 0; k++) {
        if (string[k] != '\n')
            DrawCharacter(fb, f, fw, fh, string[k], x + i, y + j, r, g, b);
        i += fw - (fw / 5);
        if (string[k] == '\n') {
            i = 0;
            j += fh;
        }
    }
}

void DrawMouse(FbDesc *fb, int x, int y, int r, int g, int b) {
    static const unsigned int mouse[10] = {
        0b1111111111,
        0b1111111110,
        0b1111111100,
        0b1111111000,
        0b1111110000,
        0b1111100000,
        0b1111000000,
        0b1110000000,
        0b1100000000,
        0b1000000000,
    };
    for (int j = 0; j < 10; j++) {
        int shift = 9;
        for (int i = 0; i < 10; i++) {
            if ((mouse[j] >> shift) & 1)
                Draw(fb, x + i, y + j, r, g, b);
            shift--;
        }
    }
}

void DrawCircle(FbDesc *fb, int x, int y, int radius, int r, int g, int b) {
    int rr = radius * radius;
    for (int j = -radius; j < radius; j++)
        for (int i = -radius; i < radius; i++)
            if (i*i + j*j <= rr)
                Draw(fb, x + i, y + j, r, g, b);
}

void Flush(FbDesc *fb) {
    if (!screen_dirty) return;
    screen_dirty = 0;
    uint32_t total = fb->width * fb->height;
    uint16_t *src = fb->shadow;
    uint16_t *dst = (uint16_t *)fb->fb_phys;
    for (uint32_t i = 0; i < total; i++)
        dst[i] = src[i];
}
