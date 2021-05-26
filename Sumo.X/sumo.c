#include <xc.h>
#include "sumo.h"

void pin_init(){	//set GPIOs
	ANSELA = 0x01;
	TRISA = 0x03;
	LATA = 0x00;
	SLRCONA = 0;
	WPUA = 0x02;

	ANSELB = 0x02;
	TRISB = 0x12;
	LATB = 0;
	SLRCONB = 0;

	ANSELC = 0x04;
	TRISC = 0x84;
	LATC = 0;
	SLRCONC = 0;
}

void btn_ioc(void){	//enable interrupt on change for user button
	IOCBF = 0x00;
	IOCBN = 0x10;
}
