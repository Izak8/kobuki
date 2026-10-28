#include "adc.h"
#include "cmsis_gcc.h"
#include "fsm.h"
#include "uart.h"
#include "kobuki.h"
#include "ultrasonic.h"

#define BUTTON_0_START		0x1U
#define BUTTON_1_STOP		0x2U
#define BUTTON_2_CALIBRATE	0x4U

#define DIST_CALIBRATE		585U

#define BUMPER_RIGHT        0x01U
#define BUMPER_LEFT         0x04U
#define CLIFF_RIGHT         0x01U
#define CLIFF_LEFT          0x04U
#define BUMPER_AVOID_ANGLE  5

#define OFFSET				50U
#define TOLERANCE			15U

#define DIST_S_TO_A1		585U
#define DIST_S_TO_B1		445U

#define DIST_S_TO_A			1200U
#define DIST_A_TO_E			1400U

#define DIST_S_TO_B			1400U
#define DIST_B_TO_E			1200U

#define DIST_E_TO_D			860U

#define SEARCH_SPEED_MM_S	50U
#define SCAN_SPEED_MM_S		25U

// data structures to hold sensed information
static Kobuki_Typedef kobuki;
static US_Typedef object;
static Kobuki_LED_Enum led1;
static Kobuki_LED_Enum led2;
static FSM_RobotState state = IDLE;
static FSM_StageState lastStage = END;
static FSM_StageState currentStage = END;
// set the initial state to IDLE

#define AVOID_DELAY_COUNT   3000000U

static void AvoidDelay(void) {
    for(volatile uint32_t i = 0U; i < AVOID_DELAY_COUNT; ++i) {
		__NOP();
    }
}

