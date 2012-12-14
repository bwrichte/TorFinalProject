#include "circbuf.h"

#define min(a,b) ((a < b) ? a : b)

#define TRUE 1
#define FALSE 0

void CircBuffer_Init(CircBuffer *buffer)
{
    buffer->start = 0;
    buffer->end = 0;
    buffer->size = 0;
    buffer->eof = FALSE;
}

int CircBuffer_Add(CircBuffer *buffer, char c)
{
    if (buffer->size >= BUFFER_SIZE)
        return -1;
    
    buffer->buf[buffer->end] = c;
    buffer->end = (buffer->end + 1) % BUFFER_SIZE;
    buffer->size++;
    return 0;
}

int CircBuffer_Remove(CircBuffer *buffer, char *ret)
{
    if (buffer->size <= 0)
        return -1;
        
    *ret = buffer->buf[buffer->start];
    buffer->start = (buffer->start + 1) % BUFFER_SIZE;
    buffer->size--;
    return 0;
}

int CircBuffer_Size(CircBuffer *buffer)
{
    return buffer->size;
}

int CircBuffer_Remaining(CircBuffer *buffer)
{
	return BUFFER_SIZE - buffer->size;
}

int CircBuffer_AddAll(CircBuffer *buffer, char *tmp, int count)
{
    if ((BUFFER_SIZE - buffer->size) < count)
        return -1;
    
    int i;
    for (i = 0; i < count; i++) {
        buffer->buf[buffer->end] = tmp[i];
        buffer->end = (buffer->end + 1) % BUFFER_SIZE;
        buffer->size++;
    }
    return 0;
}

int CircBuffer_RemoveAll(CircBuffer *buffer, char *tmp, int count)
{
    if (buffer->size <= 0 && buffer->eof)
        return 0;
    
    int i;
    int len = min(count, buffer->size);
    for (i = 0; i < len; i++) {
        tmp[i] = buffer->buf[buffer->start];
        buffer->start = (buffer->start + 1) % BUFFER_SIZE;
        buffer->size--;
    }
    
    return len;
}

void CircBuffer_Close(CircBuffer *buffer)
{
	buffer->eof = TRUE;
}

int CircBuffer_IsClosed(CircBuffer *buffer)
{
	return buffer->eof;
}

