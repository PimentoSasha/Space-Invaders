#include <stddef.h>
#include "malloc.h"
#include "video.h"
#include "control.h"
#include "sprite.h"

volatile int global = 42;
volatile uint32_t controller_status = 0;

__attribute__((always_inline)) inline void csr_enable_interrupts(void){
  asm volatile ("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void){
  asm volatile ("csrci mstatus, 0x8");
}

void* t_malloc(size_t size) {
  csr_disable_interrupts();
  void *requested_memory = malloc(size);
  csr_enable_interrupts();
  return requested_memory;
}

mutex_t mutex;
uint32_t bullet_x_pos = 0, bullet_y_pos = 0;
int16_t bulletID = -1;
uint32_t x_pos = 0;
uint32_t y_pos = 0;

void clickListener(uint32_t status) {
  if((status & 0x10) && (bulletID == -1)){
    bullet_x_pos = x_pos + SHIP_W / 2;
    bullet_y_pos = y_pos;
    bulletID = drawSprite(bullet, BULLET_W, BULLET_H, bullet_x_pos, bullet_y_pos, 1);
  }
}

void cmdListener(uint32_t status) {
  if(bulletID == -1){
    bullet_x_pos = x_pos + SHIP_W / 2;
    bullet_y_pos = y_pos;
    bulletID = drawSprite(bullet, BULLET_W, BULLET_H, bullet_x_pos, bullet_y_pos, 1);
  }
}

void updateBullets() {
  int last_global = 42;
  while (1) {
      while (!pthread_mutex_lock(&mutex)) {
        yield();
      }
      if (bulletID != -1) {
        bullet_y_pos--;
        uint32_t retCode = moveSprite(bulletID, BULLET_W, BULLET_H, bullet_x_pos, bullet_y_pos, 1);
        if (retCode) {
          removeSmallSprite(bulletID);
          bulletID = -1;
        }
      }
      last_global = global;
      pthread_mutex_unlock(&mutex);
      wait(100);
  }     
}


uint32_t tid;
void killOtherThread() {
  kill(tid);
}

void makeBackground() {
    uint8_t bg[DISPLAY_HEIGHT*DISPLAY_WIDTH];
    srand(1234);
    for (uint16_t i = 0; i < DISPLAY_HEIGHT; i++) {
        for (uint16_t j = 0; j < DISPLAY_WIDTH; j++) {
            bg[ i * DISPLAY_WIDTH + j ] = ( rand() % 200 == 0 ) ? VWHITE : VBLACK;
        }
    }    
    int16_t bid = addBackground(bg);
    thread(&killOtherThread);    
    while(1);
}

int main() {
    pthread_mutex_init(&mutex);
    int last_global = 42;
    uint32_t x_pos_prev = x_pos, y_pos_prev = y_pos;
    int textMode = 0;

    tid = thread(&makeBackground);
    join(tid);

    int16_t shipID = -1, alienID = -1;

    shipID = drawSprite(ship, SHIP_W, SHIP_H, x_pos, y_pos, 0);
    alienID = drawSprite(alien, ALIEN_W, ALIEN_H, DISPLAY_WIDTH - x_pos - ALIEN_W, DISPLAY_HEIGHT - y_pos - ALIEN_H, 0);

    // int *hi = t_malloc(4 * sizeof(int));

    setOnClickListener(&clickListener);
    setCmdListener(&cmdListener);
    thread(&updateBullets);

    while (1) {
        global = getTicks();
	/*        uint32_t cmdStatus = getCmdBtnStatus();
        if (cmdStatus) {
            clearScreen();
	    }*/
        if(global != last_global){
	  while (!pthread_mutex_lock(&mutex)) {
	    yield();
	  }
            controller_status = getControllerStatus();
            if(controller_status){
                if(controller_status & 0x1){
                    x_pos-=1;
                }
                if(controller_status & 0x2){
                    y_pos-=1;
                }
                if(controller_status & 0x4){
                    y_pos+=1;
                }
                if(controller_status & 0x8){
                    x_pos+=1;
                }
                if(controller_status & 0x20) {
                    x_pos = 0;
                    y_pos = 0;
                }
                if(controller_status & 0x40) {
                    x_pos = DISPLAY_WIDTH;
                    y_pos = DISPLAY_HEIGHT;
                }

                uint32_t retCode = 0;
                retCode = moveSprite(shipID, SHIP_W, SHIP_H, x_pos, y_pos, 0);
                // retCode |= moveSprite(alienID, ALIEN_W, ALIEN_H, DISPLAY_WIDTH - x_pos - ALIEN_W, DISPLAY_HEIGHT - y_pos - ALIEN_H, 0);
                
                if ( retCode ) {
                    x_pos = x_pos_prev;
                    y_pos = y_pos_prev;
                } else {
                    x_pos_prev = x_pos;
                    y_pos_prev = y_pos;
                }
                
            }
            last_global = global;
	    pthread_mutex_unlock(&mutex);
	    yield();
        }
    }
    return 0;
}

void abort() {
  while(1);
}

void* sbrk(size_t requested_size) {
  extern char _heap[];
  extern char _stack[];
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == 0) {
    heap_end = _heap;
  }
  prev_heap_end = heap_end;
  if (heap_end + requested_size > _stack) {
    abort();
  }
  heap_end += requested_size;
  return prev_heap_end;
}
