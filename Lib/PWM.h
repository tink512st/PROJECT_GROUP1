sbit pwm = P2^0;

int T,Ton,Toff,Ton_H,Ton_L,Toff_H,Toff_L;

void pwm_init(unsigned int ck){
	TMOD &=0xF0;
	TMOD |= 0x01;
	T = ck;
	ET0 = 1;
	EA = 1;
}
void set_duty(unsigned int dt){//tinh theo us
	Ton = ((unsigned long)T) * dt / 100;
	Toff = T - Ton;
	Ton_H=(65536 - Ton)>>8;
	Ton_L=(65536-Ton) & 0x00FF;
	
	Toff_H = (65536 - Toff)>>8;
	Toff_L= (65536 - Toff) & 0x00FF;
}
void pwm_start(){
	TR0 = 1;
	pwm = 1;
}
void pwm_stop(){
	TR0 = 0;
}
void ngat() interrupt 1{
	pwm = !pwm;
	if(pwm==0){
		TH0 = Toff_H;
		TL0 = Toff_L;
	}
	else {
		TH0 = Ton_H;
		TL0 = Ton_L;
	}
}