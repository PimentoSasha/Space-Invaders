#include <stdint.h>
#include "thread_scheduler.h"

#define max_threads 16
#define UPPER_RAM_LIMIT 0x71000000
#define THREAD_STACK_SIZE 0x00010000

#define MTIME_LOW          (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH         (*((volatile uint32_t *)0x4000000C))

extern void start_threads();
extern void load_thread_image();

extern uint32_t *_thread_status_sp_value;
extern uint32_t old_controller_value;
extern uint32_t new_controller_value;
extern void reset_timer();
extern void end_thread();

int32_t event_handler_status;
thread_status_t threads_status [max_threads];
int32_t next_thread_id; // ID, NOT thread_slot
uint16_t current_thread_slot;
uint16_t saved_thread_slot;
extern uint32_t cart_global_pointer;
extern uint32_t cart_gp_saved;

struct event_threads_t {
    int32_t button_clicked;
    int32_t button_released;
    int32_t button_changed;
    int32_t cmd_pressed;
} event_threads;

struct event_threads_entry_t {
  void (*button_clicked)(uint32_t);
  void (*button_released)(uint32_t);
  void (*button_changed)(uint32_t);
  void (*cmd_pressed)();
} event_functions;

__attribute__((always_inline)) inline void csr_enable_timer_interrupts(void){
  asm volatile ("li t0, 0x80\n\tcsrs mie, t0");
}

__attribute__((always_inline)) inline void csr_disable_timer_interrupts(void){
  asm volatile ("li t0, 0x80\n\tcsrc mie, t0");
}

uint32_t* get_stack_base(uint16_t thread_slot) {
  return (uint32_t*) (UPPER_RAM_LIMIT - thread_slot * THREAD_STACK_SIZE);
}

// set stack pointers
void initialize_threads() {
  for (uint16_t i=0; i<max_threads; ++i) {
    threads_status[i].id = -1;
    threads_status[i].stack_pointer = get_stack_base(i);
  }
  next_thread_id = 0;  
  current_thread_slot = 0;
  saved_thread_slot = 0;
  event_threads.button_clicked = -1;
  event_threads.button_released = -1;
  event_threads.button_changed = -1;
  event_threads.cmd_pressed = -1;
}

uint64_t get_time() {
  return (((uint64_t)MTIME_HIGH)<<32) | MTIME_LOW;
}

uint16_t get_next_thread_slot() {
  uint16_t next_slot_to_try = current_thread_slot;
  uint16_t active_threads = 0;
  while (1) {
    next_slot_to_try = (next_slot_to_try + 1) & 0xF; // i.e (++next_slot_to_try) % max_threads
    thread_status_t *thread_to_try = &threads_status[next_slot_to_try];
    if (thread_to_try->id != -1 && !thread_to_try->event_handler) {
      active_threads = 1;
      if (thread_to_try->thread_dependency == -1 &&
	  (thread_to_try->waiting == 0 ||
	   thread_to_try->resume_time < get_time())) {
	thread_to_try->waiting = 0;
	return next_slot_to_try;
      }
    }
    if (next_slot_to_try == current_thread_slot &&
	!active_threads) break;
  }
  return -1; // No active threads
}

void delete_thread(uint16_t thread_slot) {
  // Should be no dependent threads
  threads_status[thread_slot].id = -1;
}

void clearDependencies(uint16_t thread_id) {
  for (int i=0;i<max_threads;++i) {
    if (threads_status[i].thread_dependency == thread_id) {
      threads_status[i].thread_dependency = -1;
    }
  }
}

uint16_t finish_thread() {
  if (threads_status[current_thread_slot].event_handler) {
    current_thread_slot = saved_thread_slot;
    csr_enable_timer_interrupts();
  } else {
    uint16_t thread_id = threads_status[current_thread_slot].id;
    clearDependencies(thread_id);
    delete_thread(current_thread_slot);
  }
  return resume_next_thread();
}

void resume_button_clicked_thread(uint32_t value) {
    /* Reinitialize event thread

    Reset Stack pointer
    Reset MEPC address to the function.
    */
    uint16_t thread_slot = event_threads.button_clicked;
    thread_status_t* thread_info = &threads_status[thread_slot];
    thread_info->stack_pointer = get_stack_base(thread_slot); // For safety
    _thread_status_sp_value = thread_info->stack_pointer;

    uint32_t return_address = (uint32_t) &end_thread;
    thread_info->stack_pointer[1] = return_address;
    thread_info->stack_pointer[0] = (uint32_t) event_functions.button_clicked;
    thread_info->stack_pointer[10] = value;
    thread_info->stack_pointer[3] = cart_global_pointer;
}

