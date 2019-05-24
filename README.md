# C State Machines
This repository contains C implementations of both a finite state machine (FSM) and a hierarchical state machine (HSM) intended for event driven embedded systems.

The state machine implementations are largely based on the article [Introduction to Hierarchical State Machines](https://barrgroup.com/Embedded-Systems/How-To/Introduction-Hierarchical-State-Machines) by Miro Samek at the BARR Group, while the queue implementation is based on the Queue type described in [Introduction to Algorithms](https://mitpress.mit.edu/books/introduction-algorithms-third-edition) by Cormen et. al.

## Usage:
The state machines are designed to work with interrupts, where the interrupt service routines enqueue events to the state machines event queue:
```C
/* Example main.c */
#include "fsm.h"

void interrupt_service_routine(){
    fsm_add_event(EV_FOO);
}

void main(){
    fsm_run(); /* Blocking Call */
}
```
However, events can be queued outside of interrupts by the application, as well as within the states of the state machines.

## The Events:
Events are implemented as 8-bit unsigned integers, and defined in the respective header files for each state machine.

Both state machine has two "private" events, `EV_ENTRY` and `EV_EXIT` which is only dispatched by the state machine it self and not availiable outside the state machine modules.

If more than 256 unique events are required, the data type of the events can be changed. However, note that the size of the queue fifo will increase accordingly.

## The Queue:
The queue implementation is minimal by design and provides a 256 event FIFO such that the sequence of events occuring is maintained.

The length of the FIFO is fixed to 256 such that the 8-bit unsigned integers `head` and `tail` are allowed to overflow and wrap around to 0 as defined by the C99 standard, avoiding the need for overflow checks in the `dequeue` and `enqueue` functions.

Thus, the queue implementation does not provide any mechanism which protects against overflowing the queue. It is recommended that such a mechanism is added for systems which can experience a burst of events or systems with limited computational performance.

The queue module uses functions for entering a standby mode and entering/leaving critical sections where interrupts are disabled. Although the implementation of these are hardware specific, examples for ARM based systems are provided below:
```C
void STANDBY(){
    asm("ISB"); /* Flush pipeline      */
    asm("WFI"); /* Wait for interrupts */
}

void ENTER_CRITICAL(){
    asm("CPSID I"); /* Disable interrupts */
}

void LEAVE_CRITICAL(){
    asm("CPSIE I"); /* Enable interrupts */
    asm("ISB");     /* Flush pipeline    */
}
```
