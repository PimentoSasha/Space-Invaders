#include <stdint.h>
#include "thread_scheduler.h"

#define MTIME_LOW          (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH         (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW       (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH      (*((volatile uint32_t *)0x40000014))
#define INTERRUPT_ENABLE   (*((volatile uint32_t *)0x40000000))
#define INTERRUPT_PENDING  (*((volatile uint32_t *)0x40000004))
#define CONTROLLER         (*((volatile uint32_t *)0x40000018))

extern volatile int global;
extern volatile uint32_t controller_status;

uint32_t old_controller_value = 0;
uint32_t new_controller_value = 0;

void timer_interrupt_handler() {
  uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH)<<32) | MTIMECMP_LOW;
  //  NewCompare += 1000;
  //  MTIMECMP_HIGH = NewCompare>>32;
  //  MTIMECMP_LOW = NewCompare;
  global++;
  controller_status = CONTROLLER;
  old_controller_value = new_controller_value;
  new_controller_value = CONTROLLER;
  save_previous_sp();
  resume_next_thread();
}
/*
void external_interrupt_handler() {
  return;
}

void resume_next_thread() {
    current_thread_slot = get_next_thread_slot();
    thread_status current_thread = get_thread_status(current_thread);
    resume_thread(current_thread);
}
*/
/*
void timer_interrupt_handler() {
  //    if (is_event_thread(current_thread_slot)) { // Event thread didn't complete before next timer interrupt
  //      printf("Timed out waiting for response from button click.");
  //      kill_program();
  //    }
    if (button_changed()) {
        start_button_changed_thread(); // Also needs to reset timer before
        if (button_clicked()) {
            start_button_clicked_thread(); // Also needs to reset timer before
        }
        if (button_released()) {
            start_button_released_thread(); // Also needs to reset timer before
        }
    }
    resume_next_thread();
}

void start_button_changed_thread() {
  uint16_t thread_slot = get_button_changed_handler_thread_slot();
  if (thread_slot != -1) {
    restart_event_thread(thread_slot);
  }
}

void start_button_pressed_thread() {
  uint16_t thread_slot = get_button_pressed_handler_thread_slot();
  if (thread_slot != -1) {
    restart_event_thread(thread_slot);
  }
}

void start_button_releaed_thread() {
  uint16_t thread_slot = get_button_released_handler_thread_slot();
  if (thread_slot != -1) {
    restart_event_thread(thread_slot);
  }
}

void restart_event_thread(uint16_t thread_slot) {
  reset_pc(thread_slot); // Want to call the same function again
  turn_off_non_timer_interrupts(); // Don't want to deal with other interrupts during event handling
  resume_thread(thread_slot);
  turn_on_non_timer_interrupts();
}

void resume_thread(uint16_t thread_slot) {
  reset_timer();
  load_thread(thread_slot);
  asm("\tmret");
}
*/
