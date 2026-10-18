#include <stdint.h>

#include <stm32g4xx.h>

#include <uart.h>


void UART_init(void)
{
    // 1. ENABLE PERIPHERAL CLOCKS
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;

    // 2. ROUTE HSI16 CLOCK TO USART3
    RCC->CCIPR &= ~RCC_CCIPR_USART3SEL;
    RCC->CCIPR |= (2 << RCC_CCIPR_USART3SEL_Pos);

    // 3. CONFIGURE PC10 AND PC11 AS ALTERNATE FUNCTION
    GPIOC->MODER &= ~(GPIO_MODER_MODE10 |
                      GPIO_MODER_MODE11);

    GPIOC->MODER |= (GPIO_MODER_MODE10_1 |
                     GPIO_MODER_MODE11_1);

    // 4. SET PC10 AND PC11 TO ALTERNATE FUNCTION 7
    GPIOC->AFR[1] &= ~((0xF << GPIO_AFRH_AFSEL10_Pos) |
                       (0xF << GPIO_AFRH_AFSEL11_Pos));

    GPIOC->AFR[1] |= ((7 << GPIO_AFRH_AFSEL10_Pos) |
                      (7 << GPIO_AFRH_AFSEL11_Pos));

    // 5. DISABLE USART3 WHILE CONFIGURING
    USART3->CR1 &= ~USART_CR1_UE;

    // 6. SET BAUD RATE TO 115200
    // HSI16 = 16 MHz
    USART3->BRR = 139;

    // 7. ENABLE TRANSMITTER AND RECEIVER
    USART3->CR1 |= (USART_CR1_TE |
                    USART_CR1_RE);

    // 8. ENABLE USART3
    USART3->CR1 |= USART_CR1_UE;
}

void UART_txByte(uint8_t byte)
{
    // Wait until Transmit Data Register is empty
    while (!(USART3->ISR & USART_ISR_TXE));

    // Write one byte to the transmit register
    USART3->TDR = byte;
}

uint8_t UART_rxByte(void)
{
    // If an overrun error occurs, clear it
    if (USART3->ISR & USART_ISR_ORE)
    {
        USART3->ICR = USART_ICR_ORECF;
    }

    // Wait until received data is available
    while (!(USART3->ISR & USART_ISR_RXNE));

    // Read and return one byte
    return (uint8_t)(USART3->RDR & 0xFF);
}