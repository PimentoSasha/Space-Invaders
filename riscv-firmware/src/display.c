#include "display.h"

#define DISPLAY_WIDTH    512
#define DISPLAY_HEIGHT   288
#define SMALL_SPRITE_STRIDE 16
#define LARGE_SPRITE_STRIDE 64

volatile uint32_t *VIDEO_MODE_CONTROL = (volatile uint32_t *)(0x500FF414);
volatile char *TEXT_MEMORY = (volatile char *)(0x500FE800);
volatile uint32_t *SPRITE_PALETTES = (volatile uint32_t *)(0x500FD000);
volatile uint8_t *SMALL_SPRITE_DATA = (volatile uint8_t *)(0x500F4000);
volatile uint32_t *SMALL_SPRITE_CONTROL = (volatile uint32_t *)(0x500FF214);
volatile uint8_t *LARGE_SPRITE_DATA = (volatile uint8_t *)(0x500B4000);
volatile uint32_t *LARGE_SPRITE_CONTROL = (volatile uint32_t *)(0x500FF114);
volatile uint32_t *BACKGROUND_PALETTES = (volatile uint32_t *)(0x500FC000);
volatile uint8_t *BACKGROUND_DATA = (volatile uint8_t *)(0x50000000);
volatile uint32_t *BACKGROUND_CONTROL = (volatile uint32_t *)(0x500FF100);

uint16_t smallSpriteCount, largeSpriteCount, backgroundCount;

void displayInit(void) {
    SPRITE_PALETTES[0] = 0x00000000; // Black
    SPRITE_PALETTES[1] = 0xFFFFFFFF; // White
    SPRITE_PALETTES[2] = 0xFFFF0000; // Red
    SPRITE_PALETTES[3] = 0xFF00FF00; // Green
    SPRITE_PALETTES[4] = 0xFF0000FF; // Blue
    SPRITE_PALETTES[5] = 0xFF00FFFF; // Teal
    SPRITE_PALETTES[6] = 0xFFFFF700; // Yellow
    SPRITE_PALETTES[7] = 0xFFFF9A00; // Orange
    SPRITE_PALETTES[8] = 0xFFFF00FF; // Pink
    SPRITE_PALETTES[9] = 0xFF8900FF; // Purple
    SPRITE_PALETTES[10] = 0xFF808080; // Grey
    SPRITE_PALETTES[11] = 0xFF663300; // Brown

    BACKGROUND_PALETTES[0] = 0x00000000; // Black
    BACKGROUND_PALETTES[1] = 0xFFFFFFFF; // White
    BACKGROUND_PALETTES[2] = 0xFFFF0000; // Red
    BACKGROUND_PALETTES[3] = 0xFF00FF00; // Green
    BACKGROUND_PALETTES[4] = 0xFF0000FF; // Blue
    BACKGROUND_PALETTES[5] = 0xFF00FFFF; // Teal
    BACKGROUND_PALETTES[7] = 0xFFFF9A00; // Orange
    BACKGROUND_PALETTES[8] = 0xFFFF00FF; // Pink
    BACKGROUND_PALETTES[9] = 0xFF8900FF; // Purple
    BACKGROUND_PALETTES[10] = 0xFF808080; // Grey
    BACKGROUND_PALETTES[11] = 0xFF663300; // Brown

    smallSpriteCount = 0;
    largeSpriteCount = 0;
    backgroundCount = 0;
}

int16_t displayWriteText(char* text, uint16_t len, uint16_t offset) {
    (*VIDEO_MODE_CONTROL) = (*VIDEO_MODE_CONTROL) & 0xFFFFFFE;
    for (uint16_t i = 0; i < len; i++) {
        TEXT_MEMORY[i + offset] = text[i];
    }
    return 0;
}

int16_t displayAddSmallSprite(uint8_t *data, uint32_t w, uint32_t h, uint32_t ctrl) {
    if (smallSpriteCount > 127) {
        // Only 127 small sprites allowed
        return -1;
    }
    (*VIDEO_MODE_CONTROL) = (*VIDEO_MODE_CONTROL) | 0x1;

    volatile uint8_t *SMALL_SPRITE_LOC = SMALL_SPRITE_DATA + (smallSpriteCount * SMALL_SPRITE_STRIDE * SMALL_SPRITE_STRIDE);

    for (uint8_t i = 0; i < h; i++) {
        for (uint8_t j = 0; j < w; j++) {
            SMALL_SPRITE_LOC[j + i*SMALL_SPRITE_STRIDE] =  data[j+w*i];
        }
    }

    SMALL_SPRITE_CONTROL[smallSpriteCount] = ctrl;
    return smallSpriteCount++;
}

