#include “video.h”
#include "thread.h"

int16_t velocity = 0; // Should only access from setDirection()
uint16_t x = get_width() / 2;
uint16_t LEFT_BUTTON = 0x0001; // Needs actual value
uint16_t RIGHT_BUTTON = 0x0002; // Needs actual value

void setDirection(int32_t button_status) {
    bool left_pushed = button_state & LEFT_BUTTON;
    bool right_pushed = button_state & RIGHT_BUTTON;
    if (left_push && !right_pushed) {
        velocity = 1;
    } else if (!left_push && right_pushed) {
        velocity = -1;
    } else {
        velocity = 0;
    }
    return;
}

void move_and_draw_starfighter() {
    uint16_t y = 10;
    uint16_t z = 0;
    while (1) {
        if (isScreenUpdated()){ // Not a real function, not sure what this should actually do though
            x += velocity;
            clearScreen();
            drawSmallSprite(starfighter_sprite, x, y, 0, starfighter_palette); // Do we need to redraw if this doesn't move?
	}
    }
}

int main(void) {
    int16_t* starfighter_sprite = malloc(256);
    starfighter_sprite = fRead('starfighter.sprite');

    int16_t* starfighter_palette = malloc(4*sizeof(int32_t));
    void setSpritePalettes(int32_t p1, int32_t p2, int32_t p3, int32_t p4);

    void (*button_listener)(int);
    button_listener = &setDirection;
    setOnButtonChangeListener(button_listener); // Can we just stipulate that the thread used for this will count towards the thread quota?

    void (*update_starfighter)(int);
    update_starfighter = &move_and_draw_starfigher;

    uint16_t update_starfighter_thread = thread(update_starfighter);

    join(update_starfighter_thread);
}



