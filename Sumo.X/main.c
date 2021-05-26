// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1)
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (FSCM timer disabled)

// CONFIG2
#pragma config MCLRE = OFF      // Master Clear Enable bit (MCLR pin function is port defined function)
#pragma config PWRTS = PWRT_16  // Power-up Timer Enable bit (PWRT set at 16 ms)
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = NSLEEP   // Brown-out reset enable bits (Brown-out Reset enabled while running, disabled in sleep; SBOREN is ignored)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V)
#pragma config ZCDDIS = OFF     // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = OFF    // Peripheral Pin Select one-way control (The PPSLOCK bit can be set and cleared repeatedly by software)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled, SWDTEN is ignored)
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4
#pragma config BBSIZE = BB512   // Boot Block Size Selection bits (512 words boot block size)
#pragma config BBEN = OFF       // Boot Block Enable bit (Boot Block disabled)
#pragma config SAFEN = OFF      // SAF Enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block Write Protection bit (Application Block not write protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block not write protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration Register not write protected)
#pragma config WRTD = OFF       // Data EEPROM write protection bit (Data EEPROM NOT write protected)
#pragma config WRTSAF = OFF     // Storage Area Flash Write Protection bit (SAF not write protected)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (High Voltage on MCLR/Vpp must be used for programming)

// CONFIG5
#pragma config CP = OFF         // UserNVM Program memory code protection bit (UserNVM code protection disabled)

#include <xc.h>
#include <stdint.h>
#include "sumo.h"
#include "adc.h"
#include "ir_sens.h"
#include "motor.h"
#include "nco.h"
//#include "serial.h"
#include "timer.h"

#define SPEED_SEARCH_L 185
#define SPEED_SEARCH_R 185
#define SPEED_RUNAWAY 255
#define SPEED_CHASE 245
#define SPEED_CHASE_TURN 100

#define TIME_TURN_MIN 50
#define TIME_TURN_SCAN 20
#define TIME_RUNAWAY 100

#define IR_TIMEOUT 3
#define IR_COOLDOWN	1
#define IR_FORGET 3

#define BAT_LOW_THR 104	//cca 7V Vbat (7Vbat - 0.35Vfwd = 6.65)


//#define BLACK_EDGE
//#define SOUND_ON

enum {
	STATE_SEARCH, STATE_EDGE_TURN_MIN, STATE_EDGE_TURN_SCANNING, STATE_EDGE_RUNAWAY, STATE_CHASE, STATE_STOP
};

volatile __bit cny_l, cny_r;	//flags for line sensors
volatile __bit btn_int;			//flag for user button pressed
__bit adc_step;		//line sensor side (0=L, 1=R) - ring edge
volatile __bit alarm_cny, alarm_ir, state_update;	//flags for sensors alarms
uint8_t ir_step=0;	//step of enemy sensor reading
volatile uint8_t state=STATE_STOP;	//current state
volatile uint8_t ir_l=0, ir_r=0;	//state of line sensors
volatile uint8_t state_timer=0;		//SW timer used for changing states, automatically decremented in ISR

__bit start_alt;	//alternative start - quickly back off to one side after start, usefull if enemy is facing head-on

void btn_wait(){	//wait for user to press button
	mot_brake();
	while(P_BTN)		//if already pressed, wait for release
		__delay_ms(5);

	btn_int = 0;

	while(!btn_int){	//blink while waiting for press
		P_LED = 1;
		__delay_ms(5);
		P_LED = 0;
		__delay_ms(50);
	}

	nco_start();		//play tone
	nco_freq(1500);
	__delay_ms(100);
	nco_stop();

	while(P_BTN)		//wait for button release
		__delay_ms(5);
	
	if(ir_l && ir_r)	//if enemy/hand is in front of robot during button release, use alternative start
		start_alt = 1;
	else
		start_alt = 0;

	btn_int = 0;

	nco_start();		//play tone
	nco_freq(2000);
	__delay_ms(200);
	nco_stop();

	mot_coast();
}

