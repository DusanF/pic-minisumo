#ifndef IR_SENS_H
#define	IR_SENS_H

#define IR_L_ON PWM6DCH = IR_DC_ON		//macros for starting and stopping PWM on left/right LEDs of sensor
#define IR_L_OFF PWM6DCH = 0
#define IR_R_ON PWM7DCH = IR_DC_ON
#define IR_R_OFF PWM7DCH = 0


void ir_init(void);


#endif	/* IR_SENS_H */

