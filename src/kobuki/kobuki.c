#include <stdint.h>

#include <uart.h>
#include <kobuki.h>

// =========================================================
// KOBUKI TRANSMIT
// =========================================================

void Kobuki_Tx(uint8_t *payload, uint8_t size_payload)
{
    uint8_t checksum = 0;
    uint8_t i;

    // Send fixed Kobuki packet headers
    UART_txByte(0xAA);
    UART_txByte(0x55);

    // Send payload size
    UART_txByte(size_payload);

    // Start running checksum
    checksum ^= size_payload;

    // Send each payload byte
    for (i = 0; i < size_payload; i++)
    {
        UART_txByte(payload[i]);

        // XOR each payload byte into checksum
        checksum ^= payload[i];
    }

    // Send final checksum
    UART_txByte(checksum);
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
        feedback_header = UART_rxByte();

        if (feedback_header == 0xAA)
        {
            feedback_header = UART_rxByte();

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

    size_payload = UART_rxByte();

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
        feedback[i] = UART_rxByte();

        checksum ^= feedback[i];
    }


    // -----------------------------------------------------
    // 5. READ REMAINING PAYLOAD BYTES
    //    We do not store them, but they must still be
    //    included in the checksum calculation.
    // -----------------------------------------------------

    for (i = limit; i < size_payload; i++)
    {
        checksum ^= UART_rxByte();
    }


    // -----------------------------------------------------
    // 6. VERIFY CHECKSUM
    // -----------------------------------------------------

    rx_checksum = UART_rxByte();

    // 0 = success
    // 1 = checksum error
    return (checksum != rx_checksum);
}


// =========================================================
// READ KOBUKI SENSOR DATA
// =========================================================

void Kobuki_Read(Kobuki_Typedef *kobuki)
{
    uint8_t kobuki_rx_buffer[SIZE_FEEDBACK];

    uint8_t checksum_result;


    // Receive Kobuki bytestream
    checksum_result =
        Kobuki_Rx(kobuki_rx_buffer, SIZE_FEEDBACK);


    // Only update sensor variables if checksum is correct
    if (checksum_result == 0)
    {
        kobuki->bumper =
            kobuki_rx_buffer[BUMPER];

        kobuki->wheeldrop =
            kobuki_rx_buffer[WHEEL_DROP];

        kobuki->cliff =
            kobuki_rx_buffer[CLIFF];

        kobuki->button =
            kobuki_rx_buffer[BUTTON];
    }

    // If checksum fails, retain previous sensor values
    else
    {
        // Do nothing
    }
}


// =========================================================
// DRIVE FORWARD / BACKWARD
// Positive speed = forward
// Negative speed = backward
// =========================================================

void Kobuki_Drive(int16_t speed)
{
    // Construct Kobuki base-control payload
    //
    // Byte 0: Command ID
    // Byte 1: Command length
    // Byte 2: Speed LSB
    // Byte 3: Speed MSB
    // Byte 4: Radius LSB
    // Byte 5: Radius MSB
    //
    // Radius = 0x0000 means drive straight

    uint8_t payload[] =
    {
        0x01,
        0x04,
        0x00,
        0x00,
        0x00,
        0x00
    };


    // -----------------------------------------------------
    // SATURATE SPEED TO SAFE LIMIT
    // -----------------------------------------------------

    if (speed > SPEED_LIMIT)
    {
        speed = SPEED_LIMIT;
    }

    else if (speed < -SPEED_LIMIT)
    {
        speed = (~SPEED_LIMIT) + 1;
    }


    // -----------------------------------------------------
    // SPLIT 16-BIT SPEED INTO LITTLE-ENDIAN BYTES
    // -----------------------------------------------------

    // Least significant byte first
    payload[2] = (uint8_t)speed;

    // Most significant byte second
    payload[3] = (uint8_t)(speed >> 8);


    // Send complete payload
    Kobuki_Tx(payload, sizeof(payload));
}


// =========================================================
// ROTATE
// Positive speed = counter-clockwise
// Negative speed = clockwise
// =========================================================

void Kobuki_Rotate(int16_t speed)
{
    // Radius = 0x0001 creates rotation behaviour

    uint8_t payload[] =
    {
        0x01,
        0x04,
        0x00,
        0x00,
        0x01,
        0x00
    };


    // -----------------------------------------------------
    // SATURATE SPEED TO SAFE LIMIT
    // -----------------------------------------------------

    if (speed > SPEED_LIMIT)
    {
        speed = SPEED_LIMIT;
    }

    else if (speed < -SPEED_LIMIT)
    {
        speed = (~SPEED_LIMIT) + 1;
    }


    // -----------------------------------------------------
    // SPLIT 16-BIT SPEED INTO LITTLE-ENDIAN BYTES
    // -----------------------------------------------------

    payload[2] = (uint8_t)speed;

    payload[3] = (uint8_t)(speed >> 8);


    // Send complete payload
    Kobuki_Tx(payload, sizeof(payload));
}