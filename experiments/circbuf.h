#ifndef CIRCBUF_H_
#define CIRCBUF_H_

#define BUFFER_SIZE 4194304

typedef struct CircBuffer {
    char buf[BUFFER_SIZE];
    int start;
    int end;
    int size;
    int eof;
} CircBuffer;

void CircBuffer_Init(CircBuffer *buffer);

int CircBuffer_Add(CircBuffer *buffer, char c);

int CircBuffer_Remove(CircBuffer *buffer, char *ret);

int CircBuffer_Size(CircBuffer *buffer);

int CircBuffer_Remaining(CircBuffer *buffer);

int CircBuffer_AddAll(CircBuffer *buffer, char *tmp, int count);

int CircBuffer_RemoveAll(CircBuffer *buffer, char *tmp, int count);

void CircBuffer_Close(CircBuffer *buffer);

int CircBuffer_IsClosed(CircBuffer *buffer);

#endif
