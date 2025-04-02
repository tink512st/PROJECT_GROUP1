sbit dht11 = P3^0;

void delay_20ms(){
	TMOD = 0x01;
	TH0 = 0xB1;
	TL0 = 0xE0;
	TR0 = 1;
	while(!TF0);
	TF0=0;
	TR0 = 0;
}
void delay_30us(){
	TMOD = 0x01;
	TH0 = 0xFF;
	TL0 = 0xE2;
	TR0 = 1;
	while(!TF0);
	TF0=0;
	TR0=0;
}
void request(){
	dht11 = 0;
	delay_20ms();
	dht11=1;
}
void respone(){
	while(dht11==1);
	while(dht11==0);
	while(dht11);
}
int receive_8bit(){
	int i;
	unsigned char c;
	for(i = 0;i<8;i++){
		while(dht11==0);
		delay_30us();
		if(dht11==1){
			c=(c<<1)|0x01;
		}
		else c=(c<<1);
		while(dht11);
	}
	return c;
}