void resume_button_released_thread(uint32_t value) {
    uint16_t thread_slot = event_threads.button_released;
    thread_status_t* thread_info = &threads_status[thread_slot];
    thread_info->stack_pointer = get_stack_base(thread_slot); // For safety
    _thread_status_sp_value = thread_info->stack_pointer;

    uint32_t return_address = (uint32_t) &end_thread;
    thread_info->stack_pointer[1] = return_address;
    thread_info->stack_pointer[0] = (uint32_t) event_functions.button_released;
    thread_info->stack_pointer[10] = value;
    thread_info->stack_pointer[3] = cart_global_pointer;
}

void resume_button_changed_thread(uint32_t value) {
    uint16_t thread_slot = event_threads.button_changed;
    thread_status_t* thread_info = &threads_status[thread_slot];
    thread_info->stack_pointer = get_stack_base(thread_slot); // For safety
    _thread_status_sp_value = thread_info->stack_pointer;

    uint32_t return_address = (uint32_t) &end_thread;
    thread_info->stack_pointer[1] = return_address;
    thread_info->stack_pointer[0] = (uint32_t) event_functions.button_changed;
    thread_info->stack_pointer[10] = value;
    thread_info->stack_pointer[3] = cart_global_pointer;
}

void resume_cmd_pressed_thread() {
    uint16_t thread_slot = event_threads.cmd_pressed;
    thread_status_t* thread_info = &threads_status[thread_slot];
    thread_info->stack_pointer = get_stack_base(thread_slot); // For safety
    _thread_status_sp_value = thread_info->stack_pointer;

    uint32_t return_address = (uint32_t) &end_thread;
    thread_info->stack_pointer[1] = return_address;
    thread_info->stack_pointer[0] = (uint32_t) event_functions.cmd_pressed;
    thread_info->stack_pointer[3] = cart_global_pointer;
}

uint16_t resume_next_thread() {
  // Check to make sure no event needs handled, then resume the next thread in line
  uint32_t cont_value;
  if (event_handler_status == 0) {
    ++event_handler_status;
    cont_value = ((new_controller_value ^ old_controller_value) & new_controller_value);
    if (event_threads.button_clicked != -1 &&
        cont_value) {
      saved_thread_slot = current_thread_slot;
      current_thread_slot = event_threads.button_clicked;
      csr_disable_timer_interrupts();
      resume_button_clicked_thread(cont_value);
      return 0;
    }
  }
  if (event_handler_status == 1) {
    ++event_handler_status;
    cont_value = ((new_controller_value ^ old_controller_value) & old_controller_value);
    if (event_threads.button_released != -1 &&
	cont_value) {
      saved_thread_slot = current_thread_slot;
      current_thread_slot = event_threads.button_released;
      csr_disable_timer_interrupts();
      resume_button_released_thread(cont_value);
      return 0;
    }
  }
  if (event_handler_status == 2) {
    event_handler_status = 0;
    if (event_threads.button_changed != -1 &&
	new_controller_value != old_controller_value) {
      saved_thread_slot = current_thread_slot;
      current_thread_slot = event_threads.button_changed;
      csr_disable_timer_interrupts();
      resume_button_changed_thread(new_controller_value);
      return 0;
    }
  }
  /*  if (event_handler_status == 3) {
    event_handler_status = 0;
    if (event_threads.cmd_pressed != -1 &&
	new_controller_value != old_controller_value) {
      saved_thread_slot = current_thread_slot;
      current_thread_slot = event_threads.cmd_pressed;
      csr_disable_timer_interrupts();
      resume_cmd_pressed_thread(new_controller_value);
      return 0;
    }
  }*/
  current_thread_slot = get_next_thread_slot();
  if (current_thread_slot == -1) return 1;
  thread_status_t current_thread = threads_status[current_thread_slot];
  _thread_status_sp_value = current_thread.stack_pointer;
  //  reset_timer();
  return 0;
}

