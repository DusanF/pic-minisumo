#include <xc.h>
#include <stdint.h>

void serial_init(){		//USART1
	SP1BRGH = 0;
	SP1BRGL = 68;		//115200 baud
	BAUD1CON = 0x08;	//brg16
	RC1STA = 0x90;		//spen, cren
	TX1STA = 0x24;		//txen, brgh

	RC6PPS = 0x0F;		//TX1
}

void serial_ch(char data){	//transmit char
	while(!TX1IF);
	TX1REG = data;
}

void serial_val_u8(uint8_t data){	//transmit u8, converted to string
	char tmp;

	tmp = '0';
	while(data >= 100){
		tmp++;
		data-=100;
	}
	serial_ch(tmp);

	tmp = '0';
	while(data >= 10){
		tmp++;
		data-=10;
	}
	serial_ch(tmp);

	serial_ch('0' + data);

	serial_ch(10);
}

void serial_str(const char * str){	//transmit string
	uint8_t i=0;

	char ch = *str;
	while(ch != 0){
		serial_ch(ch);
		i++;
		ch = *(str+i);
	}
	serial_ch(10);
}
