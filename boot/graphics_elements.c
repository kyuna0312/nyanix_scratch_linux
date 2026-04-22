#include "graphics.h"

int DrawCircleButton(int x, int y, int radius, int r, int g, int b, int taskId) {
    if (mouse_possessed_task_id == taskId && (((mx - x)*(mx - x) + (my - y)*(my - y)) <= radius*radius)) {
        DrawCircle(x, y, radius, r, g, b);

        if (left_clicked == TRUE) {
            left_clicked = FALSE;
            return 1;
        }
    }
    else 
        DrawCircle(x, y, radius, r/4, g/4, b/4);

    return 0;
}

int DrawButton(int x, int y, int width, int height, int r, int g, int b, char* text, int r1, int g1, int b1, int taskId) {
    if (mouse_possessed_task_id == taskId && mx > x && mx < x + width && my > y && my < y + height) {
        DrawRect(x, y, width, height, r, g, b);

        if (left_clicked == TRUE) {
            left_clicked = FALSE;
            return 1;
        }
    }
    else
        DrawRect(x, y, width, height, r/4, g/4, b/4);

    DrawString(getArialCharacter, font_arial_width, font_arial_height, text, x + width / 10, y + height / 10, r1, g1, b1);
}

int DrawTextInput(int x, int y, int width, int height, char* text, int* cursorPos, int r, int g, int b, int taskId) {
    int isFocused = mouse_possessed_task_id == taskId;
    int rectR = isFocused ? r : r/3;
    int rectG = isFocused ? g : g/3;
    int rectB = isFocused ? b : b/3;
    
    DrawRect(x, y, width, height, rectR, rectG, rectB);
    DrawRect(x, y, width, 2, r+4, g+4, b);
    DrawRect(x, y + height - 2, width, 2, r+4, g+4, b);
    DrawRect(x, y, 2, height, r+4, g+4, b);
    DrawRect(x + width - 2, y, 2, height, r+4, g+4, b);
    
    if (*text != '\0') {
        DrawString(getArialCharacter, font_arial_width, font_arial_height, text, x + 5, y + height/2 - font_arial_height/2, 16, 32, 16);
    }
    
    if ((pit_ticks / 30) % 2 == 0 && isFocused) {
        int cursorX = x + 5;
        for (int i = 0; i < *cursorPos && text[i] != '\0'; i++) {
            cursorX += font_arial_width - 3;
        }
        DrawRect(cursorX, y + height/2 - font_arial_height/2, 2, font_arial_height, 16, 32, 16);
    }
    
    if (left_clicked == TRUE && mx > x && mx < x + width && my > y && my < y + height) {
        left_clicked = FALSE;
        return 1;
    }
    
    return 0;
}

int DrawCheckbox(int x, int y, int* checked, char* label, int taskId) {
    int boxSize = 16;
    int isHovered = mouse_possessed_task_id == taskId && mx > x && mx < x + boxSize + 30 && my > y && my < y + boxSize;
    
    if (*checked) {
        DrawRect(x, y, boxSize, boxSize, 0, 16, 0);
        DrawRect(x + 3, y + 3, boxSize - 6, boxSize - 6, 16, 32, 16);
    } else if (isHovered) {
        DrawRect(x, y, boxSize, boxSize, 0, 10, 0);
    } else {
        DrawRect(x, y, boxSize, boxSize, 0, 6, 0);
    }
    
    DrawRect(x, y, boxSize, 1, 8, 8, 8);
    DrawRect(x, y + boxSize - 1, boxSize, 1, 8, 8, 8);
    DrawRect(x, y, 1, boxSize, 8, 8, 8);
    DrawRect(x + boxSize - 1, y, 1, boxSize, 8, 8, 8);
    
    if (label[0] != '\0') {
        DrawString(getArialCharacter, font_arial_width, font_arial_height, label, x + boxSize + 5, y + 2, 16, 32, 16);
    }
    
    if (left_clicked == TRUE && isHovered) {
        left_clicked = FALSE;
        *checked = *checked == 0 ? 1 : 0;
        return 1;
    }
    
    return 0;
}

int DrawRadioButton(int x, int y, int* selected, int value, char* label, int taskId) {
    int radius = 8;
    int cx = x + radius;
    int cy = y + radius;
    int isHovered = mouse_possessed_task_id == taskId && (((mx - cx)*(mx - cx) + (my - cy)*(my - cy)) <= radius*radius);
    
    DrawCircle(cx, cy, radius, 8, 8, 8);
    DrawCircle(cx, cy, radius - 2, 16, 16, 16);
    
    if (*selected == value) {
        DrawCircle(cx, cy, radius - 5, 0, 16, 0);
    }
    
    if (label[0] != '\0') {
        DrawString(getArialCharacter, font_arial_width, font_arial_height, label, x + radius + 8, y + 2, 16, 32, 16);
    }
    
    if (left_clicked == TRUE && isHovered) {
        left_clicked = FALSE;
        *selected = value;
        return 1;
    }
    
    return 0;
}

