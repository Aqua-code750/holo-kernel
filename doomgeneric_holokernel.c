#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <stdint.h>

extern void draw_pixel(uint32_t x, uint32_t y, uint32_t color);
extern void sleep_ms(uint32_t ms);
extern uint32_t timer_ticks;
extern int keyboard_read(char *out);

// We need to implement DG_Init, DG_DrawFrame, DG_SleepMs, DG_GetTicksMs, DG_GetKey, DG_SetWindowTitle
void DG_Init(void) {
    // Nothing required right now
}

void DG_DrawFrame(void) {
    uint32_t x_offset = (800 - DOOMGENERIC_RESX) / 2;
    uint32_t y_offset = (600 - DOOMGENERIC_RESY) / 2;

    for (int y = 0; y < DOOMGENERIC_RESY; y++) {
        for (int x = 0; x < DOOMGENERIC_RESX; x++) {
            uint32_t color = DG_ScreenBuffer[y * DOOMGENERIC_RESX + x];
            draw_pixel(x_offset + x, y_offset + y, color);
        }
    }
}

void DG_SleepMs(uint32_t ms) {
    sleep_ms(ms);
}

uint32_t DG_GetTicksMs(void) {
    // Our PIT runs at 1000Hz, so 1 tick = 1 ms!
    return timer_ticks;
}

int DG_GetKey(int* pressed, unsigned char* doomKey) {
    char key;
    if (keyboard_read(&key)) {
        // Very basic mapping
        *pressed = 1; // Since we don't track key releases yet, just pretend everything is a press and hope for the best
        
        switch (key) {
            case '\n': *doomKey = KEY_ENTER; break;
            case '\b': *doomKey = KEY_BACKSPACE; break;
            // Map WASD to arrow keys just in case we don't get the scancodes for real arrows
            case 'w': *doomKey = KEY_UPARROW; break;
            case 's': *doomKey = KEY_DOWNARROW; break;
            case 'a': *doomKey = KEY_LEFTARROW; break;
            case 'd': *doomKey = KEY_RIGHTARROW; break;
            case ' ': *doomKey = ' '; break;
            default: *doomKey = key; break;
        }
        return 1;
    }
    return 0;
}

void DG_SetWindowTitle(const char * title) {
    // Ignore
}
