//   _____ _____ _____
//  |   | |__   |   | | C_StateMachines/queue.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 24-05-19
#include "queue.h"

void enqueue(volatile queue_t *queue, uint8_t in){
    ENTER_CRITICAL();
    queue->fifo[queue->tail++] = in;
    LEAVE_CRITICAL();
}

void dequeue(volatile queue_t *queue, uint8_t *out){
    while(queue->head == queue->tail){
        STANDBY();
    }

    ENTER_CRITICAL();
    *out = queue->fifo[queue->head++];
    LEAVE_CRITICAL();
}
