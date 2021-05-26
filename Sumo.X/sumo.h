#ifndef SUMO_H
#define SUMO_H

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 32000000

#define P_LED LATB5		//used GPIOs
#define P_BTN !RB4

#define P_MOT_LB LATC5
#define P_MOT_LF LATC4
#define P_MOT_RF LATB3
#define P_MOT_RB LATB2
#define P_IR_L LATC1
#define P_IR_R LATC0
#define P_IR_DEC !RA1
#define P_CNY_ON LATC3
#define P_IO1L LATA6
#define P_IO2L LATC7
#define P_IO3L LATC6
#define P_IO1P RA6
#define P_IO2P RC7
#define P_IO3P RC6

//V_batt - RB1
//CNY_L - RC2
//CNY_R - RA0
//SPEAKER - IO1 - RA6
//RX1 - IO2 - RC7
//TX1 - IO3 - RC6

#define VBATT_CH 0x09	//ADC channels
#define CNY_CH_L 0x12
#define CNY_CH_R 0x00

#define IR_DC_ON 0x69	//IR LED duty cycle

#define CNY_BLK 50      //black treshold
#define CNY_WHITE 100	//white treshold


void pin_init(void);
void btn_ioc(void);


#endif