int DrawSlider(int x, int y, int width, int* value, int minVal, int maxVal, int taskId) {
    int trackHeight = 6;
    int handleWidth = 12;
    int handleHeight = 16;
    int isHovered = mouse_possessed_task_id == taskId && mx > x && mx < x + width && my > y && my < y + handleHeight;
    
    DrawRect(x, y + handleHeight/2 - trackHeight/2, width, trackHeight, 4, 4, 4);
    DrawRect(x, y + handleHeight/2 - trackHeight/2, width, 1, 8, 8, 8);
    DrawRect(x, y + handleHeight/2 + trackHeight/2 - 1, width, 1, 8, 8, 8);
    
    int handleX = x + (*value - minVal) * (width - handleWidth) / (maxVal - minVal);
    
    if (isHovered || mouse_possessed_task_id == taskId) {
        DrawRect(handleX, y, handleWidth, handleHeight, 0, 20, 8);
    } else {
        DrawRect(handleX, y, handleWidth, handleHeight, 0, 10, 4);
    }
    
    DrawRect(handleX, y, handleWidth, 1, 8, 8, 8);
    DrawRect(handleX, y + handleHeight - 1, handleWidth, 1, 8, 8, 8);
    DrawRect(handleX, y, 1, handleHeight, 8, 8, 8);
    DrawRect(handleX + handleWidth - 1, y, 1, handleHeight, 8, 8, 8);
    
    if (left_clicked == TRUE && isHovered) {
        left_clicked = FALSE;
        *value = minVal + (mx - x) * (maxVal - minVal) / width;
        if (*value < minVal) *value = minVal;
        if (*value > maxVal) *value = maxVal;
        return 1;
    }
    
    return 0;
}

int DrawWindow(int* x, int* y, int* width, int* height, int r, int g, int b, int* mouse_held, int taskId) {
    if (left_clicked == FALSE) {
        *mouse_held = FALSE;
    }

    if (mouse_possessed_task_id == taskId && (*mouse_held == TRUE || 
        (left_clicked == TRUE && mx > *x &&
         mx < *x + *width - 30 &&
         my > *y &&
         my < *y + 20))) {
         left_clicked = FALSE;
         *mouse_held = TRUE;
         *x = mx - (*width / 2);
         *y = my - 10;
    }

    DrawRect(*x, *y, *width, 20, 16, 32, 16);
    DrawRect(*x, *y + 20, *width, *height, r, g, b);
    
    int titleBarY = *y + 10 - font_arial_height/2;
    DrawString(getArialCharacter, font_arial_width, font_arial_height, "×", *x + *width - 22, titleBarY, 16, 0, 0);
    DrawString(getArialCharacter, font_arial_width, font_arial_height, "□", *x + *width - 42, titleBarY, 16, 0, 0);
    DrawString(getArialCharacter, font_arial_width, font_arial_height, "—", *x + 5, titleBarY, 16, 0, 0);

    return DrawCircleButton(*x + *width - 10, *y + 10, 8, 16, 0, 0, taskId);
}

int DrawMenuBar(int x, int y, int width, int height, int taskId) {
    DrawRect(x, y, width, height, 16, 32, 16);
    DrawRect(x, y + height - 1, width, 1, 8, 8, 8);
    return 0;
}

int DrawMenuItem(int x, int y, int width, int height, char* label, int* isOpen, int taskId) {
    int isHovered = mouse_possessed_task_id == taskId && mx > x && mx < x + width && my > y && my < y + height;
    
    if (isHovered) {
        DrawRect(x, y, width, height, 0, 16, 4);
    }
    
    DrawString(getArialCharacter, font_arial_width, font_arial_height, label, x + 5, y + height/2 - font_arial_height/2, 16, 32, 16);
    
    if (left_clicked == TRUE && isHovered) {
        left_clicked = FALSE;
        *isOpen = *isOpen == 0 ? 1 : 0;
        return 1;
    }
    
    return 0;
}

int DrawProgressBar(int x, int y, int width, int height, int value, int maxVal, int r, int g, int b) {
    int fillWidth = (value * width) / maxVal;
    
    DrawRect(x, y, width, height, 4, 4, 4);
    DrawRect(x, y, width, 1, 8, 8, 8);
    DrawRect(x, y + height - 1, width, 1, 8, 8, 8);
    DrawRect(x, y, 1, height, 8, 8, 8);
    DrawRect(x + width - 1, y, 1, height, 8, 8, 8);
    
    if (fillWidth > 0) {
        DrawRect(x + 1, y + 1, fillWidth - 2, height - 2, r, g, b);
    }
    
    return 0;
}

