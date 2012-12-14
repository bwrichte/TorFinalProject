#ifndef MONITOR_H_
#define MONITOR_H_

#include "circbuf.h"

typedef struct Monitor {
    CircBuffer cb;
    pthread_cond_t full;
    pthread_cond_t empty;
    pthread_mutex_t mutex;
} Monitor;

void Monitor_Init(Monitor *monitor);

int Monitor_AddBuffer(Monitor *mon, char *buf, int count);

int Monitor_RemoveBuffer(Monitor *mon, char *buf, int want);

void Monitor_CloseBuffer(Monitor *mon);

#endif

