#include "adc.h"
#include "fsm.h"
#include "uart.h"
#include "kobuki.h"
#include "ultrasonic.h"

#define BUTTON_0_START		0x1U
#define BUTTON_1_STOP		0x2U
#define BUTTON_2_ROTATE		0x4U

#define LINEAR_DISTANCE_MM	750U
#define SEARCH_SPEED_MM_S	75U
#define SCAN_SPEED_MM_S		25U

Kobuki_Typedef kobuki;

int main(void) {

	// data structures to hold sensed information
	US_Typedef object;
	Kobuki_LED_Enum led1;
	Kobuki_LED_Enum led2;
	uint8_t move_complete =0;

	// initialise the relevant peripherals
	UART_init();
	ADC_init();
	Ultrasonic_init();

	// set the initial state to IDLE
	FSM_RobotState state = IDLE;

	uint8_t iteration = 0;
	uint16_t scan_angle0 = 0;
	uint16_t scan_angle1 = 0;

	Kobuki_Update_LEDs(OFF, OFF);
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
				Kobuki_Drive(0);
				Kobuki_Rotate(0);
				Kobuki_Update_LEDs(OFF, GREEN);
				
				if(kobuki.button == BUTTON_0_START) {
					// IDLE -> MOVING from B0 pressed
					// set a path to move known distance
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, LINEAR_DISTANCE_MM);
					state = MOVING;
				} else if(kobuki.distance_complete == 1) {
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, 0);
					// MOVING -> IDLE from distance complete
					// rotate 180deg
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 135);
					state = ROTATING;
				} else if (kobuki.rotation_complete == 1) {
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 0);
					Kobuki_Drive_Distance_Setpoint(kobuki.distance, LINEAR_DISTANCE_MM);
					state=MOVING;
				} else if(kobuki.button == BUTTON_2_ROTATE) {
					// IDLE -> SEARCHING
					state = SEARCHING;
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

			// SEARCHING and SCANNING logic
			case SEARCHING:
				Kobuki_Update_LEDs(OFF, RED);
				// rotate on spot until known distance is detected
				// this distance is 3000mm but we will slowly bring this 
				// acceptable range in by calibrating it
				if(object.range_mm >= 2900 || object.range_mm <= 3100) {
					// SEARCHING -> SCANNING
					// set the initial angle
					scan_angle0 = kobuki.angle;
					state = SCANNING;
				} else {
					// TODO: calibrate this speed
					Kobuki_Rotate(SEARCH_SPEED_MM_S);
				}
				break;
			case SCANNING:
				Kobuki_Update_LEDs(OFF, GREEN);
				// once known distance is detected, rotate much slower until
				// the distance is NOT detected
				if(object.range_mm <= 2900 || object.range_mm >= 3100) {
					// SCANNING -> FOLLOWING
					// set the final angle
					scan_angle1 = kobuki.angle;
					state = FOLLOWING;
				} else {
					Kobuki_Rotate(SCAN_SPEED_MM_S);
				}
				break;
			case FOLLOWING:
				Kobuki_Update_LEDs(RED, GREEN);
				// TODO: figure out finding average angle
				state = IDLE;
				break;
		}
	}

	// 2 RED is ESTOP
	Kobuki_Drive(0);
	Kobuki_Rotate(0);
	Kobuki_Update_LEDs(RED, RED);
	while(1);
}