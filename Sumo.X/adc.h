#ifndef ADC_H
#define	ADC_H


void adc_init(void);
void adc_meas_batt(void);
uint8_t adc_read(uint8_t);
void adc_convert(uint8_t channel);

#endif	/* ADC_H */