int DrawIcon(int x, int y, int size, int iconType, int r, int g, int b) {
    if (iconType == 0) {
        DrawRect(x + size/3, y, size/3, size, r, g, b);
    } else if (iconType == 1) {
        DrawRect(x, y, size, size/3, r, g, b);
    } else if (iconType == 2) {
        DrawCircle(x + size/2, y + size/2, size/3, r, g, b);
    } else if (iconType == 3) {
        DrawCircle(x + size/2, y + size/2, size/4, r, g, b);
        DrawCircle(x + size/2, y + size/2, size/8, 16, 32, 16);
    } else if (iconType == 4) {
        for (int i = 0; i < 3; i++) {
            DrawRect(x, y + i * size/3, size, size/3 - 1, r, g, b);
        }
    }
    return 0;
}

struct FileEntry {
    char name[30];
    int isDirectory;
    int size;
    int modified;
};

int DrawFileIcon(int x, int y, int isDirectory) {
    if (isDirectory) {
        DrawRect(x + 2, y, 12, 8, 0, 16, 32);
        DrawRect(x, y + 6, 16, 10, 16, 32, 16);
    } else {
        DrawRect(x + 4, y, 8, 14, 4, 4, 16);
        DrawRect(x, y + 12, 16, 8, 0, 10, 16);
    }
    return 0;
}

int DrawFileBrowser(int x, int y, int width, int height, int* selectedIndex, int* scrollOffset, int numEntries, int taskId) {
    int rowHeight = 20;
    int visibleRows = height / rowHeight - 1;
    
    DrawRect(x, y, width, height, 4, 4, 4);
    DrawRect(x, y, width, 1, 8, 8, 8);
    DrawRect(x, y + height - 1, width, 1, 8, 8, 8);
    DrawRect(x, y, 1, height, 8, 8, 8);
    DrawRect(x + width - 1, y, 1, height, 8, 8, 8);
    
    int upArrowY = y + height - 16;
    if (mx > x && mx < x + width - 20 && my > upArrowY && my < upArrowY + 12) {
        DrawRect(x + 10, upArrowY + 4, 8, 4, 0, 10, 0);
    } else {
        DrawRect(x + 10, upArrowY + 4, 8, 4, 0, 6, 0);
    }
    
    int downArrowY = y + height - 16;
    if (mx > x + 20 && mx < x + width && my > downArrowY && my < downArrowY + 12) {
        DrawRect(x + width - 18, downArrowY + 4, 8, 4, 0, 10, 0);
    } else {
        DrawRect(x + width - 18, downArrowY + 4, 8, 4, 0, 6, 0);
    }
    
    int isHovered = mouse_possessed_task_id == taskId && mx > x && mx < x + width - 20 && my > y && my < y + height - 16;
    
    for (int i = 0; i < visibleRows && (*scrollOffset + i) < numEntries; i++) {
        int rowY = y + 2 + i * rowHeight;
        int idx = *scrollOffset + i;
        int isSelected = (idx == *selectedIndex);
        int rowHovered = isHovered && mx > x && mx < x + width - 20 && my > rowY && my < rowY + rowHeight;
        
        if (isSelected || rowHovered) {
            DrawRect(x + 1, rowY, width - 22, rowHeight - 1, isSelected ? 0 : 0, isSelected ? 16 : 8, 0);
        }
    }
    
    if (left_clicked == TRUE && isHovered) {
        int clickRow = (my - y - 2) / rowHeight;
        int idx = *scrollOffset + clickRow;
        if (idx >= 0 && idx < numEntries) {
            *selectedIndex = idx;
        }
        left_clicked = FALSE;
    }
    
    return 0;
}

int DrawScrollBar(int x, int y, int width, int height, int* value, int maxVal, int taskId) {
    int thumbHeight = (height > 50) ? height / (maxVal + 1) : 20;
    if (thumbHeight < 15) thumbHeight = 15;
    
    DrawRect(x, y, width, height, 4, 4, 4);
    
    int thumbY = y + (*value * (height - thumbHeight)) / maxVal;
    int isHovered = mouse_possessed_task_id == taskId && mx > x && mx < x + width && my > thumbY && my < thumbY + thumbHeight;
    
    DrawRect(x + 1, thumbY, width - 2, thumbHeight, isHovered ? 0 : 0, isHovered ? 16 : 8, 0);
    DrawRect(x + 2, thumbY + thumbHeight/2 - 2, width - 4, 4, 16, 16, 16);
    
    if (left_clicked == TRUE && isHovered) {
        left_clicked = FALSE;
        return 1;
    }
    
    return 0;
}