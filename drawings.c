#include "display.h"
#include "drawings.h"
#include <stdint.h>

void draw_rectangle_1(void) {
    Filled_rectangle(10, 69, 20, 100, GRAPHICS_COLOR_RED);
}

void clear_rectangle_1(void) {
    Filled_rectangle(10, 69, 20, 100, GRAPHICS_COLOR_WHITE);
}

void rectangle(void) {
    Filled_rectangle(10, 69, 20, 100, GRAPHICS_COLOR_RED);
}

void clear_rectangle(void) {
    Filled_rectangle(10, 69, 20, 100, GRAPHICS_COLOR_WHITE);
}

void circle(void) {
    Filled_circle(80, 90, 30, GRAPHICS_COLOR_GREEN);
}

void clear_circle(void) {
    Filled_circle(80, 90, 30, GRAPHICS_COLOR_WHITE);
}

int8_t clear_drawing(uint8_t fig_number) {
    if (fig_number == 1) {
        Filled_rectangle(0, 19, 0, 99, GRAPHICS_COLOR_WHITE);
        return 0;
    } else if (fig_number == 2) {
        Filled_circle(25, 50, 20, GRAPHICS_COLOR_WHITE);
        return 0;
    } else {
        return -1;
    }
}
