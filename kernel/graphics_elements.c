#include "graphics.h"

int DrawCircleButton(FbDesc *fb, int x, int y, int radius, int r, int g, int b, int taskId) {
    int dx = mx - x, dy = my - y;
    if (mouse_possessed_task_id == taskId && dx*dx + dy*dy <= radius*radius) {
        DrawCircle(fb, x, y, radius, r, g, b);
        if (left_clicked == TRUE) {
            left_clicked = FALSE;
            return 1;
        }
    } else {
        DrawCircle(fb, x, y, radius, r/4, g/4, b/4);
    }
    return 0;
}

int DrawButton(FbDesc *fb, int x, int y, int w, int h, int r, int g, int b, char *text, int r1, int g1, int b1, int taskId) {
    if (mouse_possessed_task_id == taskId && mx > x && mx < x + w && my > y && my < y + h) {
        DrawRect(fb, x, y, w, h, r, g, b);
        if (left_clicked == TRUE) {
            left_clicked = FALSE;
            return 1;
        }
    } else {
        DrawRect(fb, x, y, w, h, r/4, g/4, b/4);
    }
    DrawString(fb, getArialCharacter, font_arial_width, font_arial_height, text, x + w/10, y + h/10, r1, g1, b1);
    return 0;
}

int DrawTextInput(FbDesc *fb, int x, int y, int w, int h, char *text, int *cursorPos, int r, int g, int b, int taskId) {
    int focused = mouse_possessed_task_id == taskId;
    DrawRect(fb, x, y, w, h, focused ? r : r/3, focused ? g : g/3, focused ? b : b/3);
    DrawRect(fb, x, y, w, 2, r+4, g+4, b);
    DrawRect(fb, x, y + h - 2, w, 2, r+4, g+4, b);
    DrawRect(fb, x, y, 2, h, r+4, g+4, b);
    DrawRect(fb, x + w - 2, y, 2, h, r+4, g+4, b);
    if (*text != '\0')
        DrawString(fb, getArialCharacter, font_arial_width, font_arial_height, text, x + 5, y + h/2 - font_arial_height/2, 16, 32, 16);
    if ((pit_ticks / 30) % 2 == 0 && focused) {
        int cx = x + 5;
        for (int i = 0; i < *cursorPos && text[i] != '\0'; i++)
            cx += font_arial_width - 3;
        DrawRect(fb, cx, y + h/2 - font_arial_height/2, 2, font_arial_height, 16, 32, 16);
    }
    if (left_clicked == TRUE && mx > x && mx < x + w && my > y && my < y + h) {
        left_clicked = FALSE;
        return 1;
    }
    return 0;
}

int DrawCheckbox(FbDesc *fb, int x, int y, int *checked, char *label, int taskId) {
    int sz = 16;
    int hovered = mouse_possessed_task_id == taskId && mx > x && mx < x + sz + 30 && my > y && my < y + sz;
    if (*checked)
        DrawRect(fb, x, y, sz, sz, 0, 16, 0);
    else if (hovered)
        DrawRect(fb, x, y, sz, sz, 0, 10, 0);
    else
        DrawRect(fb, x, y, sz, sz, 0, 6, 0);
    DrawRect(fb, x, y, sz, 1, 8, 8, 8);
    DrawRect(fb, x, y + sz - 1, sz, 1, 8, 8, 8);
    DrawRect(fb, x, y, 1, sz, 8, 8, 8);
    DrawRect(fb, x + sz - 1, y, 1, sz, 8, 8, 8);
    if (label[0] != '\0')
        DrawString(fb, getArialCharacter, font_arial_width, font_arial_height, label, x + sz + 5, y + 2, 16, 32, 16);
    if (left_clicked == TRUE && hovered) {
        left_clicked = FALSE;
        *checked = *checked ? 0 : 1;
        return 1;
    }
    return 0;
}

