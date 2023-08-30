#include <stdint.h>
#include "thread_scheduler.h"

volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile uint32_t *CARTRIDGE = (volatile uint32_t *)(0x4000001C);
typedef void (*FunPtr)(void);
int main() {
    while(1){
        if((*CARTRIDGE) & 0x1){
	  new_thread((FunPtr)((*CARTRIDGE) & 0xFFFFFFFC));
	  start_thread_rotation();
        }
    }
    return 0;
}
