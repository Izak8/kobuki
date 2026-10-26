#include "adc.h"
#include "fsm.h"
#include "uart.h"
#include "kobuki.h"
#include "ultrasonic.h"

#define BUTTON_0_START		0x1U
#define BUTTON_1_STOP		0x2U
#define BUTTON_2_ROTATE		0x4U

#define LINEAR_DISTANCE_MM	750U
#define KNOWN_DISTANCE1_MM	550U
#define KNOWN_DISTANCE2_MM	445U
#define DEPOSIT_BOLLARD1_MM 
#define DEPOSIT_BOLLARD2_MM 

#define SEARCH_SPEED_MM_S	50U
#define SCAN_SPEED_MM_S		25U
#define STOP_SHORT_MM       100U

Kobuki_Typedef kobuki;
US_Typedef object;
uint16_t scan_angle0 = 0;
uint16_t scan_angle1 = 0;
uint16_t scan_dist0 = 0;
uint16_t scan_dist1 = 0;
static uint16_t target_dist_mm=0;
static uint8_t target_bollard=0;

int main(void) {

	// data structures to hold sensed information
	Kobuki_LED_Enum led1;
	Kobuki_LED_Enum led2;
	uint8_t move_complete = 0;

	// initialise the relevant peripherals
	UART_init();
	ADC_init();
	Ultrasonic_init();

	// set the initial state to IDLE
	FSM_RobotState state = IDLE;

	uint8_t iteration = 0;

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
		if(/* object.detected == 1 ||  */kobuki.bumper != 0 || kobuki.cliff != 0) {
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
				} else if (kobuki.rotation_complete == 1) {
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 0);

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
				if(	(object.range_mm >= (KNOWN_DISTANCE1_MM - 50) - 30  &&
					 object.range_mm <= (KNOWN_DISTANCE1_MM - 50) + 30)

					||

					(object.range_mm >= (KNOWN_DISTANCE2_MM - 50) - 30  &&
					 object.range_mm <= (KNOWN_DISTANCE2_MM - 50) + 30)
					){

					// SEARCHING -> SCANNING
					// set the initial angle
					scan_angle0 = DEG_PER_TICK * kobuki.angle;
					scan_dist0 = object.range_mm;
					// once known distance is detected rotate an additional 16 degrees
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 18);
					state = SCANNING;
				} else {
					// TODO: calibrate this speed
					Kobuki_Rotate(SEARCH_SPEED_MM_S);
				}
				break;

			case SCANNING:
				Kobuki_Update_LEDs(OFF, GREEN);
				if(kobuki.rotation_complete == 1) {
					state=IDLE;
				} else {
					Kobuki_Rotate_Angle(kobuki.angle);
				}
				break;
			case ALIGNING:
				//Kobuki_Rotate_Angle_Setpoint(kobuki.angle, -)
				break;
			case DEPOSITSCAN:
				Kobuki_Update_LEDs(GREEN,RED);
				bool hit1=(object.range_mm >= (DEPOSIT_BOLLARD1_MM-50) - 30  &&
					 object.range_mm <= (DEPOSIT_BOLLARD1_MM+50) + 30);
					
				bool hit2=(object.range_mm >= (DEPOSIT_BOLLARD2_MM-50) - 30  &&
					 object.range_mm <= (DEPOSIT_BOLLARD2_MM+50) + 30);
				
				if(hit1||hit2){
					if(hit1){
						target_bollard=1;
						target_dist_mm=DEPOSIT_BOLLARD1_MM;
					}else{
						target_bollard=2;
						target_dist_mm=DEPOSIT_BOLLARD2_MM;
					}
					scan_angle0 = DEG_PER_TICK * kobuki.angle;
					scan_dist0 = object.range_mm;
					Kobuki_Rotate_Angle_Setpoint(kobuki.angle, 18);
					state = DEPOSIT;
				} else {
					Kobuki_Rotate(SEARCH_SPEED_MM_S);
				}
				break;
			case DEPOSIT;
				Kobuki_Update_LEDs(GREEN,GREEN);
				Kobuki_Drive_Distance_Setpoint(target_dist_mm - STOP_SHORT_MM);
			break;

		}
			
		}
	}

	// 2 RED is ESTOP
	Kobuki_Drive(0);
	Kobuki_Rotate(0);
	Kobuki_Update_LEDs(RED, RED);
	while(1);
}