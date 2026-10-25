#ifndef FSM_H
#define FSM_H

/* Basic operational state of the Kobuki robot */
typedef enum {
	IDLE,
	MOVING,
	ROTATING,
	AVOIDING, // for now, just avoid obstacles by rotating away from them
	CHECK,
	ESTOP,
} FSM_RobotState;

typedef enum {
	START,

	FIND,
	ALIGN_A1,
	ALIGN_B1,

	ROTATE_S_TO_A,
	MOVE_S_TO_A,

	ROTATE_S_TO_B,
	MOVE_S_TO_B,

	ROTATE_A_TO_E,
	MOVE_A_TO_E,
	ROTATE_AE_TO_D,

	ROTATE_B_TO_E,
	MOVE_B_TO_E,
	ROTATE_BE_TO_D,

	MOVE_E_TO_D,
	FIND_DEPOSIT,
	MOVE_DEPOSIT,

	END

} FSM_StageState;

#endif/*FSM_H*/