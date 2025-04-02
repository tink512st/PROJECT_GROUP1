#ifndef _UART_MODE1_H
#define _UART_MODE1_H
uart_init(){
	SM0 = 0; SM1 = 1;
	TMOD |= 0x20;
	TH1 = 0xFD;
	TR1 = 1;
	REN = 1;
}
uart_write( char x){
	SBUF = x;
	while(TI == 0);
	TI = 0;	
}
uart_write_text(char *s){
	unsigned char i = 0;
	while(s[i]){
		uart_write(s[i]);
		i++;
	}
}
unsigned char uart_data_ready(){
	return RI;
}
char uart_read(){
	
	RI = 0;
	return SBUF;
}

#endif