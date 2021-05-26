#ifndef NCO_H
#define	NCO_H

#include <xc.h>
#include "sumo.h"

void nco_init(void);
void nco_start(void);
void nco_stop(void);
void nco_freq(uint16_t freq);
void nco_play_short(void);
void nco_play_startup(void);
void nco_play_alarm1(uint8_t count);
void nco_play_alarm2(uint8_t count);
void nco_play_low(void);
void nco_play_high(void);
void nco_play_val_u16(uint16_t val);

#endif	/* NCO_H */

