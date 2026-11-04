#ifndef FSM_H
#define FSM_H

/* Basic operational state of the Kobuki robot */
typedef enum {
	IDLE,
	MOVING,
	ROTATING,
	SEARCHING,
	SCANNING,
	ALIGNING,
	DEPOSITSCAN,
	DEPOSIT
} FSM_RobotState;

#endif/*FSM_H*/