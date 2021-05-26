#include <xc.h>
#include <stdint.h>

void mot_init(void){	//initialize PWM for motors control
	T4PR = 0x3F;		//15.6kHz
	T4CON = 0xB0;		//ON, PRE:8
	T4CLKCON = 0x01;	//Fosc/4

	CCPTMRS0 = 0xAA;	//CCP1 - CCP4 > TMR3/TMR4

	CCP1CON = 0x8C;		//ON, PWM, rALIGN
	CCPR1L = 0;
	CCPR1H = 0;

	CCP2CON = 0x8C;		//ON, PWM, rALIGN
	CCPR2L = 0;
	CCPR2H = 0;

	CCP3CON = 0x8C;		//ON, PWM, rALIGN
	CCPR3L = 0;
	CCPR3H = 0;

	CCP4CON = 0x8C;		//ON, PWM, rALIGN
	CCPR4L = 0;
	CCPR4H = 0;

	RC5PPS = 0x0B;
	RB3PPS = 0x0A;
	RC4PPS = 0x09;
	RB2PPS = 0x0C;
}

void mot_brake(){	//braking on both motors
	CCPR1L = 255;
	CCPR2L = 255;
	CCPR3L = 255;
	CCPR4L = 255;
}

void mot_coast(){	//both motors off (no braking)
	CCPR1L = 0;
	CCPR2L = 0;
	CCPR3L = 0;
	CCPR4L = 0;
}

void mot_set_l(int8_t speed){	//set left motor speed
		if(speed>0){				//fwd
		CCPR1L = 127 + speed;
		CCPR3L = 0;
	}else if(speed == 0){			//coast
		CCPR1L = 0;
		CCPR3L = 0;
	}else if((char) speed == 0x80){	//brake
		CCPR1L = 255;
		CCPR3L = 255;
	} else{							//rev
		CCPR1L = 0;
		CCPR3L = 127+(0 - speed);
	}
}

void mot_set_r(int8_t speed){	//set right motor speed
	if(speed>0){					//fwd
		CCPR2L = 127 + speed;
		CCPR4L = 0;
	}else if(speed == 0){			//coast
		CCPR2L = 0;
		CCPR4L = 0;
	}else if((char) speed == 0x80){	//brake
		CCPR2L = 255;
		CCPR4L = 255;
	} else{							//rev
		CCPR2L = 0;
		CCPR4L = 127+(0 - speed);
	}
}
