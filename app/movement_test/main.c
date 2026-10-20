#include "adc.h"
#include "fsm.h"
#include "uart.h"
#include "kobuki.h"
#include "ultrasonic.h"

#define BUTTON_0_START		0x1U
#define BUTTON_1_STOP		0x2U
#define BUTTON_2_ROTATE		0x4U

#define LINEAR_DISTANCE_MM	300U

int main(void) {

	// data structures to hold sensed information
	Kobuki_Typedef kobuki;
	US_Typedef object;
	Kobuki_LED_Enum led1;
	Kobuki_LED_Enum led2;

	// initialise the relevant peripherals
	UART_init();
	ADC_init();
	Ultrasonic_init();

	// set the initial state to IDLE
	FSM_RobotState state = IDLE;

	// logic
	while(1) {
		// read from the kobuki UART connection
		Kobuki_Read(&kobuki);

		if(kobuki.button == BUTTON_1_STOP) {
			// stop the robot instantly if stop button pressed
			break;
		}

		// read from the ultrasonic sensor ADC
		Ultrasonic_detect(&object);

		// stop the robot in case of any obstacles
		if(object.detected == true || kobuki.bumper != 0 || kobuki.cliff != 0) {
			break;
		}

		// use the sensed input to determine the state transitions
		switch(state) {
			case IDLE:
				// 1 GREEN is IDLE
				Kobuki_Update_LEDs(OFF, GREEN);

				if(kobuki.button == BUTTON_0_START) {
					// IDLE -> MOVING from B0 pressed
					// set a path to move known distance
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, LINEAR_DISTANCE_MM);
					state = MOVING;
				} else if(kobuki.distance_complete == 1) {
					// MOVING -> IDLE from distance complete
					// rotate 180deg
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 180);
					state = ROTATING;
				}
				break;
			case MOVING:
				//2 GREEN is MOVING
				Kobuki_Update_LEDs(GREEN, GREEN);

				if(kobuki.distance_complete == 1) {
					state = IDLE;
				} else {
					Kobuki_Drive_Distance(kobuki.distance);
				}
				break;
			case ROTATING:
				// 1 RED, 1 GREEN is ROTATING
				Kobuki_Update_LEDs(RED, GREEN);
				if(kobuki.rotation_complete == 1) {
					state = IDLE;
				} else {
					Kobuki_Rotate_Angle(kobuki.angle);
				}
				break;
		}
	}

	// 2 RED is ESTOP
	Kobuki_Update_LEDs(RED, RED);
	while(1);
}