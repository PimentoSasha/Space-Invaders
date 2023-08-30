#include "video.h"

int16_t displayWriteText(char* text, uint16_t len, uint16_t offset);
int16_t displayAddSmallSprite(uint8_t *graphic, uint32_t w, uint32_t h, uint32_t ctrl);
int16_t displayUpdateSmallSprite(int16_t sid, uint32_t ctrl);
int16_t displayAddLargeSprite(uint8_t *graphic, uint32_t w, uint32_t h, uint32_t ctrl);
int16_t displayUpdateLargeSprite(int16_t sid, uint32_t ctrl);

void *
memcpy (void *dest, const void *src, uint16_t len) {
    char *d = dest;
    const char *s = src;
    while (len--)
        *d++ = *s++;
    return dest;
}

uint16_t addText(char* txt, uint16_t len, uint16_t hOffset, uint16_t vOffset) {
    if ( (hOffset + len - 1 > 0x3F) || (vOffset > 0x23) ) {
        return 1;
    }
    uint16_t offset = hOffset + 0x40 * vOffset;
    displayWriteText(txt, len, offset);
    return 0;
}

int16_t drawSprite(uint8_t* sprite, uint32_t w, uint32_t h, uint32_t x, uint32_t y, uint32_t z) {
    if ( !((x == DISPLAY_WIDTH) && (y == DISPLAY_HEIGHT)) ) {
        if ( (x > DISPLAY_WIDTH) || (x + w > DISPLAY_WIDTH) || (y > DISPLAY_HEIGHT) || (y + h > DISPLAY_HEIGHT) ) {
            return 1;
        }
    } 

    uint32_t ctrl;
    if ( w <= SMALL_SPRITE_MAX && h <= SMALL_SPRITE_MAX ) {
        ctrl = (z & 0x7) << 29 |
               ((h - 1) & 0xF) << 25 |
               ((w - 1) & 0xF) << 21 |
               ((y+16) & 0x1FF) << 12 |
               ((x+16) & 0x3FF) << 2;
        return displayAddSmallSprite(sprite, w, h, ctrl);

    } else if ( w < LARGE_SPRITE_MIN && h < LARGE_SPRITE_MIN ) {
        return -1; // Limitations in large sprites seem to imply that large sprites can't be smaller than 33x33

    } else if ( w <= LARGE_SPRITE_MAX && h <= LARGE_SPRITE_MAX ) {
        ctrl = ((h - 33) & 0x1F) << 26 |
               ((w - 33) & 0x1F) << 21 |
               ((y+64) & 0x1FF) << 12 |
               ((x+64) & 0x3FF) << 2;
        return displayAddLargeSprite(sprite, w, h, ctrl);
    }

    return -1;
}

int16_t moveSprite(int16_t sid, uint32_t w, uint32_t h, uint32_t x, uint32_t y, uint32_t z) {
    if ( !((x == DISPLAY_WIDTH) && (y == DISPLAY_HEIGHT)) ) {
        if ( (x > DISPLAY_WIDTH) || (x + w > DISPLAY_WIDTH) || (y > DISPLAY_HEIGHT) || (y + h > DISPLAY_HEIGHT) ) {
            return 1;
        }
    } 
         
    uint32_t ctrl;
    if ( w <= SMALL_SPRITE_MAX && h <= SMALL_SPRITE_MAX ) {
        ctrl = (z & 0x7) << 29 |
               ((h - 1) & 0xF) << 25 |
               ((w - 1) & 0xF) << 21 |
               ((y+16) & 0x1FF) << 12 |
               ((x+16) & 0x3FF) << 2;
        return displayUpdateSmallSprite(sid, ctrl);

    } else if ( w < LARGE_SPRITE_MIN && h < LARGE_SPRITE_MIN ) {
        return 1; // Limitations in large sprites seem to imply that large sprites can't be smaller than 33x33

    } else if ( w <= LARGE_SPRITE_MAX && h <= LARGE_SPRITE_MAX ) {
        ctrl = ((h - 33) & 0x1F) << 26 |
               ((w - 33) & 0x1F) << 21 |
               ((y+64) & 0x1FF) << 12 |
               ((x+64) & 0x3FF) << 2;
        return displayUpdateLargeSprite(sid, ctrl);
    }
    
    return 1;
}