int16_t displayUpdateSmallSprite(int16_t sid, uint32_t ctrl) {
    (*VIDEO_MODE_CONTROL) = (*VIDEO_MODE_CONTROL) | 0x1;

    SMALL_SPRITE_CONTROL[sid] = ctrl;
    return 0;
}

int16_t displayAddLargeSprite(uint8_t *data, uint32_t w, uint32_t h, uint32_t ctrl) {
    if (largeSpriteCount > 63) {
        // Only 63 large sprites allowed
        return -1;
    }
    (*VIDEO_MODE_CONTROL) = (*VIDEO_MODE_CONTROL) | 0x1;

    volatile uint8_t *LARGE_SPRITE_LOC = LARGE_SPRITE_DATA + (largeSpriteCount * LARGE_SPRITE_STRIDE * LARGE_SPRITE_STRIDE);

    for (uint8_t i = 0; i < h; i++) {
        for (uint8_t j = 0; j < w; j++) {
            LARGE_SPRITE_LOC[j + i*LARGE_SPRITE_STRIDE] =  data[j+w*i];
        }
    }

    LARGE_SPRITE_CONTROL[largeSpriteCount] = ctrl;
    return largeSpriteCount++;
}

int16_t displayUpdateLargeSprite(int16_t sid, uint32_t ctrl) {
    (*VIDEO_MODE_CONTROL) = (*VIDEO_MODE_CONTROL) | 0x1;

    LARGE_SPRITE_CONTROL[sid] = ctrl;
    return sid;
}

int16_t displayAddBackground(uint8_t *data) {
    if (backgroundCount > 4) {
        // Only 5 backgrounds allowed
        return -1;
    }
    (*VIDEO_MODE_CONTROL) = (*VIDEO_MODE_CONTROL) | 0x1;

    volatile uint8_t *BACKGROUND_LOC = BACKGROUND_DATA + (backgroundCount * DISPLAY_HEIGHT * DISPLAY_WIDTH);

    for (uint16_t i = 0; i < DISPLAY_HEIGHT; i++) {
        for (uint16_t j = 0; j < DISPLAY_WIDTH; j++) {
            BACKGROUND_LOC[i * DISPLAY_WIDTH + j] =  data[i * DISPLAY_WIDTH + j];
        }
    }

    uint32_t ctrl = ((DISPLAY_HEIGHT) & 0x3FF) << 12 |
                    ((DISPLAY_WIDTH) & 0x3FF) << 2;
    BACKGROUND_CONTROL[backgroundCount] = ctrl;

    backgroundCount++;
    return 0;
}

void displayRemoveSmallSprite(int16_t sid) {
    SMALL_SPRITE_CONTROL[sid] = 0;

    volatile uint8_t *SMALL_SPRITE_LOC = SMALL_SPRITE_DATA + (sid * SMALL_SPRITE_STRIDE * SMALL_SPRITE_STRIDE);

    for (uint8_t i = 0; i < SMALL_SPRITE_STRIDE; i++) {
        for (uint8_t j = 0; j < SMALL_SPRITE_STRIDE; j++) {
            SMALL_SPRITE_LOC[j + i*SMALL_SPRITE_STRIDE] = 0;
        }
    }
}

void displayRemoveLargeSprite(int16_t sid) {
    LARGE_SPRITE_CONTROL[sid] = 0;

    volatile uint8_t *LARGE_SPRITE_LOC = LARGE_SPRITE_DATA + (sid * LARGE_SPRITE_STRIDE * LARGE_SPRITE_STRIDE);

    for (uint8_t i = 0; i < LARGE_SPRITE_STRIDE; i++) {
        for (uint8_t j = 0; j < LARGE_SPRITE_STRIDE; j++) {
            LARGE_SPRITE_LOC[j + i*LARGE_SPRITE_STRIDE] = 0;
        }
    }
}

void displayRemoveBackground(int16_t bid) {
    BACKGROUND_CONTROL[bid] = 0;

    volatile uint8_t *BACKGROUND_LOC = BACKGROUND_DATA + (bid * DISPLAY_HEIGHT * DISPLAY_WIDTH);

    for (uint8_t i = 0; i < DISPLAY_HEIGHT; i++) {
        for (uint8_t j = 0; j < DISPLAY_WIDTH; j++) {
            BACKGROUND_LOC[j + i*DISPLAY_WIDTH] = 0;
        }
    }
}

void displayClearScreen() {
    for (; largeSpriteCount > 0; largeSpriteCount--) {
        displayRemoveLargeSprite(largeSpriteCount-1);
    }
    for (; smallSpriteCount > 0; smallSpriteCount--) {
        displayRemoveSmallSprite(smallSpriteCount-1);
    }
    for (; backgroundCount > 0; backgroundCount--) {
        displayRemoveBackground(backgroundCount-1);
    }
}