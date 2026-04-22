#include "graphics.h"

<<<<<<< Updated upstream
int DesktopTask(int taskId);
int ClockTask(int taskId);

int TasksLength = 0;
=======
int DemoTask(int taskId);

int TasksLength = 0;
>>>>>>> Stashed changes

struct Task tasks[256];
int iparams[100] = {10};

void ProcessTasks() {
    int priority;
    int i = 0;

    priority = 5;
    while (priority >= 0) {
        i = mouse_possessed_task_id;
        if (left_clicked == TRUE &&
                mx > iparams[i * task_params_length + 0] &&
                mx < iparams[i * task_params_length + 0] + iparams[i * task_params_length + 2] &&
                my > iparams[i * task_params_length + 1] &&
                my < iparams[i * task_params_length + 1] + iparams[i * task_params_length + 3])
                break;

        for (i = 0; i < TasksLength; i++) {
            if (left_clicked == TRUE &&
                mx > iparams[i * task_params_length + 0] &&
                mx < iparams[i * task_params_length + 0] + iparams[i * task_params_length + 2] &&
                my > iparams[i * task_params_length + 1] &&
                my < iparams[i * task_params_length + 1] + iparams[i * task_params_length + 3]) {
                    tasks[mouse_possessed_task_id].priority = 0;
                    mouse_possessed_task_id = i;
                    tasks[i].priority = 2;
                    left_clicked = FALSE;
                }
        }

        priority--;
    }

    priority = 0;
    while (priority <= 5) {
        for (int i = 0; i < TasksLength; i++) {
            if (tasks[i].priority == priority) {
                tasks[i].function(tasks[i].taskId);
            }
        }

        priority++;
    }
}

int NullTask(int taskId) {
    return 0;
}

void CloseTask(int taskId) {
    tasks[taskId].function = &NullTask;
    iparams[taskId * task_params_length + 0] = 0;
    iparams[taskId * task_params_length + 1] = 0;
    iparams[taskId * task_params_length + 2] = 0;
    iparams[taskId * task_params_length + 3] = 0;
}

int ClearScreenTask(int taskId) {
    ClearScreen(181.0f / 255.0f * 16.0f, 232.0f / 255.0f * 32.0f, 255.0f / 255.0f * 16.0f);
    return 0;
}

int DesktopTask(int taskId) {
    VBEInfoBlock* VBE = (VBEInfoBlock*) VBEInfoAddress;
    DrawRect(0, 40, VBE->x_resolution, VBE->y_resolution - 40, 8, 12, 16);
    DrawRect(0, VBE->y_resolution - 2, VBE->x_resolution, 2, 4, 4, 4);
    return 0;
}

int ClockTask(int taskId) {
    VBEInfoBlock* VBE = (VBEInfoBlock*) VBEInfoAddress;
    int hours = pit_ticks / 3600;
    int mins = (pit_ticks % 3600) / 60;
    int secs = pit_ticks % 60;
    
    char timeStr[] = "00:00:00";
    timeStr[0] = '0' + (hours / 10);
    timeStr[1] = '0' + (hours % 10);
    timeStr[3] = '0' + (mins / 10);
    timeStr[4] = '0' + (mins % 10);
    timeStr[6] = '0' + (secs / 10);
    timeStr[7] = '0' + (secs % 10);
    
    DrawString(getArialCharacter, font_arial_width, font_arial_height, timeStr, VBE->x_resolution - 70, 12, 16, 32, 16);
    return 0;
}

int DrawMouseTask(int taskId) {
    DrawMouse(mx, my, 16, 100.0 / 255.0 * 32, 100.0 / 255.0 * 16);

    return 0;
}

int HandleKeyboardTask(int taskId) {
    char* characterBuffer = tasks[taskId].ca1;
    int* characterBufferLength = &tasks[taskId].i1;

    char character = ProcessScancode(Scancode);

    if (backspace_pressed == TRUE) {
        characterBuffer[*characterBufferLength - 1] = '\0';
        (*characterBufferLength)--;
        backspace_pressed = FALSE;
        Scancode = -1;
    }
    else if (character != '\0') {
        characterBuffer[*characterBufferLength] = character;
        characterBuffer[*characterBufferLength + 1] = '\0';
        (*characterBufferLength)++;
        Scancode = -1;
    }

    DrawString(getArialCharacter, font_arial_width, font_arial_height, characterBuffer, 100, 100, 16, 32, 16);

    return 0;
}

