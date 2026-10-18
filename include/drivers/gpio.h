#ifndef GPIO_H
#define GPIO_H

#include <stm32g4xx.h>

/* GPIO pin number */
typedef enum {
	PIN0,
	PIN1,
	PIN2,
	PIN3,
	PIN4,
	PIN5,
	PIN6,
	PIN7,
	PIN8,
	PIN9,
	PIN10,
	PIN11,
	PIN12,
	PIN13,
	PIN14,
	PIN15
} GPIO_PinNum;

/* GPIO alternate function number */
typedef enum {
	AF0,
	AF1,
	AF2,
	AF3,
	AF4,
	AF5,
	AF6,
	AF7,
	AF8,
	AF9,
	AF10,
	AF11,
	AF12,
	AF13,
	AF14,
	AF15
} GPIO_AltFunc;

/* GPIO pin state LOW or HIGH */
typedef enum {
	LOW,
	HIGH,
} GPIO_PinState;

/* GPIO pin mode IN, OUT, ALT, or ANLG*/
typedef enum {
	MODE_IN,
	MODE_OUT,
	MODE_ALT,
	MODE_ANLG
} GPIO_PinMode;

/* GPIO pin pull NONE, PULL_UP, PULL_DOWN*/
typedef enum {
	PULL_NONE,
	PULL_UP,
	PULL_DOWN
} GPIO_PinPull;

/**
 * Initialise the clock for the given GPIO port
 *
 * @param port Pointer to the given GPIO port
 */
void GPIO_initPort(GPIO_TypeDef *port);

/**
 * Set the mode of the given GPIO pin
 *
 * @param port Pointer to the GPIO port
 * @param pin Pin number [0, 15]
 * @param mode The pin mode [Output, Input, Alternate Function, Analog]
 */
void GPIO_setPinMode(GPIO_TypeDef *port, GPIO_PinNum pin, GPIO_PinMode mode);

/**
 * Write a given state to a GPIO pin
 *
 * @param port Pointer to the GPIO port
 * @param pin Pin number [0, 15]
 * @param state Pin state [LOW | HIGH]
 */
void GPIO_writePin(GPIO_TypeDef *port, GPIO_PinNum pin, GPIO_PinState state);

/**
 * Select the pull-up / pull-down mode on a given pin
 *
 * @param port Pointer to the GPIO port
 * @param pin Pin number [0, 15]
 * @param pull Pin pull [NONE | PULL_UP | PULL_DOWN ]
 */
void GPIO_setPinPull(GPIO_TypeDef *port, GPIO_PinNum pin, GPIO_PinPull pull);

/**
 * Read the state of a given GPIO pin
 *
 * @param port Pointer to the GPIO port
 * @param pin Pin number [0, 15]
 *
 * @return LOW | HIGH based on the value read from the pin
 */
uint8_t GPIO_readPin(GPIO_TypeDef *port, GPIO_PinNum pin);

/**
 * Set the alternate function number for the given GPIO pin
 *
 * @param port Pointer to the GPIO port
 * @param pin Pin number (see `GPIO_)
 * @param alt Alternate function number
 */
void GPIO_setPinAltFunc(GPIO_TypeDef *port, GPIO_PinNum pin, GPIO_AltFunc alt);

#endif/*GPIO_H*/