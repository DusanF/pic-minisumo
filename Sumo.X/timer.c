#include <xc.h>

void tmr0_init(){	//used by enemy presence sensor
	T0CON0 = 0x80;	//ON
	T0CON1 = 0x5A;	//Fosc/4, 1:1024 pre,
	//TMR0H = 136;	// ~35ms
	TMR0H = 194;	// ~50ms
	//TMR0H = 255;	// ~65ms
}

void tmr1_init(){	//used for changing states
	TMR1L = 0;
	TMR1H = 0;
	TMR1CLK = 0x02;	//Fosc ~2ms
	T1CON = 0x01;
}
