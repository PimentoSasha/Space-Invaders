#include <stdint.h>
#include <stdlib.h>
#include "display.h"
#include "timer_interrupts.h"
#include "thread_scheduler.h"
#include "mutex.h"

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

// Adapted from https://stackoverflow.com/questions/58947716/how-to-interact-with-risc-v-csrs-by-using-gcc-c-code
__attribute__((always_inline)) inline uint32_t csr_mstatus_read(void){
    uint32_t result;
    asm volatile ("csrr %0, mstatus" : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void csr_mstatus_write(uint32_t val){
    asm volatile ("csrw mstatus, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_write_mie(uint32_t val){
    asm volatile ("csrw mie, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_enable_interrupts(void){
    asm volatile ("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void){
    asm volatile ("csrci mstatus, 0x8");
}

__attribute__((always_inline)) inline uint32_t csr_mip_read(void){
  uint32_t result;
  asm volatile ("csrr %0, mip" : "=r"(result));
  return result;
}

#define MTIME_LOW       (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH      (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW    (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH   (*((volatile uint32_t *)0x40000014))
#define CONTROLLER      (*((volatile uint32_t *)0x40000018))
#define INTERRUPT_ENABLE   (*((volatile uint32_t *)0x40000000))
#define INTERRUPT_PENDING  (*((volatile uint32_t *)0x40000004))

void init(void){
    uint8_t *Source = _erodata;
    uint8_t *Base = _data < _sdata ? _data : _sdata;
    uint8_t *End = _edata > _esdata ? _edata : _esdata;

    while(Base < End){
        *Base++ = *Source++;
    }
    Base = _bss;
    End = _ebss;
    while(Base < End){
        *Base++ = 0;
    }

    initialize_threads();

    csr_write_mie(0x888);       // Enable all interrupt soruces
    MTIMECMP_LOW = 1;
    MTIMECMP_HIGH = 0;
    displayInit();
}

extern volatile int global;
extern volatile uint32_t controller_status;
volatile uint32_t cmd_status = 0;
volatile uint32_t prev_cmd_status = 0;

void reset_timer() {
  uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH)<<32) | MTIMECMP_LOW;
  NewCompare += 100;
  MTIMECMP_HIGH = NewCompare>>32;
  MTIMECMP_LOW = NewCompare;
}

uint32_t c_interrupt_handler(uint32_t mcause){
  if (INTERRUPT_PENDING & 0x4) {
    cmdButtonPressed();
    //    cmd_status = !cmd_status;
    INTERRUPT_PENDING = 0x4;
  }
  else if (csr_mip_read() & 0x80) {
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH)<<32) | MTIMECMP_LOW;
    //    NewCompare += 1000;
    //   MTIMECMP_HIGH = NewCompare>>32;
    //   MTIMECMP_LOW = NewCompare;
    timer_interrupt_handler();
    reset_timer();
  }
  /*
  if(INTERRUPT_PENDING & 0x2) { 
    global++;
    controller_status = CONTROLLER;
    INTERRUPT_PENDING = 0x2;
  }
  */
}


uint32_t c_system_call(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t call){
    switch (call)
    {
    case 0:
        return global;
        break;
    case 1:
        return CONTROLLER;
        break;
    case 2:
        return displayWriteText((char*)(a0), (uint16_t)(a1), (uint16_t)(a2));
        break;
    case 3:
        return displayAddSmallSprite((uint8_t*)(a0), a1, a2, a3);
        break;
    case 4:
        return displayUpdateSmallSprite((int16_t)(a0), a1);
        break;
    case 5:
        return displayAddLargeSprite((uint8_t*)(a0), a1, a2, a3);
        break;
    case 6:
        return displayUpdateLargeSprite((int16_t)(a0), a1);
        break;
    case 7:
        return displayAddBackground((uint8_t*)(a0));
        break;
    case 8:
        if (cmd_status != prev_cmd_status) {
            prev_cmd_status = cmd_status;
            return 1;
        }
        return 0;
        break;
    case 9:
        return rand();
        break;
    case 10:
        srand(a0);
        return 0;
        break;
    case 11:
        displayRemoveLargeSprite((int16_t)(a0));
        return 0;
        break;
    case 12:
        displayRemoveSmallSprite((int16_t)(a0));
        return 0;
        break;
    case 13:
        displayRemoveBackground((int16_t)(a0));
        return 0;
        break;
    case 14:
        displayClearScreen();
        return 0;
        break;
    case 15:
        setButtonClicked((void (*)(uint32_t)) a0);
	return 0;
	break;
    case 16:
        setButtonReleased((void (*)(uint32_t)) a0);
	return 0;
	break;
    case 17:
        setButtonChanged((void (*)(uint32_t)) a0);
	return 0;
	break;
    case 18:
        setCmdPressed((void (*)(void)) a0);
	return 0;
	break;
    case 19:
        yield(); 
	return 0;
	break;
    case 20:
        join(a0); 
	return 0;
	break;
    case 21:
        return finish_thread();
	break;
    case 22:
        return kill();
	break;
    case 23:
	return syscall_new_thread((void (*)()) a0);
	break;
    case 24:
        wait(a0); 
	return 0;
	break; 
    case 25:
      return pthread_mutex_lock((mutex_t*) a0);
      break;
    case 26:
      return pthread_mutex_unlock((mutex_t*) a0);
      break;
    default:
        break;
    }
    return -1;
}

