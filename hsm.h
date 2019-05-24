//   _____ _____ _____
//  |   | |__   |   | | C_StateMachines/hsm.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 24-05-19
#ifndef HSM_H_
#define HSM_H_

#define EV_FOO 3U
#define EV_BAR 4U

void hsm_run();
void hsm_add_event(uint8_t in);

#endif /* HSM_H_ */