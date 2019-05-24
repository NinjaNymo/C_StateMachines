//   _____ _____ _____
//  |   | |__   |   | | C_StateMachines/fsm.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 24-05-19
#include "fsm.h"
#include "queue.h"

/* Private events */
#define EV_ENTRY 1U
#define EV_EXIT  2U

typedef struct fsm_t fsm_t;
typedef void(*state_fp)(fsm_t *fsm, uint8_t event);
struct fsm_t{
    state_fp state;
};

static volatile queue_t event_queue = {.head = 0, .tail = 0};

static void fsm_init(fsm_t *fsm, state_fp init_state);
static void fsm_dispatch(fsm_t *fsm, uint8_t event);
static void fsm_transition(fsm_t *fsm, state_fp new_state);

static void fsm_state_init(fsm_t *fsm, uint8_t event);
static void fsm_state_bas(fsm_t *fsm, uint8_t event);

void fsm_run(){
    fsm_t   fsm_i; /* FSM instance.                     */
    uint8_t event; /* Buffer for storing incoming event */

    fsm_init(&fsm_i, fsm_state_init);

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
            fsm_dispatch(&fsm_i, event);
            break;
        }
    }
}

void fsm_add_event(uint8_t in){
    enqueue(&event_queue, in);
}

static void fsm_state_init(fsm_t *fsm, uint8_t event){
    switch(event){
    case EV_ENTRY:
        /* State entry actions */
        break;
    case EV_BAS:
        fsm_transition(fsm, fsm_state_bas);
        break;
    case EV_EXIT:
        /* State exit actions */ 
        break;
    default:
        break;
    }
}

static void fsm_init(fsm_t *fsm, state_fp init_state){
    fsm->state = init_state;
    fsm_dispatch(fsm, EV_ENTRY);
}

static void fsm_dispatch(fsm_t *fsm, uint8_t event){
    (*(fsm)->state)(fsm, event);
}

static void fsm_transition(fsm_t *fsm, state_fp new_state){
    fsm_dispatch(fsm, EV_EXIT);
    fsm->state = new_state;
    fsm_dispatch(fsm, EV_ENTRY);
}
