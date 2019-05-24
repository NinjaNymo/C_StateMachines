//   _____ _____ _____
//  |   | |__   |   | | C_StateMachines/hsm.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 24-05-19
#include "hsm.h"
#include "queue.h"

/* Private events */
#define EV_ENTRY 1U
#define EV_EXIT  2U

typedef struct hsm_t hsm_t;
typedef void * (*state_fp)(hsm_t *hsm, uint8_t event);
struct hsm_t{
    state_fp state;
    state_fp super;
};

static volatile queue_t event_queue = {.head = 0, .tail = 0};

static void hsm_init(hsm_t *hsm, state_fp init_state);
static void hsm_dispatch(hsm_t *hsm, uint8_t event);
static void hsm_transition(hsm_t *hsm, state_fp new_state);

static void * hsm_state_init(hsm_t *hsm, uint8_t event);
static void * hsm_state_bas(hsm_t *hsm, uint8_t event);
static void * hsm_state_bas_sub(hsm_t *hsm, uint8_t event);

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

void hsm_add_event(uint8_t in){
    enqueue(&event_queue, in);
}

static void * hsm_state_init(hsm_t *hsm, uint8_t event){
    switch(event){
    case EV_ENTRY:
        /* State entry actions */
        return NULL;
    case EV_BAS:
        hsm_transition(hsm, hsm_state_bas);
        return NULL;
    case EV_EXIT:
        /* State exit actions  */ 
        return NULL;
    default:
        return NULL;
    }
}

static void * hsm_state_bas(hsm_t *hsm, uint8_t event){
    switch(event){
    case EV_ENTRY:
        /* State entry actions */
        hsm_transition(hsm, hsm_state_bas_sub);
        return NULL;
    case EV_EXIT:
        /* State exit actions  */ 
        return NULL;
    default:
        return NULL;
    }
}

static void * hsm_state_bas_sub(hsm_t *hsm, uint8_t event){
    switch(event){
    case EV_ENTRY:
        /* State entry actions */
        return NULL;
    case EV_EXIT:
        /* State exit actions  */
        return NULL;
    default:
        return hsm_state_bas;
    }
}

static void hsm_init(hsm_t *hsm, state_fp init_state){
    hsm->state = init_state;
    hsm_dispatch(hsm, EV_ENTRY);
}

static void hsm_dispatch(hsm_t *hsm_p, u8 ev){
    hsm_p->super = hsm_p->state;
    do{
        hsm_p->super = (state_fp)(*(hsm_p)->super)(hsm_p, ev);
    }while(hsm_p->super != NULL);
}

static void hsm_transition(hsm_t *hsm, state_fp new_state){
    hsm_dispatch(hsm, EV_EXIT);
    hsm->state = new_state;
    hsm_dispatch(hsm, EV_ENTRY);
}