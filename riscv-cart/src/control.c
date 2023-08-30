#include "control.h"

int pthread_mutex_init(mutex_t *mutex) {
  *mutex = 0;
  return 1;
}

int pthread_mutex_destroy(mutex_t *mutex) {
  return 1;
}