void cmdButtonPressed() {
  saved_thread_slot = current_thread_slot;                                                                                                                                                                                                
  current_thread_slot = event_threads.cmd_pressed;                                                                                                                                                                                        
  csr_disable_timer_interrupts();                                                                                                                                                                                                         
  resume_cmd_pressed_thread(new_controller_value);
}

void save_previous_sp() {
  thread_status_t *thread_info = &threads_status[current_thread_slot];
  thread_info->stack_pointer = _thread_status_sp_value;
}

uint16_t find_next_available_thread_slot() {
  for (uint16_t i=0; i<max_threads;++i) {
    if (threads_status[i].id == -1) {
      return i;
    }
  }
  return -1;
}

// f can take arguments
uint16_t new_thread(void (*f)()) {
    /* Create a new thread, initialize stack and PC.

    Currently, the stack area's will be designated statically.
    PC will be set to the function that is to be called.
        - Will need to figure out how to preserve argument registers here.

    Will return thread_slot on success, -1 on failure.
    */
    uint16_t thread_slot;
    if (-1 == (thread_slot = find_next_available_thread_slot())) { // Get the next open slot in threads_status
        return -1;
    }
    thread_status_t* thread_info = &threads_status[thread_slot];
    thread_info->id = next_thread_id++;
    thread_info->thread_dependency = -1;
    thread_info->stack_pointer = get_stack_base(thread_slot);

    // Initialize Stack
    thread_info->stack_pointer -= (64 / 4); // Since 64 bytes
    for (int i=0;i<16;++i) {
      thread_info->stack_pointer[i] = 0; 
    }
    uint32_t return_address = (uint32_t) &end_thread;
    thread_info->stack_pointer[1] = return_address;
    thread_info->stack_pointer[0] = (uint32_t) f;
    thread_info->stack_pointer[3] = cart_global_pointer;
    thread_info->waiting = 0;
    thread_info->event_handler = 0;
    return thread_slot;
}

void setButtonClicked(void (*f)(uint32_t)) {
    if (event_threads.button_clicked != -1) {
        delete_thread(event_threads.button_clicked);
    }
    event_threads.button_clicked = new_thread(f);
    event_functions.button_clicked = f;
    threads_status[event_threads.button_clicked].event_handler = 1;
}

void setButtonChanged(void (*f)(uint32_t)) {
    if (event_threads.button_changed != -1) {
        delete_thread(event_threads.button_changed);
    }
    event_threads.button_changed = new_thread(f);
    event_functions.button_changed = f;
    threads_status[event_threads.button_changed].event_handler = 1;
}

void setCmdPressed(void (*f)()) {
    if (event_threads.cmd_pressed != -1) {
        delete_thread(event_threads.cmd_pressed);
    }
    event_threads.cmd_pressed = new_thread(f);
    event_functions.cmd_pressed = f;
    threads_status[event_threads.cmd_pressed].event_handler = 1;
}

void setButtonReleased(void (*f)(uint32_t)) {
    if (event_threads.button_released != -1) {
        delete_thread(event_threads.button_released);
    }
    event_threads.button_released = new_thread(f);
    event_functions.button_released = f;
    threads_status[event_threads.button_released].event_handler = 1;
}

void start_thread_rotation() {
  _thread_status_sp_value = threads_status[0].stack_pointer;
  start_threads(); // Set MPIE to 1
  return load_thread_image();
}

void join (int thread_to_join) {
  threads_status[current_thread_slot].thread_dependency = thread_to_join;
  save_previous_sp();
  resume_next_thread();
}

void yield() {
  save_previous_sp();
  resume_next_thread();
}

#define MTIME_LOW       (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH      (*((volatile uint32_t *)0x4000000C))
#define MTIME_PERIOD      (*((volatile uint32_t *)0x40000040))

void wait(int time_in_ms) {
  uint64_t current_time = get_time();
  threads_status[current_thread_slot].waiting = 1;
  threads_status[current_thread_slot].resume_time = (current_time + (time_in_ms * 1000) / MTIME_PERIOD);
  save_previous_sp();
  resume_next_thread();
}

uint16_t kill(uint32_t tid) {
  uint16_t i;
  for (;i<max_threads;++i) {
    if (threads_status[i].id == tid) {
      break;
    }
  }
  clearDependencies(tid);
  delete_thread(i);
  return resume_next_thread();
}

uint16_t syscall_new_thread(void (*f)()) {
  return threads_status[new_thread(f)].id;
}
