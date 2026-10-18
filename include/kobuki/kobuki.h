#ifndef KOBUKI_H
#define KOBUKI_H
#define SIZE_FEEDBACK 26
#define BUMPER 4
#define WHEEL_DROP 5
#define CLIFF 6
#define DIST_LSB 7
#define DIST_MSB 8
#define BUTTON 13
#define ANGLE_LSB 24
#define ANGLE_MSB 25
#define SPEED_LIMIT (int16_t)150
#define SEARCH_SPEED((int16_t)75)
#define STOP ((int16_t)0)
#define MIN_SPEED ((int16_t)50)
#define PI (3.14159)
#define WHEEL_RADIUS (35.0)
#define REVOLUTIONS_PER_TICK (132.0/6545.0)/52.0
#define MM_PER_TICK (2.0*PI*WHEEL_RADIUS*REVOLUTIONS_PER_TICK)
#define DEG_PER_TICK (1.0/100.0)
#define GAIN_ROTATE (1.0/0.90)
#define GAIN_DRIVE (100.0/100.0)

#define sign(x) ((int8_t)((x)>=0 ? 1:-1))



#include <stdint.h>


// =========================================================
// KOBUKI SENSOR STRUCTURE
// =========================================================

typedef struct
{
    uint8_t cliff;
    uint8_t bumper;
    uint8_t wheeldrop;
    uint8_t button;
    uint16_t distance;
    int16_t angle;
    uint8_t rotation_complete;
    uint8_t distance_complete;

} Kobuki_Typedef;
typedef enum uint8_t {OFF, RED, GREEN, YELLOW} Kobuki_LED_Enum;



// =========================================================
// RECEIVE BUFFER
// =========================================================

// We only need the first 14 bytes of the Kobuki payload
// to reach the button sensor byte.


// =========================================================
// BASIC SENSOR DATA BUFFER POSITIONS
// =========================================================

// Kobuki Basic Sensor Data payload:
//
// [0]  Feedback identifier = 0x01
// [1]  Data length         = 0x0F
// [2]  Timestamp LSB
// [3]  Timestamp MSB
// [4]  Bumper
// [5]  Wheel Drop
// [6]  Cliff
// [7]  Left Encoder LSB
// [8]  Left Encoder MSB
// [9]  Right Encoder LSB
// [10] Right Encoder MSB
// [11] Left PWM
// [12] Right PWM
// [13] Button

//#define BUMPER      0x4
//#define WHEEL_DROP  0x5
//#define CLIFF       0x6
//#define BUTTON      0xD


// =========================================================
// KOBUKI SPEED LIMIT
// =========================================================

// Maximum speed used in this practical: 150 mm/s
//#define SPEED_LIMIT ((int16_t)150)


// =========================================================
// KOBUKI FUNCTION PROTOTYPES
// =========================================================

// Receive Kobuki payload
//uint8_t Kobuki_Rx(uint8_t *feedback,
                  uint32_t size_feedback);

// Transmit Kobuki payload
//void Kobuki_Tx(uint8_t *payload,
               uint8_t size_payload);

// Read Kobuki sensors
//void Kobuki_Read(Kobuki_Typedef *kobuki);

// Drive forward/backward
// Positive = forward
// Negative = backward
//void Kobuki_Drive(int16_t speed);

// Rotate
// Positive = counter-clockwise
// Negative = clockwise
//void Kobuki_Rotate(int16_t speed);

void Kobuki_Rotate_Angle_Setpoint(int16_t current_angle_tick, int16_t rot_angle_deg);
void Kobuki_Rotate_Angle(int16_t current_angle_tick);
void Kobuki_Drive_Distance_Setpoint(uint16_t current_distance_tick, int16_t distance);
void Kobuki_Drive_Distance(uint16_t current_distance_tick);

void Kobuki_Update_LEDs(Kobuki_LED_Enum led_1_colour, Kobuki_LED_Enum led_2_colour);

#endif