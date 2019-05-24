# C State Machines
This repository contains C implementations of both a finite state machine (FSM) and a hierarchical state machine (HSM) intended for event driven embedded systems.

The state machine implementations are largely based on the article [Introduction to Hierarchical State Machines](https://barrgroup.com/Embedded-Systems/How-To/Introduction-Hierarchical-State-Machines) by Miro Samek at the BARR Group, while the queue implementation is based on the Queue type described in [Introduction to Algorithms](https://mitpress.mit.edu/books/introduction-algorithms-third-edition) by Cormen et. al.

Note that the implementations within this repository has been extracted and refined from a larger application. Thus, it has not been compiled in the state presented here and there might be syntax errors. The code within this repository is thus to be considered as pseudo code.

**However posting an issue or a pull request is encuraged.**

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

## The State Machines:
The FSM and the HSM are functionally very similar and only differs in how events are dispatched and how the function pointer which represent the states (`state_fp`) is defined.

The FSM `state_fp` has no return value and events are only dispatched to the current state:
```C
static void fsm_dispatch(fsm_t *fsm, uint8_t event){
    (*(fsm)->state)(fsm, event);
}
```
However, the HSM `state_fp` is expected to return a pointer that points to
* `NULL` if the state is not a sub-state.
* `NULL` if the state is a sub-state and the event has been handled.
* The `state_fp` of the sub-state's super-state if the event was not handled.

Thus, when dispatching events to a sub-state that does not handle the event, the event will be handled by it's super-state:
```C
static void hsm_dispatch(hsm_t *hsm_p, u8 ev){
    hsm_p->super = hsm_p->state;
    do{
        hsm_p->super = (state_fp)(*(hsm_p)->super)(hsm_p, ev);
    }while(hsm_p->super != NULL);
}
```
Note that this will cause a super-state's exit actions to trigger on a transition to one of it's sub-states.

## Notes:
### Handling events in both super and sub-states:
By letting a sub-state return the `state_fp` of it's super-state even if an event was handled, the event will also be handled by it's super-state.

### Increasing Performance:
To increase performance, the `static inline` attribute is suitable for several functions within this implementation, although it is likely that a compiler would inline by default due to the compact size of the functions.

### Compliance with state machine semantics:
Both state machine implementation allows events to be handled completely outside any state. Consider the HSM's super-loop:
```C
void hsm_run(){
    hsm_t   hsm_i; /* hsm instance.                     */
    uint8_t event; /* Buffer for storing incoming event */

    hsm_init(&hsm_i, hsm_state_init);

    while(1){
        dequeue(&event_queue, &event);
        /*
         * Some events can be handled independently of state,
         * while others are dispatched to the current state
         * in the default condition.
         */
        switch(event){
        case EV_FOO:
            foo();
            break;
        case EV_BAR:
            bar();
            break;
        default:
            hsm_dispatch(&hsm_i, event);
            break;
        }
    }
}
```
Here, the events `EV_FOO` and `EV_BAR` are handled by the functions `foo()` and `bas()` and not dispatched to the sate machine at all.

Technically, this breaks the semantics of state machines and the semantically correct method would be to provide a top-level super-state which handles these "generic" events, and making every other state a sub-state of the top-level state.

However, this "pre-handling" is provided as it is considered unecessary to fully traverse the state hierarchy before handling events that is known in advance to be "generic".