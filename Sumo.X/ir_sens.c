#include <xc.h>

void ir_init(void){	//configure PWM frequency and channels for enemy presence sensor IR LEDs
	T2PR = 209;			//38kHz
	T2CON = 0x80;		//ON
	T2CLKCON = 0x01;	//Fosc/4

	
	PWM6DCL = 0;		//IR-L
	PWM6DCH = 0;
	PWM6CON = 0x80;

	PWM7DCL = 0;		//IR-R
	PWM7DCH = 0;
	PWM7CON = 0x80;

	RC1PPS = 0x0D;		//IR-L - PWM6
	RC0PPS = 0x0E;		//IR-R - PWM7
}