int DrawRadioButton(FbDesc *fb, int x, int y, int *selected, int value, char *label, int taskId) {
    int radius = 8, cx = x + radius, cy = y + radius;
    int dx = mx - cx, dy = my - cy;
    int hovered = mouse_possessed_task_id == taskId && dx*dx + dy*dy <= radius*radius;
    DrawCircle(fb, cx, cy, radius, 8, 8, 8);
    DrawCircle(fb, cx, cy, radius - 2, 16, 16, 16);
    if (*selected == value)
        DrawCircle(fb, cx, cy, radius - 5, 0, 16, 0);
    if (label[0] != '\0')
        DrawString(fb, getArialCharacter, font_arial_width, font_arial_height, label, x + radius + 8, y + 2, 16, 32, 16);
    if (left_clicked == TRUE && hovered) {
        left_clicked = FALSE;
        *selected = value;
        return 1;
    }
    return 0;
}

int DrawSlider(FbDesc *fb, int x, int y, int width, int *value, int minVal, int maxVal, int taskId) {
    int th = 6, hw = 12, hh = 16;
    int hovered = mouse_possessed_task_id == taskId && mx > x && mx < x + width && my > y && my < y + hh;
    DrawRect(fb, x, y + hh/2 - th/2, width, th, 4, 4, 4);
    DrawRect(fb, x, y + hh/2 - th/2, width, 1, 8, 8, 8);
    DrawRect(fb, x, y + hh/2 + th/2 - 1, width, 1, 8, 8, 8);
    int hx = x + (*value - minVal) * (width - hw) / (maxVal - minVal);
    DrawRect(fb, hx, y, hw, hh, hovered ? 0 : 0, hovered ? 20 : 10, hovered ? 8 : 4);
    DrawRect(fb, hx, y, hw, 1, 8, 8, 8);
    DrawRect(fb, hx, y + hh - 1, hw, 1, 8, 8, 8);
    DrawRect(fb, hx, y, 1, hh, 8, 8, 8);
    DrawRect(fb, hx + hw - 1, y, 1, hh, 8, 8, 8);
    if (left_clicked == TRUE && hovered) {
        left_clicked = FALSE;
        *value = minVal + (mx - x) * (maxVal - minVal) / width;
        if (*value < minVal) *value = minVal;
        if (*value > maxVal) *value = maxVal;
        return 1;
    }
    return 0;
}

int DrawWindow(FbDesc *fb, int *x, int *y, int *width, int *height, int r, int g, int b, int *mouse_held, int taskId) {
    if (!left_clicked)
        *mouse_held = FALSE;
    if (mouse_possessed_task_id == taskId &&
        (*mouse_held == TRUE ||
         (left_clicked == TRUE && mx > *x && mx < *x + *width - 30 && my > *y && my < *y + 20))) {
        left_clicked = FALSE;
        *mouse_held = TRUE;
        *x = mx - (*width / 2);
        *y = my - 10;
    }
    DrawRect(fb, *x, *y, *width, 20, 16, 32, 16);
    DrawRect(fb, *x, *y + 20, *width, *height, r, g, b);
    int tby = *y + 10 - font_arial_height/2;
    DrawString(fb, getArialCharacter, font_arial_width, font_arial_height, "x", *x + *width - 22, tby, 16, 0, 0);
    return DrawCircleButton(fb, *x + *width - 10, *y + 10, 8, 16, 0, 0, taskId);
}

int DrawMenuBar(FbDesc *fb, int x, int y, int width, int height, int taskId) {
    DrawRect(fb, x, y, width, height, 16, 32, 16);
    DrawRect(fb, x, y + height - 1, width, 1, 8, 8, 8);
    return 0;
}

int DrawMenuItem(FbDesc *fb, int x, int y, int width, int height, char *label, int *isOpen, int taskId) {
    int hovered = mouse_possessed_task_id == taskId && mx > x && mx < x + width && my > y && my < y + height;
    if (hovered)
        DrawRect(fb, x, y, width, height, 0, 16, 4);
    DrawString(fb, getArialCharacter, font_arial_width, font_arial_height, label, x + 5, y + height/2 - font_arial_height/2, 16, 32, 16);
    if (left_clicked == TRUE && hovered) {
        left_clicked = FALSE;
        *isOpen = *isOpen ? 0 : 1;
        return 1;
    }
    return 0;
}

