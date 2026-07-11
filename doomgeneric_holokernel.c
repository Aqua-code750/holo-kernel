#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <stdint.h>

extern void draw_pixel(uint32_t x, uint32_t y, uint32_t color);
extern void sleep_ms(uint32_t ms);
extern uint32_t timer_ticks;
extern int keyboard_read(uint8_t *out);

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
    uint8_t scancode = 0;
    if (keyboard_read(&scancode)) {
        // High bit set means key release
        *pressed = (scancode & 0x80) ? 0 : 1;
        scancode &= 0x7F; // strip the release bit to map it
        
        switch (scancode) {
            case 0x11: *doomKey = KEY_UPARROW; break;    // W -> Up Arrow
            case 0x1F: *doomKey = KEY_DOWNARROW; break;  // S -> Down Arrow
            case 0x1E: *doomKey = KEY_LEFTARROW; break;  // A -> Left Arrow
            case 0x20: *doomKey = KEY_RIGHTARROW; break; // D -> Right Arrow
            case 0x39: *doomKey = ' '; break;            // Space -> Space (Shoot)
            case 0x1D: *doomKey = KEY_FIRE; break;       // Left Ctrl -> Fire/Use (Doom internally maps fire/use depending on config, usually Ctrl is fire)
            case 0x1C: *doomKey = KEY_ENTER; break;      // Enter -> Enter
            case 0x01: *doomKey = KEY_ESCAPE; break;     // Esc -> Escape
            default: return 0; // Ignore other keys for now to avoid garbage input
        }
        return 1;
    }
    return 0;
}

void DG_SetWindowTitle(const char * title) {
    // Ignore
}
