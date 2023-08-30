#include <stdint.h>
#include "mutex.h"

uint16_t pthread_mutex_lock(mutex_t *p_mutex){
    if(*p_mutex==0){
      *p_mutex = 1;
      return 1;
    }else {  //Mutex is already locked and is in use by another thread or some error 
      return 0;
    }
}

uint16_t pthread_mutex_unlock(mutex_t *p_mutex){
  if (*p_mutex == 0 || *p_mutex == 1) {
    *p_mutex = 0;
    return 1;
  } else {//Some error condition
    return 0;
  }
}


