#include "graphics.h"

int shift_pressed = FALSE;
int caps_pressed = FALSE;
int escape_pressed = FALSE;
int backspace_pressed = FALSE;
int alt_pressed = FALSE;
int ctrl_pressed = FALSE;
int enter_pressed = FALSE;

unsigned char ProcessScancode(int scancode) {
    if (scancode ==  0x01)
        escape_pressed = TRUE;

    else if (scancode ==  0x02)
        if (shift_pressed == TRUE)
            return '!';
        else
            return '1';

    else if (scancode ==  0x03)
        if (shift_pressed == TRUE)
            return '"';
        else
            return '2';

    else if (scancode ==  0x04)
        if (shift_pressed == TRUE)
            return '#';
        else
            return '3';

    else if (scancode ==  0x05)
        if (shift_pressed == TRUE)
            return '$';
        else
            return '4';

    else if (scancode == 0x06)
        if (shift_pressed == TRUE)
            return '%';
        else
            return '5';

    else if (scancode == 0x07)
        if (shift_pressed == TRUE)
            return '^';
        else
            return '6';

    else if (scancode == 0x08)
        if (shift_pressed == TRUE)
            return '&';
        else
            return '7';

    else if (scancode == 0x09)
        if (shift_pressed == TRUE)
            return '*';
        else
            return '8';

    else if (scancode == 0x0A)
        if (shift_pressed == TRUE)
            return '(';
        else
            return '9';

    else if (scancode == 0x0B)
        if (shift_pressed == TRUE)
            return ')';
        else
            return '0';

    else if (scancode == 0x0C)
        if (shift_pressed == TRUE)
            return '_';
        else
            return '-';

    else if (scancode == 0x0D)
        if (shift_pressed == TRUE)
            return '+';
        else
            return '=';

    else if (scancode == 0x0E)
        backspace_pressed = TRUE;

    else if (scancode == 0x0F)
        return '\t';

    else if (scancode == 0x10)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'Q';
        else
            return 'q';

    else if (scancode == 0x11)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'W';
        else
            return 'w';

    else if (scancode == 0x12)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'E';
        else
            return 'e';

    else if (scancode == 0x13)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'R';
        else
            return 'r';

    else if (scancode == 0x14)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'T';
        else
            return 't';

    else if (scancode == 0x15)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'Y';
        else
            return 'y';

    else if (scancode == 0x16)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'U';
        else
            return 'u';

    else if (scancode == 0x17)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'I';
        else
            return 'i';

    else if (scancode == 0x18)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'O';
        else
            return 'o';

    else if (scancode == 0x19)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'P';
        else
            return 'p';

    else if (scancode == 0x1A)
        if (shift_pressed == TRUE)
            return '{';
        else
            return '[';

    else if (scancode == 0x1B)
        if (shift_pressed == TRUE)
            return '}';
        else
            return ']';

    else if (scancode == 0x1C) {
        enter_pressed = TRUE;
        return '\n';
    }

    else if (scancode == 0x1D)
        ctrl_pressed = TRUE;

    else if (scancode == 0x1E)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'A';
        else
            return 'a';

    else if (scancode == 0x1F)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'S';
        else
            return 's';

    else if (scancode == 0x20)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'D';
        else
            return 'd';

    else if (scancode == 0x21)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'F';
        else
            return 'f';

    else if (scancode == 0x22)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'G';
        else
            return 'g';

    else if (scancode == 0x23)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'H';
        else
            return 'h';

    else if (scancode == 0x24)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'J';
        else
            return 'j';

    else if (scancode == 0x25)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'K';
        else
            return 'k';

    else if (scancode == 0x26)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'L';
        else
            return 'l';

    else if (scancode == 0x27)
        if (shift_pressed == TRUE)
            return ':';
        else
            return ';';

    else if (scancode == 0x28)
        if (shift_pressed == TRUE)
            return '@';
        else
            return '\'';

    else if (scancode == 0x29)
        if (shift_pressed == TRUE)
            return '~';
        else
            return '`';

    else if (scancode == 0x2A) {
        shift_pressed = TRUE;
        Scancode = -1;
    }

    else if (scancode == 0x2B)
        if (shift_pressed == TRUE)
            return '|';
        else
            return '\\';

    else if (scancode == 0x2C)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'Z';
        else
            return 'z';

    else if (scancode == 0x2D)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'X';
        else
            return 'x';

    else if (scancode == 0x2E)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'C';
        else
            return 'c';

    else if (scancode == 0x2F)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'V';
        else
            return 'v';

    else if (scancode == 0x30)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'B';
        else
            return 'b';

    else if (scancode == 0x31)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'N';
        else
            return 'n';

    else if (scancode == 0x32)
        if (shift_pressed == TRUE || caps_pressed == TRUE)
            return 'M';
        else
            return 'm';

    else if (scancode == 0x33)
        if (shift_pressed == TRUE)
            return '<';
        else
            return ',';

    else if (scancode == 0x34)
        if (shift_pressed == TRUE)
            return '>';
        else
            return '.';

    else if (scancode == 0x35)
        if (shift_pressed == TRUE)
            return '?';
        else
            return '/';

    else if (scancode == 0x36) {
        shift_pressed = TRUE;
        Scancode = -1;
    }

    else if (scancode == 0x38)
        alt_pressed = TRUE;

    else if (scancode == 0x39)
        return ' ';

    else if (scancode == 0x3A) {
        if (caps_pressed == TRUE)
            caps_pressed = FALSE;
        else if (caps_pressed == FALSE)
            caps_pressed = TRUE;
        Scancode = -1;
    }

    if (scancode == 0xAA) {
        shift_pressed = FALSE;
        Scancode = -1;
    }

    if (scancode == 0xB6) {
        shift_pressed = FALSE;
        Scancode = -1;
    }

    return '\0';
}
