//   _____ _____ _____
//  |   | |__   |   | | C_StateMachines/fsm.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 24-05-19
#ifndef FSM_H_
#define FSM_H_

#define EV_FOO 3U
#define EV_BAR 4U

void fsm_run();
void fsm_add_event(uint8_t in);


#endif /* FSM_H_ */