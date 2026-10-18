#ifndef KOBUKI_H
#define KOBUKI_H

#include <stdint.h>

/* Kobuki Sensors */
typedef struct
{
    uint8_t cliff;
    uint8_t bumper;
    uint8_t wheeldrop;
    uint8_t button;

} Kobuki_Typedef;


// =========================================================
// RECEIVE BUFFER
// =========================================================

// We only need the first 14 bytes of the Kobuki payload
// to reach the button sensor byte.
#define SIZE_FEEDBACK 14


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

#define BUMPER      0x4
#define WHEEL_DROP  0x5
#define CLIFF       0x6
#define BUTTON      0xD


// =========================================================
// KOBUKI SPEED LIMIT
// =========================================================

// Maximum speed used in this practical: 150 mm/s
#define SPEED_LIMIT ((int16_t)150)


// =========================================================
// KOBUKI FUNCTION PROTOTYPES
// =========================================================

// Receive Kobuki payload
uint8_t Kobuki_Rx(uint8_t *feedback,
                  uint32_t size_feedback);

// Transmit Kobuki payload
void Kobuki_Tx(uint8_t *payload,
               uint8_t size_payload);

// Read Kobuki sensors
void Kobuki_Read(Kobuki_Typedef *kobuki);

// Drive forward/backward
// Positive = forward
// Negative = backward
void Kobuki_Drive(int16_t speed);

// Rotate
// Positive = counter-clockwise
// Negative = clockwise
void Kobuki_Rotate(int16_t speed);


#endif