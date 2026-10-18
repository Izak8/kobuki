#include <stdint.h>

#include <stm32g4xx.h>

#include <kobuki.h>
#include <uart.h>
#include <adc.h>
#include <ultrasonic.h>

#define BUTTON_0_START 0x1U
#define BUTTON_1_STOP 0x2U

#define SEARCH_SPEED_MM_S   ((uint16_t)75)
#define FORWARD_SPEED_MM_S  ((uint16_t)100)
#define REVERSE_SPEED_MM_S  ((uint16_t)-100)
#define AVOID_SPEED_MM_S    ((uint16_t)100)

#define AVOID_DELAY_COUNT   300000U

typedef enum {
    IDLE = 0,
    SEARCH,
    OBJECT,
    OBSTACLE,
    AVOID
} RobotState;

static void AvoidDelay(void) {
    for(volatile uint32_t i = 0U; i < AVOID_DELAY_COUNT; ++i) {
    }
}

static void Send_Kobuki_Move(uint16_t speed, uint16_t radius) {
    uint8_t payload[6];

    payload[0] = 0x01U;
    payload[1] = 0x04U;

    payload[2] = (uint8_t)(speed &0xFF);
    payload[3] = (uint8_t)((uint16_t)speed >> 8);
    payload[4] = (uint8_t)(radius & 0xFF);
    payload[5] = (uint8_t)((uint16_t)radius >> 8);

    Kobuki_Tx(payload, sizeof(payload));
}



int main(void)
{
    // Create structure to store Kobuki sensor values
    Kobuki_Typedef my_kobuki = {0};

    // Create structure to store ultrasonic sensor values
    US_Typedef ultrasonic_object = {0};

    RobotState state = IDLE;

    // Initialise UART communication
    UART_init();
    ADC_init();
    Ultrasonic_init();
    Send_Kobuki_Move(0, 0);

    while (1)
    {
        Kobuki_Read(&my_kobuki);
        Ultrasonic_detect(&ultrasonic_object);

        if((my_kobuki.button & BUTTON_1_STOP) != 0U) {
            state=IDLE;
        } else {
            switch(state) {
                case IDLE:
                    if ((my_kobuki.button & BUTTON_0_START) != 0U) {
                        state = SEARCH;
                    }
                    break;
                case SEARCH:
                    if((ultrasonic_object.detected != 0U)) {
                        state = OBJECT;
                    }
                    break;
                case OBJECT:
                    if ((my_kobuki.bumper != 0U) || (my_kobuki.cliff != 0U)) {
                        state = OBSTACLE;
                    } else if (ultrasonic_object.detected == 0U) {
                        state = SEARCH;
                    }
                    break;
                case OBSTACLE:
                    if((my_kobuki.bumper == 0U) && (my_kobuki.cliff == 0U)) {
                        state = AVOID;
                    }
                    break;
                case AVOID:
                    break;
                default:
                    state = IDLE;
                    break;
            }
        }
        
        switch(state) {
            case IDLE:
                Send_Kobuki_Move(0, 0);
                break;
            case SEARCH:
                Kobuki_Rotate(SEARCH_SPEED_MM_S);
                break;
            case OBJECT:
                Kobuki_Drive(FORWARD_SPEED_MM_S);
                break;
            case OBSTACLE:
                Kobuki_Drive(REVERSE_SPEED_MM_S);
                break;
            case AVOID:
                Kobuki_Rotate(AVOID_SPEED_MM_S);
                AvoidDelay();
                state=SEARCH;
                break;
            default:
                Send_Kobuki_Move(0, 0);
                state = IDLE;
                break;
        }
    
        for(volatile uint32_t i = 0U; i < 5000U; ++i) {
        }
    }
}