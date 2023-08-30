/* Handler for timer_interrupts for the OS.

We will only enable timer interrupts when running in "User" (not actually user) mode.

On a timer interrupt, we will return from a "User" thread and we need to handle several things:
    - Turn off interrupts (I believe happens automatically according to the spec)
    - Save the data from the current user thread into the image for that thread.
    - Check buttons against the saved button state to make sure no-changes in button status.
    - Iterate to the next thread, check to make sure it can run
    - Load next threads image
    - Give control to that thread.
*/
// #include <stdint.h>

// extern volatile int global;
// extern volatile uint32_t controller_status;
extern uint32_t old_controller_value;
extern uint32_t new_controller_value;

void timer_interrupt_handler();
// void external_interrupt_handler();
// void c_interrupt_handler(uint32_t mcause);

/*
void timer_interrupt_handler() {
    // turn_off_interrupts() // Don't need to, mstatus.MIE should get turn off
      if (is_event_thread(current_thread_slot)) { // Event thread didn't complete before next timer interrupt
      printf("Timed out waiting for response from button click.");
      kill_program();
    }
    if (button_changed()) {
        start_button_changed_thread(); // Also needs to reset timer before
        if (button_clicked()) {
            start_button_clicked_thread(); // Also needs to reset timer before
        }
        if (button_released()) {
            start_button_released_thread(); // Also needs to reset timer before
        }
	}
    current_thread_slot = get_next_thread_slot();
    thread_status current_thread = get_thread_status(current_thread);
    resume_thread(current_thread);
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