int ShellTask(int taskId) {
    int* r = &iparams[taskId * task_params_length + 4];
    int* g = &iparams[taskId * task_params_length + 5];
    int* b = &iparams[taskId * task_params_length + 6];

    int closeClicked = DrawWindow(
        &iparams[taskId * task_params_length + 0],
        &iparams[taskId * task_params_length + 1],
        &iparams[taskId * task_params_length + 2],
        &iparams[taskId * task_params_length + 3],
        *r,
        *g,
        *b,
        &iparams[taskId * task_params_length + 9],
        taskId);

    int x = iparams[taskId * task_params_length + 0];
    int y = iparams[taskId * task_params_length + 1];
    int width = iparams[taskId * task_params_length + 2];
    int height = iparams[taskId * task_params_length + 3];

    if(closeClicked == TRUE)
        CloseTask(taskId);

    char text[] = "Dark\0";
    char text1[] = "Light\0";

    if (DrawButton(x + 20, y + 20, 50, 20, 0, 32, 0,
            text, 16, 32, 16, taskId
    ) == TRUE) {
        *r = 0;
        *g = 0;
        *b = 0;
    }

    if (DrawButton(x + 100, y + 20, 50, 20, 0, 32, 0,
            text1, 16, 32, 16, taskId
    ) == TRUE) {
        *r = 16;
        *g = 31;
        *b = 16;
    }

    return 0;
}

int BallTask(int taskId) {
    int closeClicked = DrawWindow(
        &iparams[taskId * task_params_length + 0],
        &iparams[taskId * task_params_length + 1],
        &iparams[taskId * task_params_length + 2],
        &iparams[taskId * task_params_length + 3],
        0,
        0,
        0,
        &iparams[taskId * task_params_length + 9],
        taskId);

    
    if(closeClicked == TRUE)
        CloseTask(taskId);

    int x = iparams[taskId * task_params_length + 0];
    int y = iparams[taskId * task_params_length + 1];
    int width = iparams[taskId * task_params_length + 2];
    int height = iparams[taskId * task_params_length + 3];

    iparams[taskId * task_params_length + 5] += iparams[taskId * task_params_length + 7];
    iparams[taskId * task_params_length + 6] += iparams[taskId * task_params_length + 8];

    if (iparams[taskId * task_params_length + 5] + 10 > iparams[taskId * task_params_length + 2] ||
        iparams[taskId * task_params_length + 5] - 10 < 0)
        iparams[taskId * task_params_length + 7] = -iparams[taskId * task_params_length + 7];

    if (iparams[taskId * task_params_length + 6] + 10 > iparams[taskId * task_params_length + 3] ||
        iparams[taskId * task_params_length + 6] - 10 < 20)
        iparams[taskId * task_params_length + 8] = -iparams[taskId * task_params_length + 8];

    DrawCircle(x + iparams[taskId * task_params_length + 5], y + iparams[taskId * task_params_length + 6], 10, 16, 32, 16);
}

int TaskbarTask(int taskId) {
    VBEInfoBlock* VBE = (VBEInfoBlock*) VBEInfoAddress;
    DrawRect(0, 0, VBE->x_resolution, 40, 16, 32, 16);

    int i = iparams[taskId * task_params_length + 4];

    char text[] = "Shell";
    if (DrawButton(0, 0, 50, 40, 0, 10, 16, text, 16, 32, 16, taskId) == TRUE) {
        tasks[TasksLength].priority = 0;
        tasks[TasksLength].taskId = TasksLength;
        tasks[TasksLength].function = &ShellTask;
        iparams[TasksLength * task_params_length + 0] = i * 40;
        iparams[TasksLength * task_params_length + 1] = i * 40;
        iparams[TasksLength * task_params_length + 2] = 300;
        iparams[TasksLength * task_params_length + 3] = 300;
        iparams[TasksLength * task_params_length + 4] = 0;
        iparams[TasksLength * task_params_length + 5] = 0;
        iparams[TasksLength * task_params_length + 6] = 0;
        TasksLength++;
        iparams[taskId * task_params_length + 4]++;
    }

    char text2[] = "Ball";
    if (DrawButton(50, 0, 50, 40, 16, 10, 0, text, 16, 32, 16, taskId) == TRUE) {
        tasks[TasksLength].priority = 0;
        tasks[TasksLength].taskId = TasksLength;
        tasks[TasksLength].function = &BallTask;
        iparams[TasksLength * task_params_length + 0] = i * 40;
        iparams[TasksLength * task_params_length + 1] = i * 40;
        iparams[TasksLength * task_params_length + 2] = 300;
        iparams[TasksLength * task_params_length + 3] = 300;
        iparams[TasksLength * task_params_length + 4] = 0;
        iparams[TasksLength * task_params_length + 5] = 20;
        iparams[TasksLength * task_params_length + 6] = 30;
        iparams[TasksLength * task_params_length + 7] = 5;
        iparams[TasksLength * task_params_length + 8] = 5;
        TasksLength++;
    }
    
    char text3[] = "Demo";
    if (DrawButton(100, 0, 50, 40, 16, 10, 0, text3, 16, 32, 16, taskId) == TRUE) {
        tasks[TasksLength].priority = 0;
        tasks[TasksLength].taskId = TasksLength;
        tasks[TasksLength].function = &DemoTask;
        iparams[TasksLength * task_params_length + 0] = i * 40;
        iparams[TasksLength * task_params_length + 1] = i * 40;
        iparams[TasksLength * task_params_length + 2] = 400;
        iparams[TasksLength * task_params_length + 3] = 300;
        iparams[TasksLength * task_params_length + 4] = 0;
        iparams[TasksLength * task_params_length + 5] = 50;
        TasksLength++;
    }
}

