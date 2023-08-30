/* This file defines the logic for scheduling threads.

We will cycle through threads without priority (maybe eventually)

If current thread not waiting, we will startup the thread.
    - Can be waiting on either another thread or for a certain amount of time.
        - Timed waits are created by wait()
	- Thread dependencies are created by join()

Support a maximum of 16 threads
z
Question: Theoretically, an event handler thread doesn't need to save any of its own data (unless globally), and it will finish before any other threads occur, the stack should fully unwind. I think we could reuse a thread for this, but for now won't allow.
*/
#include <stdint.h>

typedef struct {
   int32_t id;
   int32_t thread_dependency; // ID of thread this thread is waiting on, -1 if not waiting
   uint64_t resume_time; // How do we deal with overflow here?
   uint16_t waiting;
   uint32_t* stack_pointer;
   uint16_t event_handler; // If event handler, don't call normally
} thread_status_t;

void initialize_threads();
void timer_interrupts_thread_switch();
uint16_t finish_thread();
uint16_t new_thread(void (*f)());
void setButtonClicked(void (*f)(uint32_t));
void setButtonReleased(void (*f)(uint32_t));
void setButtonChanged(void (*f)(uint32_t));
void setCmdPressed(void (*f)());
void start_thread_rotation();
uint16_t resume_next_thread();
void save_previous_sp();
void wait(int time_in_ms);
uint16_t kill();
void join (int thread_to_join);
void yield();
uint16_t syscall_new_thread(void (*f)());
void cmdButtonPressed();
