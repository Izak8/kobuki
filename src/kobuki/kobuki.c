#include <stdint.h>

#define STM32G474xx
#include "stm32g4xx.h"
#include <stdlib.h>
#include "uart.h"
#include "kobuki.h"

static volatile int32_t setpoint_distance_tick;
static volatile int16_t setpoint_angle_deg;
static uint8_t rotation_complete = 0;
static uint8_t distance_complete = 0;

// =========================================================
// KOBUKI TRANSMIT
// =========================================================

void Kobuki_Tx(uint8_t *payload, uint8_t size_payload)
{
    uint8_t checksum = 0;
    uint8_t i;

    // Send fixed Kobuki packet headers
    UART_Tx(0xAA);
    UART_Tx(0x55);

    // Send payload size
    UART_Tx(size_payload);

    // Start running checksum
    checksum ^= size_payload;

    // Send each payload byte
    for (i = 0; i < size_payload; i++)
    {
        UART_Tx(payload[i]);

        // XOR each payload byte into checksum
        checksum ^= payload[i];
    }

    // Send final checksum
    UART_Tx(checksum);
}


// =========================================================
// KOBUKI RECEIVE
// =========================================================

uint8_t Kobuki_Rx(uint8_t *feedback, uint32_t size_feedback)
{
    uint8_t feedback_header = 0;
    uint8_t size_payload = 0;
    uint8_t checksum = 0;
    uint8_t rx_checksum = 0;

    uint32_t i;


    // -----------------------------------------------------
    // 1. FIND VALID 0xAA 0x55 HEADER SEQUENCE
    // -----------------------------------------------------

    while (1)
    {
        feedback_header = UART_Rx();

        if (feedback_header == 0xAA)
        {
            feedback_header = UART_Rx();

            if (feedback_header == 0x55)
            {
                // Valid packet beginning found
                break;
            }
        }
    }


    // -----------------------------------------------------
    // 2. READ PAYLOAD LENGTH
    // -----------------------------------------------------

    size_payload = UART_Rx();

    checksum ^= size_payload;


    // -----------------------------------------------------
    // 3. DETERMINE SAFE BUFFER BOUNDARY
    // -----------------------------------------------------

    uint32_t limit =
        (size_payload < size_feedback) ? size_payload : size_feedback;


    // -----------------------------------------------------
    // 4. READ PAYLOAD INTO FEEDBACK BUFFER
    // -----------------------------------------------------

    for (i = 0; i < limit; i++)
    {
        feedback[i] = UART_Rx();

        checksum ^= feedback[i];
    }


    // -----------------------------------------------------
    // 5. READ REMAINING PAYLOAD BYTES
    //    We do not store them, but they must still be
    //    included in the checksum calculation.
    // -----------------------------------------------------

    for (i = limit; i < size_payload; i++)
    {
        checksum ^= UART_Rx();
    }


    // -----------------------------------------------------
    // 6. VERIFY CHECKSUM
    // -----------------------------------------------------

    rx_checksum = UART_Rx();

    // 0 = success
    // 1 = checksum error
    return (checksum != rx_checksum);
}


// =========================================================
// READ KOBUKI SENSOR DATA
// =========================================================

void Kobuki_Read(Kobuki_Typedef *kobuki)
{
    uint16_t distance_LSB;
    uint16_t distance_MSB;
    int16_t angle_LSB;
    int16_t angle_MSB;
    uint8_t kobuki_rx_buffer[SIZE_FEEDBACK];
    uint8_t checksum_result;
    // Receive Kobuki bytestream
    checksum_result =
        Kobuki_Rx(kobuki_rx_buffer, SIZE_FEEDBACK);


    // Only update sensor variables if checksum is correct
    if (checksum_result == 0)
    {
    //     kobuki->bumper =
    //         kobuki_rx_buffer[BUMPER];

    //     kobuki->wheeldrop =
    //         kobuki_rx_buffer[WHEEL_DROP];

    //     kobuki->cliff =
    //         kobuki_rx_buffer[CLIFF];

    //     kobuki->button =
    //         kobuki_rx_buffer[BUTTON];
    // }

    // // If checksum fails, retain previous sensor values
    // else
    // {
    //     // Do nothing
    // }
    distance_LSB = kobuki_rx_buffer[DIST_LSB];
    angle_LSB = kobuki_rx_buffer[ANGLE_LSB];
    distance_MSB = (uint16_t)((uint16_t)kobuki_rx_buffer[DIST_MSB]<<8);
    angle_MSB= (int16_t)((int16_t)kobuki_rx_buffer[ANGLE_MSB]<<8);

    kobuki->bumper= kobuki_rx_buffer[BUMPER];
    kobuki->wheeldrop = kobuki_rx_buffer[WHEEL_DROP];
    kobuki->cliff = kobuki_rx_buffer[CLIFF];
    kobuki->button = kobuki_rx_buffer[BUTTON];
    kobuki->distance = distance_LSB | distance_MSB;
    kobuki->angle = angle_LSB | angle_MSB;
    }

    else{}
    kobuki -> rotation_complete = rotation_complete;
    kobuki -> distance_complete = distance_complete;

}
void Kobuki_Rotate_Angle_Setpoint(int16_t current_angle_tick, int16_t rot_angle_deg){
    volatile int32_t init_angle;
    rotation_complete = 0;
    init_angle = (int32_t)((double)(current_angle_tick +(int32_t)18000)*DEG_PER_TICK);
    setpoint_angle_deg = (init_angle = rot_angle_deg = 360) % 360;
}

