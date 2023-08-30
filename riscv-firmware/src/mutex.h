#include <stdint.h>

typedef uint16_t mutex_t;

uint16_t pthread_mutex_lock(mutex_t *p_mutex);

uint16_t pthread_mutex_unlock(mutex_t *p_mutex);

