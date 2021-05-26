#include "sumo.h"

void adc_init(){
	ADCON0 = 0x80;
	ADCLK = 15;		//Fosc/16
	ADACQ = 500;	//500 Tad acq time
	FVRCON = 0x81;
}

void adc_meas_batt(void){	//change channel and measure battery voltage, waiting for completion
	ADACQL = 10;	//10 Tad acq time
	ADREF = 0x03;
	ADPCH = VBATT_CH;

	GO_nDONE = 1;
	while(GO_nDONE)
		;
	ADREF = 0x00;
}

uint8_t adc_read(uint8_t channel){	//one measurement on selected channel
	ADACQL = 10;	//10 Tad acq time
	ADPCH = channel;
	GO_nDONE = 1;
	while(GO_nDONE)
		;
	return ADRESH;
}

void adc_convert(uint8_t channel){	//start conversion on channel
	ADPCH = channel;
	GO_nDONE = 1;
}
