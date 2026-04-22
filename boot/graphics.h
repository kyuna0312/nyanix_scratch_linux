#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

typedef struct VBEInfoBlockStruct {
    unsigned short mode_attribute;
    unsigned char win_a_attribute;
    unsigned char win_b_attribute;
    unsigned short win_granuality;
    unsigned short win_size;
    unsigned short win_a_segment;
    unsigned short win_b_segment;
    unsigned int win_func_ptr;
    unsigned short bytes_per_scan_line;
    unsigned short x_resolution;
    unsigned short y_resolution;
    unsigned char char_x_size;
    unsigned char char_y_size;
    unsigned char number_of_planes;
    unsigned char bits_per_pixel;
    unsigned char number_of_banks;
    unsigned char memory_model;
    unsigned char bank_size;
    unsigned char number_of_image_pages;
    unsigned char b_reserved;
    unsigned char red_mask_size;
    unsigned char red_field_position;
    unsigned char green_mask_size;
    unsigned char green_field_position;
    unsigned char blue_mask_size;
    unsigned char blue_field_position;
    unsigned char reserved_mask_size;
    unsigned char reserved_field_position;
    unsigned char direct_color_info;
    unsigned int screen_ptr;
} VBEInfoBlock;

struct Task {
    int priority;
    int taskId;
    char ca1[100];
    int i1;
    int (*function)(int taskId);
};

struct FileEntry {
    char name[30];
    int isDirectory;
    int size;
    int modified;
};

#define task_params_length 10
#define TRUE 1
#define FALSE 0

#define VBEInfoAddress 0x8000
#define ScreenBufferAddress 0xffff0

extern int screen_dirty;
extern const int font_arial_width;
extern const int font_arial_height;
extern volatile int pit_ticks;
extern int mx, my;
extern int left_clicked, right_clicked;
extern int mouse_possessed_task_id;
extern int Scancode;
extern int backspace_pressed;
extern unsigned int base, base0, base12;
extern unsigned int isr0, isr1, isr12;
extern void InitialiseMouse();
extern void InitPIT();
extern void InitialiseIDT();
extern struct Task tasks[256];
extern int iparams[100];
extern int TasksLength;

int getArialCharacter(int index, int y);
int rgb(int r, int g, int b);
void Draw(int x, int y, int r, int g, int b);
void ClearScreen(int r, int g, int b);
void DrawRect(int x, int y, int width, int height, int r, int g, int b);
void DrawCharacter(int (*f)(int, int), int font_width, int font_height, char character, int x, int y, int r, int g, int b);
void DrawString(int (*f)(int, int), int font_width, int font_height, char* string, int x, int y, int r, int g, int b);
void DrawMouse(int x, int y, int r, int g, int b);
void DrawCircle(int x, int y, int radius, int r, int g, int b);
void Flush();
void ProcessTasks();
unsigned char ProcessScancode(int scancode);
int ClearScreenTask(int taskId);
int TaskbarTask(int taskId);
int HandleKeyboardTask(int taskId);
int DrawMouseTask(int taskId);
int DrawCircleButton(int x, int y, int radius, int r, int g, int b, int taskId);
int DrawButton(int x, int y, int width, int height, int r, int g, int b, char* text, int r1, int g1, int b1, int taskId);
int DrawTextInput(int x, int y, int width, int height, char* text, int* cursorPos, int r, int g, int b, int taskId);
int DrawCheckbox(int x, int y, int* checked, char* label, int taskId);
int DrawRadioButton(int x, int y, int* selected, int value, char* label, int taskId);
int DrawSlider(int x, int y, int width, int* value, int minVal, int maxVal, int taskId);
int DrawMenuBar(int x, int y, int width, int height, int taskId);
int DrawMenuItem(int x, int y, int width, int height, char* label, int* isOpen, int taskId);
int DrawProgressBar(int x, int y, int width, int height, int value, int maxVal, int r, int g, int b);
int DrawIcon(int x, int y, int size, int iconType, int r, int g, int b);
int DrawFileIcon(int x, int y, int isDirectory);
int DrawFileBrowser(int x, int y, int width, int height, int* selectedIndex, int* scrollOffset, int numEntries, int taskId);
int DrawScrollBar(int x, int y, int width, int height, int* value, int maxVal, int taskId);
int DrawWindow(int* x, int* y, int* width, int* height, int r, int g, int b, int* mouse_held, int taskId);

#endif