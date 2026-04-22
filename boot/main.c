<<<<<<< Updated upstream
#include "graphics.h"

int ClockTask(int taskId);

int start() {
    VBEInfoBlock* VBE = (VBEInfoBlock*) VBEInfoAddress;

	mx = VBE->x_resolution / 2;
	my = VBE->y_resolution / 2;
	// String literals cannot be more than 61 characters.
	char str1[] = "Welcome to SaphireOS!\n\nText rendered by custom library.";
	char *p = str1;

	char characterBuffer[1000] = "\0";
	char* characterBufferPointer = characterBuffer;
	int characterBufferLength = 0;

	base0 = (unsigned int) &isr0;
	base = (unsigned int) &isr1;
	base12 = (unsigned int) &isr12;

	InitialiseMouse();
	InitPIT();
	InitialiseIDT();

	tasks[TasksLength].priority = 0;
	tasks[TasksLength].function = &ClearScreenTask;
	TasksLength++;

	tasks[TasksLength].priority = 5;
	tasks[TasksLength].function = &DesktopTask;
	TasksLength++;

	tasks[TasksLength].priority = 5;
	tasks[TasksLength].function = &ClockTask;
	TasksLength++;

	tasks[TasksLength].priority = 0;
	tasks[TasksLength].function = &TaskbarTask;
	tasks[TasksLength].taskId = TasksLength;
	iparams[TasksLength * task_params_length + 0] = 0;
	iparams[TasksLength * task_params_length + 1] = 0;
	iparams[TasksLength * task_params_length + 2] = VBE->x_resolution;
	iparams[TasksLength * task_params_length + 3] = 40;
	iparams[TasksLength * task_params_length + 4] = 1;
	TasksLength++;

	tasks[TasksLength].priority = 0;
	tasks[TasksLength].function = &HandleKeyboardTask;
	TasksLength++;

	tasks[TasksLength].priority = 5;
	tasks[TasksLength].function = &DrawMouseTask;
	TasksLength++;

	// TasksLength++;


	// Before loop
	int last_tick = 0;

	while (1) {
		while (pit_ticks == last_tick) {
			__asm__ volatile ("hlt");
		}
		last_tick = pit_ticks;

		ProcessTasks();
		Flush();
	}
}
=======
#include "graphics.h"

int start() {
    VBEInfoBlock* VBE = (VBEInfoBlock*) VBEInfoAddress;

	mx = VBE->x_resolution / 2;
	my = VBE->y_resolution / 2;
	char str1[] = "Welcome to SaphireOS!\n\nText rendered by custom library.";
	char *p = str1;

	char characterBuffer[1000] = "\0";
	char* characterBufferPointer = characterBuffer;
	int characterBufferLength = 0;

	base0 = (unsigned int) &isr0;
	base = (unsigned int) &isr1;
	base12 = (unsigned int) &isr12;

	InitialiseMouse();
	InitPIT();
	InitialiseIDT();

	tasks[TasksLength].priority = 0;
	tasks[TasksLength].function = &ClearScreenTask;
	TasksLength++;

	tasks[TasksLength].priority = 0;
	tasks[TasksLength].function = &TaskbarTask;
	tasks[TasksLength].taskId = TasksLength;
	iparams[TasksLength * task_params_length + 0] = 0;
	iparams[TasksLength * task_params_length + 1] = 0;
	iparams[TasksLength * task_params_length + 2] = VBE->x_resolution;
	iparams[TasksLength * task_params_length + 3] = 40;
	iparams[TasksLength * task_params_length + 4] = 1;
	TasksLength++;

	tasks[TasksLength].priority = 0;
	tasks[TasksLength].function = &HandleKeyboardTask;
	TasksLength++;

	tasks[TasksLength].priority = 5;
	tasks[TasksLength].function = &DrawMouseTask;
	TasksLength++;

	int last_tick = 0;
	int flush_tick = 0;

	while (1) {
		while (pit_ticks == last_tick) {
			__asm__ volatile ("hlt");
		}
		last_tick = pit_ticks;
		
		flush_tick = (flush_tick + 1) % 2;
		
		if (screen_dirty == 1) {
			ProcessTasks();
			Flush();
		}
	}
}
>>>>>>> Stashed changes
