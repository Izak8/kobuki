#ifndef KOBUKI_H
#define KOBUKI_H

#include <stdint.h>

#define SIZE_FEEDBACK 26

#define BUMPER 4
#define WHEEL_DROP 5
#define CLIFF 6
#define DIST_LSB 7
#define DIST_MSB 8
#define BUTTON 13
#define ANGLE_LSB 24
#define ANGLE_MSB 25

#define SPEED_LIMIT ( int16_t ) 150 // mm / s
#define SEARCH_SPEED (( int16_t ) 75) // mm / s
#define STOP (( int16_t ) 0) // mm / s
#define MIN_SPEED (( int16_t ) 50) // mm / s

typedef struct {
    uint8_t cliff;
    uint8_t bumper;
    uint8_t wheeldrop;
    uint8_t button;
    uint16_t distance;
    int16_t angle;
    uint8_t rotation_complete;
    uint8_t distance_complete;
} Kobuki_Typedef;

typedef enum {
    OFF,
    RED,
    GREEN,
    YELLOW
} Kobuki_LED_Enum;

/*******************************************************************************
 * Basic Kobuki functions (from practical 5 and 6)
 */
uint8_t Kobuki_Rx(	uint8_t *feedback,
                	uint32_t size_feedback);

void Kobuki_Tx(	uint8_t *payload,
            	uint8_t size_payload);

void Kobuki_Read(Kobuki_Typedef *kobuki);

void Kobuki_Drive(int16_t speed);

void Kobuki_Rotate(int16_t speed);

/*******************************************************************************
 * New Kobuki functions (from assignment pdf)
 */

void Kobuki_Rotate_Angle_Setpoint(	int16_t current_angle_tick,
									int16_t rot_angle_deg);

void Kobuki_Rotate_Angle(int16_t current_angle_tick);

void Kobuki_Drive_Distance_Setpoint(uint16_t current_distance_tick,
									int16_t distance);

void Kobuki_Drive_Distance(uint16_t current_distance_tick);

void Kobuki_Update_LEDs(Kobuki_LED_Enum led_1_colour,
						Kobuki_LED_Enum led_2_colour);
					
#endif/*KOBUKI_H*/