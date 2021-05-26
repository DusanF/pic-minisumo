#include <xc.h>
#include "sumo.h"
#include "nco.h"


void nco_init(void){	//NCO (numerically controlled oscillator) init
	RA6PPS = 0x18;
	CLKRCON = 0x94;
	NCO1CLK = 0x06;
	NCO1INCU = 0;
}

void nco_start(void){
	NCO1CON = 0x80;
}

void nco_stop(void){
	NCO1CON = 0x00;
	LATA6 = 0;
}

void nco_freq(uint16_t freq){	//apply NCO freq
	NCO1INCH = freq >> 8;
	NCO1INCL = freq & 0xFF;
}

void nco_play_short(void){	//play short beep
	nco_start();

	nco_freq(2000);
	__delay_ms(5);

	nco_stop();
}

void nco_play_startup(void){	//play startup sound
	nco_start();

	nco_freq(1000);
	__delay_ms(100);
	nco_freq(1200);
	__delay_ms(100);
	nco_freq(1400);
	__delay_ms(100);
	nco_freq(1600);
	__delay_ms(100);

	nco_stop();
}

void nco_play_alarm1(uint8_t count){	//beep count times
	uint16_t freq;

	nco_start();
	
	for(uint8_t i=0; i<count; i++){
		freq = 0;
		while(freq++<4500){
			nco_freq(freq);
			__delay_us(25);
		}
	}

	nco_stop();
}

void nco_play_alarm2(uint8_t count){	//different beep
	nco_start();

	while(count--){
		nco_freq(500);
		__delay_ms(120);
		nco_freq(1000);
		__delay_ms(120);
	}

	nco_stop();
}

void nco_play_low(void){	//low pitched tone
	nco_start();
	nco_freq(300);
	__delay_ms(50);
	nco_stop();
}

void nco_play_high(void){	//high pitched tone
	nco_start();
	nco_freq(2000);
	__delay_ms(50);
	nco_stop();
}

void nco_play_val_u16(uint16_t val){	//beep out 16bit value, low pitched tone for 0, high for 1
	char i = 16;
	while(i){
		if(val & 0x8000)
			nco_play_high();
		else
			nco_play_low();
		__delay_ms(800);
		val = val << 1;
		i--;
	}
}
