#include "stm32g474xx.h"
#include <gpio.h>

#include <stm32g4xx.h>

void GPIO_initPort(GPIO_TypeDef *port) {
    // based on the port's address
    // determine which bit to write in IOPENR
	switch((uint32_t)(port)) {
        case GPIOA_BASE:
            RCC->AHB2ENR |= (0x1 << 0);
            break;
        case GPIOB_BASE:
            RCC->AHB2ENR |= (0x1 << 1);
            break;
        case GPIOC_BASE:
            RCC->AHB2ENR |= (0x1 << 2);
            break;
        case GPIOD_BASE:
            RCC->AHB2ENR |= (0x1 << 3);
            break;
        case GPIOE_BASE:
            RCC->AHB2ENR |= (0x1 << 4);
            break;
        case GPIOF_BASE:
            RCC->AHB2ENR |= (0x1 << 5);
            break;
        case GPIOG_BASE:
            RCC->AHB2ENR |= (0x1 << 6);
            break;
    }
}

void GPIO_setPinMode(GPIO_TypeDef *port, GPIO_PinNum pin, GPIO_PinMode mode) {
    // Set the mode register
    // Each pin has two bits associated therefore pin * 2 to calculate shift
	uint32_t position = (uint32_t)(pin * 2U);
	port->MODER &= ~(0x3 << position);
	port->MODER |= (uint32_t)(mode) << position;
}

void GPIO_writePin(GPIO_TypeDef *port, GPIO_PinNum pin, GPIO_PinState state) {
    // clear or set based on desired pin state
    // value goes in output data register at given bit position
	if(state == HIGH) {
		port->ODR |= 0x1 << pin;
	}
	else {
		port->ODR &= ~(0x1 << pin);
	}
}

void GPIO_setPinPull(GPIO_TypeDef *port, GPIO_PinNum pin, GPIO_PinPull pull) {
    // Set the pull-up pull-down register
    // Each pin has two bits associated hence pin * 2 to calculate shift
	uint32_t position = (uint32_t)(pin * 2U);
	port->PUPDR &= ~(0x3 << position);
	port->PUPDR |= (uint32_t)(pull) << position;
}

uint8_t GPIO_readPin(GPIO_TypeDef *port, GPIO_PinNum pin) {
    // Read the input data register
	return port->IDR & (0x1 << pin);
}

void GPIO_setPinAltFunc(GPIO_TypeDef *port, GPIO_PinNum pin, GPIO_AltFunc alt) {
    // low or high Alt-func register
    if(pin < 8) {
        port->AFR[0] &= ~(0xFUL << (pin * 4));
        port->AFR[0] |= (uint32_t)(alt) << (pin * 4);
    }
    else {
        port->AFR[1] &= ~(0xFUL << ((pin - 8)* 4));
        port->AFR[1] |= (uint32_t)(alt) << ((pin - 8) * 4);
    }
}