void __interrupt() isr(void){	//interrupt service routine
	if(TMR0IF){				//timer 0 interrupt - enemy sensor reading
		if(ir_step == 0){	//turn on left LED
			IR_L_ON;
			ir_step++;
		}
		else if(ir_step == 1){	//check left side
			if(P_IR_DEC){
				ir_l = IR_TIMEOUT;	//sensor measurement is valid for some time
				if(ir_r < IR_FORGET)	//if right side was not active recently, enemy has moved to the left
					ir_r = 0;
			}
			else		//forget detection of enemy after some time
				if(ir_l)
					ir_l--;

			IR_L_OFF;
			ir_step++;
		}
		else if(ir_step == 2 + IR_COOLDOWN){	//turn on right LED
			IR_R_ON;
			ir_step++;
		}
		else if(ir_step == 3 + IR_COOLDOWN){	//check right side
			if(P_IR_DEC){
				ir_r = IR_TIMEOUT;
				if(ir_l < IR_FORGET)
					ir_l = 0;
			}
			else
				if(ir_r)
					ir_r--;

			IR_R_OFF;
			ir_step = 256 - IR_COOLDOWN;
		}
		else
			ir_step++;

		if(ir_l || ir_r)
			alarm_ir = 1;	//set flag if enemy is detected on either side
		else
			alarm_ir = 0;

		TMR0IF=0;	//clear interrupt flag
	}

	if(ADIF){	//line sensor
		if(!adc_step){	//left sensor

#ifdef BLACK_EDGE
			if(ADRESH < CNY_BLK){
#else
			if(ADRESH > CNY_WHITE){
#endif
				cny_l = 1;
				alarm_cny = 1;
			}
			else
				cny_l = 0;
			adc_convert(CNY_CH_R);	//start conversion on other side
			adc_step = 1;
		}
		else{	//right side

#ifdef BLACK_EDGE
			if(ADRESH < CNY_BLK){
#else
			if(ADRESH > CNY_WHITE){
#endif
				cny_r = 1;
				alarm_cny = 1;
			}
			else
				cny_r = 0;

			adc_convert(CNY_CH_L);	//start conversion on other side
			adc_step = 0;
		}
		ADIF = 0;
	}

	if(IOCIF){		//user button pressed
		btn_int = 1;
		IOCBF = 0;
	}

	if(TMR1IF){		//timer 2
		if(state_timer){	//if software timer is set, decrement
			state_timer--;
			state_update = 1;	//sw timer changed value, set flag
		}
		TMR1IF = 0;
	}
}

void main(void) {
	pin_init();
	adc_init();
	mot_init();
	ir_init();
	tmr0_init();
	tmr1_init();
	nco_init();
	btn_ioc();

	PIR0 = 0x00;	//clear interrupt request and unmask int. sources
	PIR1 = 0x00;
	PIR4 = 0x00;
	PIE0 = 0x30;	//TMR0, IOC
	PIE1 = 0x01;	//ADC
	PIE4 = 0x01;	//TMR1

	nco_play_short();

	__delay_ms(1000);
	
	adc_meas_batt();	//check battery voltage
	if(ADRESH < BAT_LOW_THR){
		nco_freq(300);	//battery low, beep 5 times
		for(uint8_t i=0; i<5; i++){
			P_LED = 1;
			nco_start();
			__delay_ms(500);
			P_LED = 0;
			nco_stop();
			__delay_ms(500);
		}
		while(1)	//enter sleep mode
			SLEEP();
		
	}

	P_LED = 1;
	nco_play_startup();
	P_LED = 0;

	P_CNY_ON = 1;	//enable line sensors

	btn_int = 1;				//inicializacia premennych
	alarm_cny = 0;
	alarm_ir = 0;
	state_update = 1;

	INTCON = 0xC0;	//enable global interrupts

	adc_convert(CNY_CH_L);		//start first ADC conversion


	while(1){
		if(btn_int){	//button pressed, stop
			state = STATE_STOP;
			state_update = 1;
		}
		else{
			if(alarm_cny){	//edge of ring
				state = STATE_EDGE_TURN_MIN;	//stop and start turning around
				state_timer = TIME_TURN_MIN;
				state_update = 1;
			}
			else if(state != STATE_EDGE_TURN_MIN){
				if(alarm_ir){
					state = STATE_CHASE;	//enemy detected, start chasing
					state_update = 1;
				}
				else if(state != STATE_STOP){	//stop turning, search for enemy
					state = STATE_SEARCH;
					CCPR1L = SPEED_SEARCH_L;
					CCPR2L = SPEED_SEARCH_R;
					CCPR3L = 0;
					CCPR4L = 0;
#ifdef SOUND_ON
					nco_stop();
#endif
					P_LED = 0;
				}
			}
		}

		if(state_update){	//state changed, do something
			switch(state){
				case STATE_EDGE_TURN_MIN:	//turn away from edge
					alarm_cny = 0;
					if(state_timer == TIME_TURN_MIN){
						P_LED = 1;
#ifdef SOUND_ON
						nco_start();
#endif
						if(cny_l && cny_r){
							CCPR1L = 0;
							CCPR2L = 0;
							CCPR3L = 255;
							CCPR4L = 255;
#ifdef SOUND_ON
							nco_freq(400);
#endif
						}
						else{
							if(cny_l){	//line on left side
								CCPR1L = SPEED_SEARCH_L;
								CCPR2L = 0;
								CCPR3L = 0;
								CCPR4L = 255;
#ifdef SOUND_ON
								nco_freq(600);
#endif
							}
							else{	//line on right
								CCPR1L = 0;
								CCPR2L = SPEED_SEARCH_R;
								CCPR3L = 255;
								CCPR4L = 0;
#ifdef SOUND_ON
								nco_freq(700);
#endif
							}
						}
					}
					else if(state_timer == 0){	//continue turning away from edge but start looking for enemy
						alarm_cny = 0;
						alarm_ir = 0;
						state = STATE_EDGE_TURN_SCANNING;
						state_timer = TIME_TURN_SCAN;
					}
					break;

				case STATE_EDGE_TURN_SCANNING:
					if(state_timer == 0){	//run away from edge faster
						CCPR1L = SPEED_RUNAWAY;
						CCPR2L = SPEED_RUNAWAY;
						CCPR3L = 0;
						CCPR4L = 0;
						state_timer = TIME_RUNAWAY;
						state = STATE_EDGE_RUNAWAY;
					}
					break;

				case STATE_EDGE_RUNAWAY:
#ifdef SOUND_ON
					nco_stop();
#endif
					if(state_timer == 0){	//clear of edge, back to searching for enemy
						CCPR1L = SPEED_SEARCH_L;
						CCPR2L = SPEED_SEARCH_R;
						CCPR3L = 0;
						CCPR4L = 0;
						state = STATE_SEARCH;
					}
					P_LED = 0;
					break;

				case STATE_CHASE:	//chasing enemy
					P_LED = 1;
#ifdef SOUND_ON
					nco_start();
#endif
					CCPR3L = 0;
					CCPR4L = 0;

					if(ir_l && ir_r){	//enemy in front, go straight
						CCPR1L = SPEED_CHASE;
						CCPR2L = SPEED_CHASE;
#ifdef SOUND_ON
						nco_freq(5000);
#endif
					}
					else if(ir_l){	//enemy to the left, turn
						CCPR1L = SPEED_CHASE_TURN;
						CCPR2L = 255;
#ifdef SOUND_ON
						nco_freq(3000);
#endif
					}
					else if(ir_r){	//enemy to the right, turn
						CCPR1L = 255;
						CCPR2L = SPEED_CHASE_TURN;
#ifdef SOUND_ON
						nco_freq(4000);
#endif
					}
					else{	//move slower, look for enemy
						state = STATE_SEARCH;
						CCPR1L = SPEED_SEARCH_L;
						CCPR2L = SPEED_SEARCH_R;
						P_LED = 0;
#ifdef SOUND_ON
						nco_stop();
#endif
					}
					break;

				case STATE_STOP:	//stop, wait for next start
					P_LED = 1;
					nco_play_alarm1(2);
					btn_wait();		//wait for button
					if(start_alt)
						P_LED = 1;

					//__delay_ms(4100);
					state_timer = 100;
					while(state_timer);
					
					uint8_t i=10;	//countdown
					
					while(i--){
						state_timer = 220;
						while(state_timer){
							P_LED = 1;
							__delay_ms(20);
							P_LED = 0;
							__delay_ms(100);
						}
					}
					
					nco_play_alarm2(1);
					P_LED = 0;
					
					if(start_alt){	//alternative start - back away to the side, let enemy pass by, then continue
						CCPR1L = 0;	//reverse to one side
						CCPR2L = 0;
						CCPR3L = 180;
						CCPR4L = 255;
						__delay_ms(270);
						
						CCPR1L = 0;	//reverse straight
						CCPR2L = 0;
						CCPR3L = 255;
						CCPR4L = 255;
						__delay_ms(200);

						CCPR1L = 255;	//brake
						CCPR2L = 255;
						CCPR3L = 255;
						CCPR4L = 255;
						__delay_ms(100);
					}

					P_LED = 0;
					
					state = STATE_SEARCH;	//start search for enemy
					alarm_cny = 0;
					CCPR1L = SPEED_SEARCH_L;
					CCPR2L = SPEED_SEARCH_R;
					CCPR3L = 0;
					CCPR4L = 0;

					break;
			}
			state_update = 0;
		}
	}
}
