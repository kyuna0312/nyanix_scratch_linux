#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;   /* bytes per row */
    uint32_t bpp;     /* bits per pixel (16 for RGB565, 32 for XRGB8888) */
    uptr     fb_phys; /* physical framebuffer address */
    uint16_t *shadow; /* off-screen draw buffer */
} FbDesc;

/* ── drawing globals ── */
extern int screen_dirty;
extern int mx, my;
extern int left_clicked, right_clicked;
extern int mouse_possessed_task_id;
extern int Scancode;
extern int backspace_pressed;
extern volatile int pit_ticks;
extern const int font_arial_width;
extern const int font_arial_height;

/* ── task globals ── */
#define task_params_length 10

struct Task {
    int  priority;
    int  taskId;
    char ca1[100];
    int  i1;
    char name[16];
    int  (*function)(int taskId);
};

struct FileEntry {
    char name[30];
    int  isDirectory;
    int  size;
    int  modified;
};

extern struct Task tasks[256];
extern int iparams[100];
extern int TasksLength;

/* ── color ── */
int rgb(int r, int g, int b);

/* ── graphics (FbDesc-based) ── */
void Draw(FbDesc *fb, int x, int y, int r, int g, int b);
void ClearScreen(FbDesc *fb, int r, int g, int b);
void DrawRect(FbDesc *fb, int x, int y, int w, int h, int r, int g, int b);
void DrawMouse(FbDesc *fb, int x, int y, int r, int g, int b);
void DrawCircle(FbDesc *fb, int x, int y, int radius, int r, int g, int b);
void DrawCharacter(FbDesc *fb, int (*f)(int,int), int fw, int fh, char c, int x, int y, int r, int g, int b);
void DrawString(FbDesc *fb, int (*f)(int,int), int fw, int fh, char *s, int x, int y, int r, int g, int b);
void Flush(FbDesc *fb);

/* ── font ── */
int getArialCharacter(int index, int y);

/* ── UI elements ── */
int DrawCircleButton(FbDesc *fb, int x, int y, int radius, int r, int g, int b, int taskId);
int DrawButton(FbDesc *fb, int x, int y, int w, int h, int r, int g, int b, char *text, int r1, int g1, int b1, int taskId);
int DrawTextInput(FbDesc *fb, int x, int y, int w, int h, char *text, int *cursorPos, int r, int g, int b, int taskId);
int DrawCheckbox(FbDesc *fb, int x, int y, int *checked, char *label, int taskId);
int DrawRadioButton(FbDesc *fb, int x, int y, int *selected, int value, char *label, int taskId);
int DrawSlider(FbDesc *fb, int x, int y, int width, int *value, int minVal, int maxVal, int taskId);
int DrawWindow(FbDesc *fb, int *x, int *y, int *width, int *height, int r, int g, int b, int *mouse_held, int taskId);
int DrawMenuBar(FbDesc *fb, int x, int y, int width, int height, int taskId);
int DrawMenuItem(FbDesc *fb, int x, int y, int width, int height, char *label, int *isOpen, int taskId);
int DrawProgressBar(FbDesc *fb, int x, int y, int width, int height, int value, int maxVal, int r, int g, int b);
int DrawIcon(FbDesc *fb, int x, int y, int size, int iconType, int r, int g, int b);
int DrawFileIcon(FbDesc *fb, int x, int y, int isDirectory);
int DrawFileBrowser(FbDesc *fb, int x, int y, int width, int height, int *selectedIndex, int *scrollOffset, int numEntries, int taskId);
int DrawScrollBar(FbDesc *fb, int x, int y, int width, int height, int *value, int maxVal, int taskId);

/* ── scheduler ── */
void ProcessTasks(FbDesc *fb);
void CloseTask(int taskId);
int  NullTask(int taskId);

/* ── input ── */
unsigned char ProcessScancode(int scancode);

#endif
