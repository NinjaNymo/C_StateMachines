//   _____ _____ _____
//  |   | |__   |   | | C_StateMachines/queue.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 24-05-19
#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct{
    uint8_t fifo[256];
    uint8_t head;
    uint8_t tail;
}queue_t;

void enqueue(volatile queue_t *queue, uint8_t in);
void dequeue(volatile queue_t *queue, uint8_t *out);

#endif /* QUEUE_H_ */