int DrawProgressBar(FbDesc *fb, int x, int y, int width, int height, int value, int maxVal, int r, int g, int b) {
    int fw = value * width / maxVal;
    DrawRect(fb, x, y, width, height, 4, 4, 4);
    DrawRect(fb, x, y, width, 1, 8, 8, 8);
    DrawRect(fb, x, y + height - 1, width, 1, 8, 8, 8);
    DrawRect(fb, x, y, 1, height, 8, 8, 8);
    DrawRect(fb, x + width - 1, y, 1, height, 8, 8, 8);
    if (fw > 0)
        DrawRect(fb, x + 1, y + 1, fw - 2, height - 2, r, g, b);
    return 0;
}

int DrawIcon(FbDesc *fb, int x, int y, int size, int iconType, int r, int g, int b) {
    if (iconType == 0)
        DrawRect(fb, x + size/3, y, size/3, size, r, g, b);
    else if (iconType == 1)
        DrawRect(fb, x, y, size, size/3, r, g, b);
    else if (iconType == 2)
        DrawCircle(fb, x + size/2, y + size/2, size/3, r, g, b);
    else if (iconType == 3) {
        DrawCircle(fb, x + size/2, y + size/2, size/4, r, g, b);
        DrawCircle(fb, x + size/2, y + size/2, size/8, 16, 32, 16);
    } else if (iconType == 4)
        for (int i = 0; i < 3; i++)
            DrawRect(fb, x, y + i * size/3, size, size/3 - 1, r, g, b);
    return 0;
}

int DrawFileIcon(FbDesc *fb, int x, int y, int isDirectory) {
    if (isDirectory) {
        DrawRect(fb, x + 2, y, 12, 8, 0, 16, 32);
        DrawRect(fb, x, y + 6, 16, 10, 16, 32, 16);
    } else {
        DrawRect(fb, x + 4, y, 8, 14, 4, 4, 16);
        DrawRect(fb, x, y + 12, 16, 8, 0, 10, 16);
    }
    return 0;
}

int DrawFileBrowser(FbDesc *fb, int x, int y, int width, int height, int *selectedIndex, int *scrollOffset, int numEntries, int taskId) {
    int rh = 20, visible = height / rh - 1;
    DrawRect(fb, x, y, width, height, 4, 4, 4);
    DrawRect(fb, x, y, width, 1, 8, 8, 8);
    DrawRect(fb, x, y + height - 1, width, 1, 8, 8, 8);
    DrawRect(fb, x, y, 1, height, 8, 8, 8);
    DrawRect(fb, x + width - 1, y, 1, height, 8, 8, 8);
    int hovered = mouse_possessed_task_id == taskId && mx > x && mx < x + width - 20 && my > y && my < y + height - 16;
    for (int i = 0; i < visible && (*scrollOffset + i) < numEntries; i++) {
        int ry = y + 2 + i * rh;
        int idx = *scrollOffset + i;
        int sel = idx == *selectedIndex;
        int rh2 = hovered && mx > x && mx < x + width - 20 && my > ry && my < ry + rh;
        if (sel || rh2)
            DrawRect(fb, x + 1, ry, width - 22, rh - 1, 0, sel ? 16 : 8, 0);
    }
    if (left_clicked == TRUE && hovered) {
        int clickRow = (my - y - 2) / rh;
        int idx = *scrollOffset + clickRow;
        if (idx >= 0 && idx < numEntries)
            *selectedIndex = idx;
        left_clicked = FALSE;
    }
    return 0;
}

int DrawScrollBar(FbDesc *fb, int x, int y, int width, int height, int *value, int maxVal, int taskId) {
    int th = height > 50 ? height / (maxVal + 1) : 20;
    if (th < 15) th = 15;
    DrawRect(fb, x, y, width, height, 4, 4, 4);
    int ty = y + (*value * (height - th)) / maxVal;
    int hovered = mouse_possessed_task_id == taskId && mx > x && mx < x + width && my > ty && my < ty + th;
    DrawRect(fb, x + 1, ty, width - 2, th, 0, hovered ? 16 : 8, 0);
    DrawRect(fb, x + 2, ty + th/2 - 2, width - 4, 4, 16, 16, 16);
    if (left_clicked == TRUE && hovered) {
        left_clicked = FALSE;
        return 1;
    }
    return 0;
}
