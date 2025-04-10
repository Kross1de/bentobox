#include <kernel/fifo.h>
#include <kernel/malloc.h>

void fifo_init(struct fifo *fifo, int size) {
    fifo->data = kmalloc(sizeof(int) * size);
    fifo->size = size;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->count = 0;
}

int fifo_is_full(struct fifo *fifo) {
    return fifo->count == fifo->size;
}

int fifo_is_empty(struct fifo *fifo) {
    return fifo->count == 0;
}

int fifo_enqueue(struct fifo *fifo, int value) {
    if (fifo_is_full(fifo)) return false;

    fifo->data[fifo->tail] = value;
    fifo->tail = (fifo->tail + 1) % fifo->size;
    fifo->count++;
    return true;
}

int fifo_dequeue(struct fifo *fifo, int *value) {
    if (fifo_is_empty(fifo)) return false;

    *value = fifo->data[fifo->head];
    fifo->head = (fifo->head + 1) % fifo->size;
    fifo->count--;
    return true;
}