void Kobuki_Rotate_Angle(int16_t current_angle_tick){
    volatile int16_t speed;
    volatile int16_t error;
    volatile int32_t current_angle_deg;

    current_angle_deg = (int32_t)((double)(current_angle_tick+(int32_t)18000)*DEG_PER_TICK);
    error = (int16_t)(setpoint_angle_deg - current_angle_deg);

    if(abs(error)>180){
        error = -sign(error)*(abs(error)%180);
        if((error<0)&&(current_angle_deg<setpoint_angle_deg)){
            error = -(error+180);
        }
        else if((error>0)77(current_angle_deg>setpoint_angle_deg)){
            error = -(error -180);
        }
    }

    if(abs(error)<=0){
        rotation_complete=1;
    }
    else{
        speed = (sign(error)*MIN_SPEED)+(int16_t)(GAIN_ROTATE*error);
        Kobuki_Rotate(speed);
    }
}

void Kobuki_Drive_Distance_Setpoint(uint16_t current_distance_tick, int16_t distance){
    distance_complete = 0;
    setpoint_distance_tick = current_distance_tick + (int32_t)(distance/MM_PER_TICK);
}

void Kobuki_Drive_Distance(uint16_t current_distance_tick){
    volatile int16_t speed;
    volatile int16_t error;
    volatile uint16_t setpoint_alias;
    error =  (int16_t)((double)(setpoint_distance_tick - current_distance_tick)*MM_PER_TICK);
    setpoint_alias = (uint16_t)((setpoint_distance_tick + 0x10000) % 0x10000);
    if((setpoint_distance_tick>0xFFFF)&&(current_distance_tick<setpoint_alias)||((setpoint_distance_tick
    <0) &&(current_distance_tick>setpoint_alias)))
    {
        error = (int16_t)((double)(setpoint_alias-current_distance_tick)*MM_PER_TICK);
    }
    if(abs(error)<=2){
        distance_complete=1;
    }
    else{
        speed=(sign(error)*MIN_SPEED)+(int16_t)(GAIN_DRIVE*error);
        Kobuki_Drive(speed);
    }

}

#define EXTERN_PWR 0x30 
void Kobuki_Update_LEDs(Kobuki_LED_Enum led_1_colour, Kobuki_LED_Enum led_2_colour){
    uint8_t GPO_lsb = EXTERN_PWR;
    uint8_t GPO_msb = led_1_colour | (uint8_t)(led_2_colour<<2);
    uint8_t payload[]={0x0X,0x02,GPO_lsb,GPO_msb};
    Kobuki_Tx(payload,sizeof(payload));
}

// =========================================================
// DRIVE FORWARD / BACKWARD
// Positive speed = forward
// Negative speed = backward
// =========================================================

// void Kobuki_Drive(int16_t speed)
// {
//     // Construct Kobuki base-control payload
//     //
//     // Byte 0: Command ID
//     // Byte 1: Command length
//     // Byte 2: Speed LSB
//     // Byte 3: Speed MSB
//     // Byte 4: Radius LSB
//     // Byte 5: Radius MSB
//     //
//     // Radius = 0x0000 means drive straight

//     uint8_t payload[] =
//     {
//         0x01,
//         0x04,
//         0x00,
//         0x00,
//         0x00,
//         0x00
//     };


//     // -----------------------------------------------------
//     // SATURATE SPEED TO SAFE LIMIT
//     // -----------------------------------------------------

//     if (speed > SPEED_LIMIT)
//     {
//         speed = SPEED_LIMIT;
//     }

//     else if (speed < -SPEED_LIMIT)
//     {
//         speed = (~SPEED_LIMIT) + 1;
//     }


//     // -----------------------------------------------------
//     // SPLIT 16-BIT SPEED INTO LITTLE-ENDIAN BYTES
//     // -----------------------------------------------------

//     // Least significant byte first
//     payload[2] = (uint8_t)speed;

//     // Most significant byte second
//     payload[3] = (uint8_t)(speed >> 8);


//     // Send complete payload
//     Kobuki_Tx(payload, sizeof(payload));
// }


// // =========================================================
// // ROTATE
// // Positive speed = counter-clockwise
// // Negative speed = clockwise
// // =========================================================

// void Kobuki_Rotate(int16_t speed)
// {
//     // Radius = 0x0001 creates rotation behaviour

//     uint8_t payload[] =
//     {
//         0x01,
//         0x04,
//         0x00,
//         0x00,
//         0x01,
//         0x00
//     };


//     // -----------------------------------------------------
//     // SATURATE SPEED TO SAFE LIMIT
//     // -----------------------------------------------------

//     if (speed > SPEED_LIMIT)
//     {
//         speed = SPEED_LIMIT;
//     }

//     else if (speed < -SPEED_LIMIT)
//     {
//         speed = (~SPEED_LIMIT) + 1;
//     }


//     // -----------------------------------------------------
//     // SPLIT 16-BIT SPEED INTO LITTLE-ENDIAN BYTES
//     // -----------------------------------------------------

//     payload[2] = (uint8_t)speed;

//     payload[3] = (uint8_t)(speed >> 8);


//     // Send complete payload
//     Kobuki_Tx(payload, sizeof(payload));
// }