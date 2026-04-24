#include "graphics.h"

int TasksLength = 0;

struct Task tasks[256];
int iparams[100] = {10};

void ProcessTasks(FbDesc *fb) {
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
