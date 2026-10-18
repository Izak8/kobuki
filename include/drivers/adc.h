#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define ADC_CHANNEL_1 ((uint8_t)0x1U)

typedef enum {
	CH0,
	CH1,
	CH2,
	CH3,
	CH4,
	CH5,
	CH6,
	CH7,
	CH8,
	CH9,
	CH10,
	CH11,
	CH12,
	CH13,
	CH14,
	CH15,
    CH16,
    CH17,
    CH18,
} ADC_Channel;

void ADC_init(void);
uint16_t ADC_readChannel(ADC_Channel channel);

#endif