int DemoTask(int taskId) {
    int x = iparams[taskId * task_params_length + 0];
    int y = iparams[taskId * task_params_length + 1];
    int width = iparams[taskId * task_params_length + 2];
    int height = iparams[taskId * task_params_length + 3];
    
    int closeClicked = DrawWindow(&x, &y, &width, &height, 16, 16, 16, &iparams[taskId * task_params_length + 9], taskId);
    if (closeClicked == TRUE)
        CloseTask(taskId);
    
    DrawRect(x + 10, y + 30, 150, 25, 4, 4, 4);
    static char inputText[] = "Hello World";
    static int cursorPos = 10;
    if (DrawTextInput(x + 10, y + 30, 150, 25, inputText, &cursorPos, 0, 10, 0, taskId) == TRUE) {
    }
    
    static int checked1 = 0;
    static int checked2 = 0;
    if (DrawCheckbox(x + 10, y + 60, &checked1, "Checkbox 1", taskId) == TRUE) {
    }
    if (DrawCheckbox(x + 10, y + 80, &checked2, "Checkbox 2", taskId) == TRUE) {
    }
    
    static int radioVal = 1;
    if (DrawRadioButton(x + 10, y + 105, &radioVal, 1, "Option 1", taskId) == TRUE) {
    }
    if (DrawRadioButton(x + 10, y + 125, &radioVal, 2, "Option 2", taskId) == TRUE) {
    }
    
    static int sliderVal = 50;
    DrawSlider(x + 10, y + 150, 150, &sliderVal, 0, 100, taskId);
    char sliderText[] = "Value: 50";
    sliderText[8] = '0' + (sliderVal / 10) % 10;
    sliderText[9] = '0' + sliderVal % 10;
    DrawString(getArialCharacter, font_arial_width, font_arial_height, sliderText, x + 170, y + 145, 16, 32, 16);
    
    static int selectedIdx = 0;
    static int scrollIdx = 0;
    struct FileEntry files[5];
    files[0].name[0] = 'd'; files[0].name[1] = 'o'; files[0].name[2] = 'c'; files[0].name[3] = 's'; files[0].name[4] = '\0'; files[0].isDirectory = 1;
    files[1].name[0] = 'd'; files[1].name[1] = 'o'; files[1].name[2] = 'c'; files[1].name[3] = 's'; files[1].name[4] = '2'; files[1].name[5] = '\0'; files[1].isDirectory = 1;
    files[2].name[0] = 'r'; files[2].name[1] = 'e'; files[2].name[2] = 'a'; files[2].name[3] = 'd'; files[2].name[4] = 'm'; files[2].name[5] = 'e'; files[2].name[6] = '.'; files[2].name[7] = 't'; files[2].name[8] = 'x'; files[2].name[9] = 't'; files[2].name[10] = '\0'; files[2].isDirectory = 0;
    files[3].name[0] = 's'; files[3].name[1] = 'o'; files[3].name[2] = 'u'; files[3].name[3] = 'r'; files[3].name[4] = 'c'; files[3].name[5] = 'e'; files[3].name[6] = '.'; files[3].name[7] = 'c'; files[3].name[8] = '\0'; files[3].isDirectory = 0;
    files[4].name[0] = 'd'; files[4].name[1] = 'a'; files[4].name[2] = 't'; files[4].name[3] = 'a'; files[4].name[4] = '.'; files[4].name[5] = 'b'; files[4].name[6] = 'i'; files[4].name[7] = 'n'; files[4].name[8] = '\0'; files[4].isDirectory = 0;
    DrawFileBrowser(x + 10, y + 170, 180, 100, &selectedIdx, &scrollIdx, 5, taskId);
}