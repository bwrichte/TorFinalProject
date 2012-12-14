#include <pthread.h>
#include "monitor.h"

void Monitor_Init(Monitor *monitor) {
    CircBuffer_Init(&monitor->cb);
    pthread_cond_init(&monitor->full, NULL);
    pthread_cond_init(&monitor->empty, NULL);
    pthread_mutex_init(&monitor->mutex, NULL);
}

int Monitor_AddBuffer(Monitor *mon, char *buf, int count) {
    pthread_mutex_lock(&mon->mutex);
    while (CircBuffer_Remaining(&mon->cb) < count) {
        pthread_cond_wait(&mon->full, &mon->mutex);
    }
   
    CircBuffer_AddAll(&mon->cb, buf, count);
    pthread_cond_broadcast(&mon->empty);
    pthread_mutex_unlock(&mon->mutex);
    return 0;
}

int Monitor_RemoveBuffer(Monitor *mon, char *buf, int want)
{    
    pthread_mutex_lock(&mon->mutex);
    
    while(CircBuffer_Size(&mon->cb) <= 0 && !CircBuffer_IsClosed(&mon->cb)) {
        pthread_cond_wait(&mon->empty, &mon->mutex);
    }
   
    int count = CircBuffer_RemoveAll(&mon->cb, buf, want);
    
    pthread_cond_broadcast(&mon->full);
    pthread_mutex_unlock(&mon->mutex);
    return count;
}

void Monitor_CloseBuffer(Monitor *mon)
{
	CircBuffer_Close(&mon->cb);
}

