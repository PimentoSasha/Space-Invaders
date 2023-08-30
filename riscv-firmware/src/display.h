#include <stdint.h>

void displayInit(void);

int16_t displayWriteText(char* text, uint16_t len, uint16_t offset);

int16_t displayAddSmallSprite(uint8_t *data, uint32_t w, uint32_t h, uint32_t ctrl);

int16_t displayUpdateSmallSprite(int16_t sid, uint32_t ctrl);

int16_t displayAddLargeSprite(uint8_t *data, uint32_t w, uint32_t h, uint32_t ctrl);

int16_t displayUpdateLargeSprite(int16_t sid, uint32_t ctrl);

int16_t displayAddBackground(uint8_t *data);

void displayRemoveSmallSprite(int16_t sid);

void displayRemoveLargeSprite(int16_t sid);

void displayRemoveBackground(int16_t bid);

void displayClearScreen();
