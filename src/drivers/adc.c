#include <stdint.h>

#include <stm32g4xx.h>

#include <adc.h>

void ADC_init(void) {
    RCC->AHB2ENR |= 0x1 << 13;
 
    ADC12_COMMON->CCR &= ~ADC_CCR_CKMODE;
    ADC12_COMMON->CCR |= ADC_CCR_CKMODE_0;

    ADC1->CR &= ~(0x1 << 29);
    ADC1->CR |= 0x1 << 28;

    for(volatile uint32_t  i = 0; i < 1000; i++) {
        __NOP();
    }

    // set to single-ended mode (clear ADCALDIF to 0)
    ADC1->CR &= ~(0x1 << 30);

    while((ADC1->CR & ADC_CR_ADCAL) != 0);

    ADC1->ISR = 0x1 << 0; // write 1 to clear ADRDY
    ADC1->CR |= 0x1 << 0; // write 1 to enable ADC
    while((ADC1->ISR & ADC_ISR_ADRDY) == 0); // wait for ADRDY to be set
}

uint16_t ADC_readChannel(ADC_Channel channel) {
    if((channel < 1U) || (channel > 18U)) {
        return 0;
    }

    ADC1->SQR1 = ((uint32_t)channel << 6); // set the channel to read
    if(channel <= 9U) {
        #define POSITION1 channel * 3U
        ADC1->SMPR1 &= ~(0x7U << POSITION1);
        ADC1->SMPR1 |= 0x7U << POSITION1;
    }
    else {
        #define POSITION2 (channel - 10U) * 3U
        ADC1->SMPR2 &= ~(0x7U << POSITION2);
        ADC1->SMPR2 |= (0x7U << POSITION2);
    }

    ADC1->ISR = ADC_ISR_EOC | ADC_ISR_EOS;
    ADC1->CR |=  0x1 << 2; // start conversion

    while((ADC1->ISR & ADC_ISR_EOC) == 0); // wait for EOC to be set

    return (uint16_t)(ADC1->DR & ADC_DR_RDATA);
}
