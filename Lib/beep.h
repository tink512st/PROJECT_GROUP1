sbit buzzer = P2^5;
void delay_beep2(unsigned int time){
	while(time--){};
}
void beep(unsigned int period, unsigned char t){ 
	unsigned int i;
	for( i = 0; i < t;i++){
		buzzer = 1;
		delay_beep2(period/2);
		buzzer = 0;
		delay_beep2(period/2);
	}
}
void beep_delay(){
	beep(70,50);
}