int main(void) {
	

	// initialise the relevant peripherals
	UART_init();
	ADC_init();
	Ultrasonic_init();

	Kobuki_Update_LEDs(OFF, OFF);

	// logic
	while(state != ESTOP) {
		// read from the kobuki UART connection
		Kobuki_Read(&kobuki);

		if(kobuki.button == BUTTON_1_STOP) {
			// stop the robot instantly if stop button pressed
			state = ESTOP;
			continue;
		}

		if(kobuki.button == BUTTON_0_START) {
			state = IDLE;
			lastStage = START;
			currentStage = START;
		}

		if(kobuki.cliff != 0U) {
			if((kobuki.cliff != CLIFF_LEFT && kobuki.cliff != CLIFF_RIGHT) ||
			(state != MOVING && state != AVOIDING)) {
			state = ESTOP;
			continue;
			}	
		}

		if(((kobuki.bumper & BUMPER_LEFT) != 0U ||
			(kobuki.cliff & CLIFF_LEFT) != 0U) && state == MOVING) {
				// left bumper or cliff activated
				Kobuki_Drive(0);
				Kobuki_Rotate(0);
				Kobuki_Rotate_Angle_Setpoint(kobuki.angle, -BUMPER_AVOID_ANGLE);
				state = AVOIDING;
		}

		if(((kobuki.bumper & BUMPER_RIGHT) != 0U ||
			(kobuki.cliff & CLIFF_RIGHT) != 0U) && state == MOVING) {
				// right bumper or cliff activated
				Kobuki_Drive(0);
				Kobuki_Rotate(0);
				Kobuki_Rotate_Angle_Setpoint(kobuki.angle, BUMPER_AVOID_ANGLE);
				state = AVOIDING;
		}

		if(kobuki.button == BUTTON_2_CALIBRATE) {
			// enter the check state
			state = CHECK;
			lastStage = END;
			currentStage = END;
		}

		// read from the ultrasonic sensor ADC
		Ultrasonic_detect(&object);

		Kobuki_Update_LEDs(OFF, OFF);
		// use the sensed input to determine the state transitions
		switch(state) {
			case IDLE:
				Kobuki_Drive(0);
				Kobuki_Rotate(0);
				Kobuki_Drive_Distance_Setpoint(kobuki.distance, 0);
				Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 0);
				break;
				case MOVING:
				if(kobuki.distance_complete == 1) {
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, 0);
					state = IDLE;
				} else {
					Kobuki_Drive_Distance(kobuki.distance);
				}
				break;
			case ROTATING:
				if(kobuki.rotation_complete == 1) {
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 0);
					state = IDLE;
				} else {
					Kobuki_Rotate_Angle(kobuki.angle);
				}
				break;
			case AVOIDING:
				if(kobuki.rotation_complete == 1 &&
					kobuki.bumper == 0U && kobuki.cliff == 0U) {
					Kobuki_Rotate(0);
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 0);
					state = MOVING;
				} else if(kobuki.rotation_complete == 1) {
					Kobuki_Rotate(0);
				} else {
					Kobuki_Rotate_Angle(kobuki.angle);
				}
				break;
			case CHECK:
				if(	object.range_mm >= (DIST_CALIBRATE- OFFSET) - TOLERANCE  &&
					object.range_mm <= (DIST_CALIBRATE- OFFSET) + TOLERANCE) {
					
					Kobuki_Update_LEDs(GREEN, GREEN);
				} else {
					Kobuki_Update_LEDs(GREEN, RED);
				}
				break;
			case ESTOP:
				break;
		}

		if(state == AVOIDING) {
		// pause route progression during bumper avoidance
			continue;
		}

		switch(currentStage) {
			case START:
				if(	object.range_mm >= (DIST_S_TO_A1 - OFFSET) - (TOLERANCE + 15)  &&
					object.range_mm <= (DIST_S_TO_A1 - OFFSET) + (TOLERANCE + 15)) {

					Kobuki_Rotate(SEARCH_SPEED_MM_S);

				} else if(	object.range_mm >= (DIST_S_TO_B1 - OFFSET) - (TOLERANCE + 15)  &&
							object.range_mm <= (DIST_S_TO_B1 - OFFSET) + (TOLERANCE + 15)) {
				
					Kobuki_Rotate(SEARCH_SPEED_MM_S);

				} else {
					Kobuki_Rotate(0);
					currentStage = FIND;
				}
				break;
			case FIND:
				Kobuki_Update_LEDs(RED, GREEN);
				// transition behavior from START -> CALIBRATION
				if(lastStage == START) {
					state = ROTATING;
					lastStage = FIND;
				}

				Kobuki_Rotate(75U);
				
				if(	object.range_mm >= (DIST_S_TO_A1 - OFFSET) - TOLERANCE  &&
					object.range_mm <= (DIST_S_TO_A1 - OFFSET) + TOLERANCE) {

					Kobuki_Rotate(0);
					AvoidDelay();
					currentStage = ALIGN_A1;
				}

				if(	object.range_mm >= (DIST_S_TO_B1- OFFSET)- TOLERANCE  &&
					object.range_mm <= (DIST_S_TO_B1- OFFSET)+ TOLERANCE) {
				
					Kobuki_Rotate(0);
					AvoidDelay();
					currentStage = ALIGN_B1;
				}

				break;

			case ALIGN_A1:
				Kobuki_Update_LEDs(RED, OFF);
				// transition behavior from START -> CALIBRATION
				if(lastStage == FIND) {
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 18);
					state = ROTATING;
					lastStage = ALIGN_A1;
				}

				if(state == IDLE) {
					currentStage = ROTATE_S_TO_A;
				}
				break;

			case ALIGN_B1:
				Kobuki_Update_LEDs(GREEN, OFF);
				// transition behavior from START -> CALIBRATION
				if(lastStage == FIND) {
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 18);
					state = ROTATING;
					lastStage = ALIGN_B1;
				}

				if(state == IDLE) {
					currentStage = ROTATE_S_TO_B;
				}
				break;
			case ROTATE_S_TO_A:
				Kobuki_Update_LEDs(OFF, RED);
				// transition from CALIBRATION -> ROTATE_S_TO_A
				if(lastStage == ALIGN_A1) {
					Kobuki_Rotate(0);
					// We are facing a1 so we rotate CW 90deg to aim at A
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, -85);
					state = ROTATING;
					lastStage = ROTATE_S_TO_A;
				}
				
				// once robot stops, consider stage complete and set the stage
				// to MOVE_S_TO_A
				if(state == IDLE) {
					currentStage = MOVE_S_TO_A;
				}
				break;
			case MOVE_S_TO_A:
				Kobuki_Update_LEDs(RED, RED);
				// transition from ROTATE_S_TO_A -> MOVE_S_TO_A
				if(lastStage == ROTATE_S_TO_A) {
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, DIST_S_TO_A);
					state = MOVING;
					lastStage = MOVE_S_TO_A;
				}

				// once robot stops, consider stage complete and set the stage
				// to ROTATE_A_TO_E
				if(state == IDLE) {
					currentStage = ROTATE_A_TO_E;
				}
				break;

			case ROTATE_A_TO_E:
				Kobuki_Update_LEDs(RED, RED);
				// transition from MOVE_S_TO_A -> ROTATE_A_TO_E
				if(lastStage == MOVE_S_TO_A) {
					// rotate CCW 90 to aim from A to E
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 90);
					state = ROTATING;
					currentStage = ROTATE_A_TO_E;
				}

				// once robot stops, consider stage complete and set the stage
				// to MOVE_A_TO_E
				if(state == IDLE) {
					currentStage = MOVE_A_TO_E;
				}
				break;
			
			case MOVE_A_TO_E:
				Kobuki_Update_LEDs(RED, RED);
				// transition from ROTATE_A_TO_E -> MOVE_A_TO_E
				if(lastStage == ROTATE_A_TO_E) {
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, DIST_A_TO_E);
					state = MOVING;
					lastStage = MOVE_A_TO_E;
				}

				// once robot stops, consider stage complete and set the stage
				// to ROTATE_AE_TO_D
				if(state == IDLE) {
					currentStage = ROTATE_AE_TO_D;
				}
				break;
			
			case ROTATE_AE_TO_D:
				Kobuki_Update_LEDs(RED, RED);
				if(lastStage == MOVE_A_TO_E) {
					// aimed AE therefore rotate CCW 135deg to aim at D
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 135);
					state = ROTATING;
					lastStage = ROTATE_AE_TO_D;
				}

				// once robot stops, consider stage complete and set the stage
				// to MOVE_E_TO_D
				if(state == IDLE) {
					currentStage = MOVE_E_TO_D;
				}
				break;
			
			case ROTATE_S_TO_B:
				Kobuki_Update_LEDs(OFF, GREEN);
				// transition from CALIBRATION -> ROTATE_S_TO_B
				if(lastStage == ALIGN_B1) {
					Kobuki_Rotate(0);
					// We are facing b1 so we rotate CCW 90deg to aim at B
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 95);
					state = ROTATING;
					lastStage = ROTATE_S_TO_B;
				}

				// once robot stops, consider stage complete and set the stage
				// to MOVE_S_TO_B
				if(state == IDLE) {
					currentStage = MOVE_S_TO_B;
				}
				break;

			case MOVE_S_TO_B:
				Kobuki_Update_LEDs(GREEN, GREEN);
				// transition from ROTATE_S_TO_B -> MOVE_S_TO_B
				if(lastStage == ROTATE_S_TO_B) {
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, DIST_S_TO_B);
					state = MOVING;
					lastStage = MOVE_S_TO_B;
				}

				// once robot stops, consider stage complete and set the stage
				// to ROTATE_B_TO_E
				if(state == IDLE) {
					currentStage = ROTATE_B_TO_E;
				}
				break;
			
			case ROTATE_B_TO_E:
				Kobuki_Update_LEDs(GREEN, GREEN);
				// transition from MOVE_S_TO_B -> ROTATE_B_TO_E
				if(lastStage == MOVE_S_TO_B) {
					// facing SB therefore rotate CW 90 to aim at E
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, -90);
					state = ROTATING;
					lastStage = ROTATE_B_TO_E;
				}

				// once robot stops, consider stage complete and set the stage
				// to MOVE_B_TO_E
				if(state == IDLE) {
					currentStage = MOVE_B_TO_E;
				}
				break;

			case MOVE_B_TO_E:
				Kobuki_Update_LEDs(GREEN, GREEN);
				// transition from ROTATE_B_TO_E -> MOVE_B_TO_E
				if(lastStage == ROTATE_B_TO_E) {
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, DIST_B_TO_E);
					state = MOVING;
					lastStage = MOVE_B_TO_E;
				}

				// once robot stops, consider stage complete and set the stage
				// to ROTATE_BE_TO_D
				if(state == IDLE) {
					currentStage = ROTATE_BE_TO_D;
				}
				break;
			
			case ROTATE_BE_TO_D:
				Kobuki_Update_LEDs(GREEN, GREEN);
				// transition from MOVE_B_TO_E -> ROTATE_BE_TO_D
				if(lastStage == MOVE_B_TO_E) {
					// facing BE therefore rotate CW 135deg to aim at D
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, -130);
					state = ROTATING;
					lastStage = ROTATE_BE_TO_D;
				}

				// once robot stops, consider stage complete and set the stage
				// to MOVE_E_TO_D
				if(state == IDLE) {
					currentStage = MOVE_E_TO_D;
				}
				break;

			case MOVE_E_TO_D:
				// transition from either to -> MOVE_E_TO_D
				if(lastStage == ROTATE_AE_TO_D || lastStage == ROTATE_BE_TO_D) {
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, DIST_E_TO_D);
					state = MOVING;
					lastStage = MOVE_E_TO_D;
				}

				// once robot stops, consider stage complete and set the stage
				// to FIND_DEPOSIT
				if(state == IDLE) {
					currentStage = FIND_DEPOSIT;
				}
				break;
			
			case FIND_DEPOSIT:
				Kobuki_Rotate(10);
				break;
			case MOVE_DEPOSIT:
				currentStage = END;
				break;
			case END:
				break;
		}
	}

	// 2 RED is ESTOP
	Kobuki_Drive(0);
	Kobuki_Rotate(0);
	Kobuki_Update_LEDs(RED, RED);
	while(1);
}