# AGENTS.md - Nyanix (SaphireOS) Agent Guidelines

## Build Commands
- `make` - Build os.img (MBR + kernel)
- `make run` - Build and run in QEMU
- `make clear` - Clean build artifacts

## Testing
No formal test framework exists. Verification is done through:
1. Visual inspection when running in QEMU (`make run`)
2. Manual testing of individual components
3. Boot verification (system reaches cooperative task loop)

To test a single component:
1. Modify the relevant .c file in boot/
2. Rebuild with `make`
3. Run with `make run` and observe behavior

## Code Style Guidelines

### Language & Constraints
- C99 standard (gcc -m32 -ffreestanding)
- No libc usage - all functions must be implemented manually
- String literals limited to ≤61 characters (stack allocation constraint)
- Fixed memory layout: kernel loads at 0x1000, stack at 0x90000
- No dynamic allocation - all memory pre-allocated
- VBE fixed at mode 0x111 (640x480 16bpp)

### Formatting
- 4-space indentation
- Opening braces on same line as control statement
- Line length ideally < 100 characters
- File headers: Brief description of file purpose
- Function comments: Describe parameters, return value, side effects

### Imports & Inclusion Model
- boot/final.c #includes all .c files in specific order:
  graphics.c → font.c → input.c → graphics_elements.c → task.c → main.c
- No separate compilation - all code compiled as single translation unit
- Header files (.h) contain only struct declarations and function prototypes
- Global variables shared via inclusion order (defined in input.c)

### Types
- Fixed-width types preferred when size matters:
  - unsigned short for VBE framebuffer (RGB565)
  - int for general purpose
  - char for characters/strings
  - Function pointers for task system: int (*function)(int taskId)
- Custom types defined in headers:
  - VBEInfoBlock (from VBE spec)
  - Task (in task.c)
  - Various structs in graphics_elements.c

### Naming Conventions
- Functions: CamelCase (DrawString, ProcessTasks, InitialiseMouse)
- Variables: camelCase (screen_dirty, pit_ticks, mx, my)
- Constants: ALL_CAPS_WITH_UNDERSCORES (VBEInfoAddress, ScreenBufferAddress)
- Structs: PascalCase (VBEInfoBlock, Task)
- Enums: PascalCase with _t suffix if needed
- Macros: ALL_CAPS_WITH_UNDERSCORES
- File names: snake_case (graphics.c, font.c)

### Error Handling
- No exceptions - return error codes where appropriate
- Check return values from all functions that can fail
- Assert critical invariants (though limited due to freestanding)
- Graceful degradation preferred over halting
- Infinite loops with hlt for idle states

### Color System
- RGB565 format: r<<11 | g<<5 | b
- Valid ranges: R 0-16, G 0-32, B 0-16 (not 0-255)
- Helper function rgb(r,g,b) in graphics.c
- Never use raw 24-bit color values

### Task System
- struct Task tasks[256] in task.c
- Priority 0-5 (0 = highest priority)
- Function signature: int (*function)(int taskId)
- Parameters: iparams[taskId * task_params_length + N] (10 slots per task)
- mouse_possessed_task_id global - tasks must check before consuming clicks
- Tasks cooperatively yield via main loop

### Input Handling
- Keyboard: IDT vector 1, Scancode global set by HandleISR1
- Mouse: IDT vector 12, 3-byte packets → mx/my/left_clicked/right_clicked globals
- Direct global access - no abstraction layer
- Tasks must poll globals rather than using events/callbacks

### UI Elements
- Windows: Draggable, 20px title bar, close circle (DrawWindow)
- Buttons: Hover+click detection, scoped to mouse_possessed_task_id (DrawButton)
- All coordinates absolute - no relative positioning
- Manual double buffering via Flush() (copies off-screen buffer to VBE->screen_ptr)

### Font System
- getArialCharacter(charIndex, row) returns bitmap row
- Pass function pointer to DrawString()/DrawCharacter()
- Use font_arial_width/font_arial_height for dimensions
- Monospace font rendering

### Memory Regions
- 0x7c00: MBR (boot/boot.asm)
- 0x8000: VBEInfoBlock
- 0x1000: Kernel entry point (boot/kernel_entry.asm)
- 0xffff0: Off-screen buffer (RGB565, 640x480x2 bytes)
- 0x90000: Stack top (grows downward)

### String Limitations
- Maximum 61 characters for string literals
- Use character buffers for dynamic strings
- No standard string functions (strlen, strcpy, etc.) - implement manually
- Null-terminated strings expected by drawing functions

### Assembly Integration
- Inline assembly with __asm__ volatile for hlt, cli, sti
- External assembly files: boot/boot.asm, boot/kernel_entry.asm
- Linker script: boot/kernel.ld
- Interrupt handlers named isrN (in final.c)

### Performance Considerations
- Cooperative multitasking - no preemption
- PIT configured for ~100Hz (10ms ticks)
- hlt used in idle loops to reduce power consumption
- Screen updates only when dirty (screen_dirty flag)
- Minimize work in interrupt handlers

### Common Patterns
- Global state via inclusion order (see input.c for shared globals)
- Function pointers for callbacks/tasks
- Bit manipulation for graphics/mouse cursors
- Polling vs interrupts: Keyboard/mouse use interrupts but expose via globals
- Double buffering to prevent tearing

### Portability Notes
- Specifically targets x86 architecture
- Relies on VBE BIOS extensions
- Requires QEMU or real hardware with VBE support
- Not portable to other architectures without significant changes