#include <stdint.h>

// Values used by the system and valuable to the cartridge
#define DISPLAY_WIDTH    512
#define DISPLAY_HEIGHT   288
#define SMALL_SPRITE_MAX 16
#define LARGE_SPRITE_MIN 33
#define LARGE_SPRITE_MAX 64

// Available colors provided by the firmware. Rudimentary by design. Contact the group to expand this list with specific values.
#define VBLACK   0
#define VWHITE   1
#define VRED     2
#define VGREEN   3
#define VBLUE    4
#define VTEAL    5
#define VYELLOW  6
#define VORANGE  7
#define VPINK    8
#define VPURPLE  9
#define VGREY    10
#define VBROWN   11

/**
 * Adds a string of text in the specified location. Returns an error code if any part of the string would go off-screen.
 * 
 * @param txt the string to print to the screen
 * @param len the length of \p txt
 * @param hOffset the horizontal offset of the first character in the string, in characters
 * @param vOffset the vertical offset of the first character in the string, in characters
 * 
 * @return 0 if successful, 1 if an error occurred
*/
uint16_t addText(char* txt, uint16_t len, uint16_t hOffset, uint16_t vOffset);

/**
 * Draws a small or large sprite to the screen. Returns an integer value of the sprite's index, to be used later.
 * There is bounds checking on the \p x and \p y coordinates, but if you want to 'hide' the sprite you can simply
 *  set x to DISPLAY_WIDTH and y to DISPLAY_HEIGHT.
 * Note that this function does not distinguish between large and small sprites (it determines which based on the
 *  input width and height) but the 'remove' functions below will need to be specified.
 * It is important to save the return value of the index on success, as the index will be how you move and remove
 *  the sprite later.
 * 
 * @param sprite a 1D array with the data for the sprite. A small example:
 *  uint8_t simple_sprite[3*4] = { VBLACK, VWHITE, VWHITE, VBLACK,
 *                                 VGREEN, VBLACK, VBLACK, VGREEN,
 *                                 VBLACK, VWHITE, VWHITE, VBLACK,
 *                               };
 * @param w the width of the sprite in pixels
 * @param h the height of the sprite in pixels
 * @param x the horizontal position on the screen to place the origin of the sprite in pixels
 * @param y the vertical position on the screen to place the origin of the sprite in pixels
 * @param z the order to draw the sprite. A sprite with a higher \p z is drawn later than those with a lower \p z
 * 
 * @return the index of the drawn sprite if successful, -1 if unsuccessful
*/
int16_t drawSprite(uint8_t* sprite, uint32_t w, uint32_t h, uint32_t x, uint32_t y, uint32_t z);

/**
 * Moves a sprite to the specified location.
 * There is bounds checking on the \p x and \p y coordinates, but if you want to 'hide' the sprite you can simply
 *  set x to DISPLAY_WIDTH and y to DISPLAY_HEIGHT.
 * Note that this function does not distinguish between large and small sprites (it determines which based on the
 *  input width and height) but the 'remove' functions below will need to be specified.
 * 
 * @param sid an integer ID of the referenced sprite, which was returned by the drawSprite function
 * @param w the width of the sprite in pixels. This must be the same as used in drawSprite, as changing it will give you undefined behavior.
 * @param h the height of the sprite in pixels. This must be the same as used in drawSprite, as changing it will give you undefined behavior.
 * @param x the horizontal position on the screen to place the origin of the sprite in pixels
 * @param y the vertical position on the screen to place the origin of the sprite in pixels
 * @param z the order to draw the sprite. A sprite with a higher \p z is drawn later than those with a lower \p z
 * 
 * @return 0 if successful, 1 if unsuccessful
*/
int16_t moveSprite(int16_t sid, uint32_t w, uint32_t h, uint32_t x, uint32_t y, uint32_t z);

/**
 * Removes the large sprite indicated by the input ID.
 * If this function is used to remove a sprite, the location for the sprite indicated by the ID will not be overwritten.
 * If moveSprite is used on the sprite with this ID after it has been removed, the behavior is undefined.
 * 
 * @param sid an integer ID of the referenced large sprite, which was returned by the drawSprite function
*/
void removeLargeSprite(int16_t sid);

/**
 * Removes the small sprite indicated by the input ID.
 * If this function is used to remove a sprite, the location for the sprite indicated by the ID will not be overwritten.
 * If moveSprite is used on the sprite with this ID after it has been removed, the behavior is undefined.
 * 
 * @param sid an integer ID of the referenced small sprite, which was returned by the drawSprite function
*/
void removeSmallSprite(int16_t sid);

/**
 * Adds a background image to the screen.
 * It is important to save the return value of the index on success, as the index will be how you remove
 *  the background later.
 * 
 * @param bg a 1D array with the data for the background. A small example:
 *  uint8_t simple_sprite[DISPLAY_HEIGHT*DISPLAY_WIDTH] = { VBLACK, VWHITE, ... VWHITE, VBLACK,
 *                                                          VGREEN, VBLACK, ... VBLACK, VGREEN,
 *                                                          ...     ...         ...     ...
 *                                                          VBLACK, VWHITE, ... VWHITE, VBLACK,
 *                                                        };
 * The color indices are shared between sprite and background, so the provided enum will work the same
 * 
 * @return the index of the drawn background if successful, -1 if unsuccessful
*/
int16_t addBackground(uint8_t *bg);

/**
 * Removes the background indicated by the input ID.
 * If this function is used to remove a background, the location for the background indicated by the ID will not be overwritten.
 * 
 * @param sid an integer ID of the referenced background, which was returned by the drawBackground function
*/
void removeBackground(int16_t bid);

/**
 * Removes all sprites and backgrounds. Resets the counters for the sprite and background indices.
*/
void clearScreen();

/**
 * A carbon copy of GCC's memcpy. Useful for initializing an array using the {...} syntax.
 * 
 * @param dest the destination address to copy the memory to
 * @param src the source address to copy the memory from
 * @param len the length in bytes of the memory to copy
*/
void *memcpy(void *dest, const void *src, uint16_t len);
