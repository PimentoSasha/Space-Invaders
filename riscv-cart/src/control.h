#include <stdint.h>

typedef uint16_t mutex_t;

uint32_t getTicks(void);

uint32_t getControllerStatus(void);

uint32_t getCmdBtnStatus(void);

int thread(void (*f)() ); // WHAT IF ARGUMENT


void join(int thread_id);

void kill(int thread_id);

void wait(int time_in_ms);

void yield();

int pthread_mutex_init(mutex_t *mutex);

int pthread_mutex_lock(mutex_t *mutex);

int pthread_mutex_unlock(mutex_t *mutex);

int pthread_mutex_destroy(mutex_t *mutex);

int setOnClickListener(void (*f)(uint32_t) );

int setOnReleasedListener(void (*f)(int) );

int setOnChangedListener(void (*f)(int) );

int setCmdListener(void (*f)() );

/**
 * Get a pseudorandom integer value. This uses the standard library's rand() under the hood.
 * 
 * @return a pseudorandom integer value
*/
uint32_t rand(void);

/**
 * Seed the random number generator with the specified value. This uses the standard library's srand() under the hood.
 * 
 * @param seed an integer value to 'seed' the random number generator with.
*/
void srand(uint32_t seed);
